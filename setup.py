#!/usr/bin/env python3

'''
Installation script for MuPDF Python bindings.

We operate like a normal setup.py script except that we first build MuPDF's C,
C++ and Python libraries, and list the .so files as data_files when calling
setuptools.setup().

This is unusual, but the only alternative seems to be to somehow get
setuptools.setup() to build MuPDF's C, C++ and Python libraries itself, which
is impractical.

Is there an alternative where we specify a callback to setuptools.setup() which
is called when libraries should be built?
'''

import os
import re
import setuptools
import subprocess
import sys


def log(text=''):
    for line in text.split('\n'):
        print(f'mupdf/setup.py: {line}', file=sys.stdout)
    sys.stdout.flush()

log(f'== sys.argv={sys.argv}')

mupdf_dir = os.path.abspath(f'{__file__}/..')

def mupdf_version():
    with open(f'{mupdf_dir}/include/mupdf/fitz/version.h') as f:
        text = f.read()
    m = re.search('\n#define FZ_VERSION "([^"]+)"\n', text)
    assert m
    version = m.group(1)
    return version


# Information about supported build directories.
#
build_dir_allowed = (
        'build/shared-debug',
        'build/shared-debug-extract',
        'build/shared-release',
        'build/shared-release-extract',
        )
build_dir_default = 'build/shared-release'

def build_dir_allowed_text(indentation):
    '''
    Returns string describing allowed build directories.
    '''
    ret = ''
    for b in build_dir_allowed:
        ret += f'{indentation*" "}{b}\n'
    return ret


# Handle our extra --mupdf-* options. We remove any such options from sys.argv
# after we have handled them so setuptools.setup() does not see them.
#
do_build = 1
build_dir = build_dir_default
show_help = False
show_help_commands = False
i = 1
while 1:
    if i == len(sys.argv):
        break
    if sys.argv[i] == '--mupdf-build':
        do_build = int(sys.argv[i+1])
        del sys.argv[i:i+2]
    elif sys.argv[i] == '--mupdf-build-dir':
        build_dir = sys.argv[i+1]
        if build_dir not in build_dir_allowed:
            raise Exception(f'Unrecognised build-dir {build_dir!r}. Must be one of:\n{build_dir_allowed_text(4)}')
        del sys.argv[i:i+2]
    elif sys.argv[i] in ('-h', '--help'):
        show_help = True
        i += 1
    elif sys.argv[i] in ('--help-commands'):
        show_help_commands = True
        i += 1
    else:
        i += 1

# Extra files that should be copied into install directory.
#
data_files = [
        f'{mupdf_dir}/{build_dir}/libmupdf.so',      # C
        f'{mupdf_dir}/{build_dir}/libmupdfcpp.so',   # C++
        f'{mupdf_dir}/{build_dir}/_mupdf.so',        # Python internals
        ]

if show_help:
    log(
            'Additional options for mupdf:\n'
            +  '    --mupdf-build 0 | 1\n'
            +  '        Whether to build mupdf C, C++ and Python libraries. Default is 1.\n'
            +  '    --mupdf-build-dir\n'
            + f'        Set mupdf build type/directory; must be one of:\n{build_dir_allowed_text(12)}'
            + f'        Default is: {build_dir_default}\n'
            )
elif show_help_commands:
    pass
else:
    # Only build mupdf etc if we are not showing help.
    #
    if do_build:
        command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -d {build_dir} -b all'
        log(f'Building mupdf C, C++ and Python libraries with: {command}')
        subprocess.check_call(command, shell=True)

    # Check whether extra files exist.
    #
    missing = []
    for f in data_files:
        if not os.path.isfile(f):
            missing.append(f)
    if missing:
        log('Warning: expected mupdf binaries are missing:')
        for m in missing:
            log(f'    {m}')
        if do_build:
            log('*** do_build is true so this should not have happened.')
        else:
            log('Suggest running again without "--mupdf-build 0".')
        log()


# Run setuptools.
#
log(f'== calling setuptools.setup(). sys.argv={sys.argv}')

setuptools.setup(
        name='mupdf',
        version=mupdf_version(),
        description="Python bindings for MuPDF",
        url='https://mupdf.com/',
        author='Artifex Software, Inc',
        author_email='support@artifex.com',
        license='GNU Affero',
        classifiers=[
                'Development Status :: 4 - Beta',
                'Intended Audience :: Developers',
                'License :: GNU Affero',
                'Programming Language :: Python :: 3',
                ],
        keywords='pdf',
        project_urls={
                'Documentation': 'https://twiki.ghostscript.com/do/view/Main/MuPDFWrap/',
                'Source': 'https://git.ghostscript.com/?p=mupdf.git',
                'Tracker': 'https://bugs.ghostscript.com/',
                },
        package_dir={
                '': f'{mupdf_dir}/{build_dir}',
                },
        py_modules=['mupdf'],
        data_files=[('', data_files)],
        )
