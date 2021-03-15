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
import time


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
    # Append unique(ish) suffix so we can easily experiment on test.pypi.org.
    #
    version += time.strftime(".%Y%m%d.%H%M")
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

do_mupdf_build = None

do_build = False
do_install = False
do_egg_info = False
do_sdist = False
do_bdist_wheel = False
do_d = None

build_dir = build_dir_default
show_help = False
show_help_commands = False
i = 1
while 1:
    if i == len(sys.argv):
        break
    if sys.argv[i] == '--mupdf-build':
        do_mupdf_build = int(sys.argv[i+1])
        del sys.argv[i:i+2]
    elif sys.argv[i] == '--mupdf-build-dir':
        build_dir = sys.argv[i+1]
        if build_dir not in build_dir_allowed:
            raise Exception(f'Unrecognised build-dir {build_dir!r}. Must be one of:\n{build_dir_allowed_text(4)}')
        del sys.argv[i:i+2]
    elif sys.argv[i] in ('-h', '--help'):
        show_help = True
        i += 1
    elif sys.argv[i] == '--help-commands':
        show_help_commands = True
        i += 1
    elif sys.argv[i] == 'build':
        do_build = True
        i += 1
    elif sys.argv[i] == 'install':
        do_install = True
        i += 1
    elif sys.argv[i] == 'egg_info':
        do_egg_info = True
        i += 1
    elif sys.argv[i] == 'sdist':
        do_sdist = True
        i += 1
    elif sys.argv[i] == 'bdist_wheel':
        do_bdist_wheel = True
        i += 1
    elif sys.argv[i] == '-d':
        do_d = sys.argv[i+1]
        i += 2
    else:
        i += 1

if do_mupdf_build is None:
    if do_build or do_install or do_bdist_wheel:
        do_mupdf_build = 1

# Extra files that should be copied into install directory.
#
so_files = [
        f'{mupdf_dir}/{build_dir}/libmupdf.so',      # C
        f'{mupdf_dir}/{build_dir}/libmupdfcpp.so',   # C++
        f'{mupdf_dir}/{build_dir}/_mupdf.so',        # Python internals
        #f'{mupdf_dir}/{build_dir}/mupdf.py',         # Python
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
    if do_mupdf_build:
        if 1:
            log(f'*** Doing fake build of sos. do_d={do_d}')
            command = f'cd {mupdf_dir} && mkdir -p {build_dir} '
            for leaf in (
                    'libmupdf.so',
                    'libmupdfcpp.so',
                    '_mupdf.so',
                    ):
                command += f' && rsync -ai /home/jules/artifex/mupdf/build/shared-release/{leaf} {build_dir}/{leaf}'
                if do_d:
                    command += f' && rsync -ai /home/jules/artifex/mupdf/build/shared-release/{leaf} {do_d}/build/lib/{leaf}'
        else:
            command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -d {build_dir} -b all'
        log(f'Building mupdf C, C++ and Python libraries with: {command}')
        subprocess.check_call(command, shell=True)

    # Check whether extra files exist.
    #
    missing = []
    for f in so_files:
        if not os.path.isfile(f):
            missing.append(f)
    if missing:
        log('Warning: mupdf binaries have not been built and are missing:')
        for m in missing:
            log(f'    {m}')
        log()

log(f'== do_egg_info={do_egg_info}')

# Run setuptools.
#
if do_sdist:
    package_dir = {
            '': mupdf_dir,
            }
    if 1:
        data_files = ['setup.py']
    else:
        sys.path.append( f'{mupdf_dir}/scripts')
        import jlib
        data_files = jlib.get_gitfiles( f'{mupdf_dir}', submodules=True)
elif do_build or do_install:
    package_dir={
            '': f'{mupdf_dir}/{build_dir}',
            }
    data_files = so_files
else:
    package_dir = {}
    data_files = []


log(f'== calling setuptools.setup(). sys.argv={sys.argv} package_dir={package_dir} data_files={data_files}')

setuptools.setup(
        name='mupdf',
        version=mupdf_version(),
        description="Python bindings for MuPDF",
        url='https://mupdf.com/',
        author='Artifex Software, Inc',
        author_email='support@artifex.com',
        license='GNU Affero General Public License v3',
        classifiers=[
                'Development Status :: 4 - Beta',
                'Intended Audience :: Developers',
                'License :: OSI Approved :: GNU Affero General Public License v3',
                'Programming Language :: Python :: 3',
                ],
        keywords='pdf',
        project_urls={
                'Documentation': 'https://twiki.ghostscript.com/do/view/Main/MuPDFWrap/',
                'Source': 'https://git.ghostscript.com/?p=mupdf.git',
                'Tracker': 'https://bugs.ghostscript.com/',
                },
        package_dir=package_dir,
        py_modules=['mupdf'],
        data_files=[('', data_files)],
        )
