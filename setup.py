#!/usr/bin/env python3

'''
Installation script for MuPDF Python bindings, using scripts/pipcl.py.

Extra command-line options:

    --mupdf-build 0 | 1 | <path>
        0:  We do not do a build.
        1:  We do a build.
        <path>:
            Instead of doing a build we copy from an existing build directory
            called <path>, e.g. from a different tree with: --mupdf-build
            .../mupdf/build/shared-release

        If --mupdf-build is not specified we use $MUPDF_SETUP_BUILD if it is
        set.

        Otherwise we default to "--mupdf-build 1".

    --mupdf-build-dir <dir>
        Sets the type of build by specifing the build directory, for example:
        --mupdf-build-dir build/shared-debug
'''

import os
import re
import subprocess
import sys
import sys
import time


def log(text=''):
    for line in text.split('\n'):
        print(f'setup.py: {line}')
    sys.stdout.flush()

mupdf_dir = os.path.abspath(f'{__file__}/..')

log(f'== sys.argv={sys.argv}')
log(f'== mupdf_dir={mupdf_dir}')

sys.path.append(f'{mupdf_dir}/scripts')
import pipcl


def mupdf_version():
    with open(f'{mupdf_dir}/include/mupdf/fitz/version.h') as f:
        text = f.read()
    m = re.search('\n#define FZ_VERSION "([^"]+)"\n', text)
    assert m
    version = m.group(1)
    # Append unique(ish) suffix so we can easily experiment on test.pypi.org
    # and make multiple Python releases without requiring an increment to the
    # MuPDF version.
    #
    version += time.strftime(".%Y%m%d.%H%M")
    return version

# Generated files that are copied into the Python installation.
#
out_names = [
        'libmupdf.so',      # C
        'libmupdfcpp.so',   # C++
        '_mupdf.so',        # Python internals
        'mupdf.py',         # Python
        ]

build_dir = 'build/shared-release'
mupdf_build = None

# Process any of our own command-line options.
#
i = 1
while 1:
    if i == len(sys.argv):
        break
    if sys.argv[i] == '--mupdf-build':
        mupdf_build = sys.argv[i+1]
        del sys.argv[i:i+2]
    elif sys.argv[i] == '--mupdf-build-dir':
        build_dir = sys.argv[i+1]
        infix = 'build/shared-'
        if infix not in build_dir:
            raise Exception('--mupdf-build-dir value "{build_dir}" must contain {infix}')
        del sys.argv[i:i+2]
    else:
        i += 1

def sdist():
    '''
    pipcl callback. If we are a git checkout, return all files known to
    git. Otherwise return all files except for those in build/.
    '''
    if os.path.exists('.git'):
        return pipcl.git_items( '.', submodules=True)
    log(f'No .git so including all files apart from build/.')
    ret = []
    for dirpath, dirnames, filenames in os.walk('.'):
        if dirpath.startswith('./build'):
            continue
        if dirpath.startswith('pylocal'):
            continue
        for filename in filenames:
            path = os.path.join(dirpath, filename)
            ret.append(path)
    return ret

def build():
    '''
    pipcl callback. Build MuPDF C, C++ and Python libraries, with exact
    behaviour depending on <mupdf_build> and <build_dir>.
    '''
    global mupdf_build
    if mupdf_build is None:
        mupdf_build = os.environ.get('MUPDF_SETUP_BUILD')

    if mupdf_build is None:
        mupdf_build = '1'

    if mupdf_build == '1':
        command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -d {build_dir} -b all'
    elif mupdf_build == '0':
        command = None
    else:
        log(f'build(): Building by copying files from {mupdf_build} into {build_dir}/')
        command = f'mkdir -p {build_dir}'
        for n in out_names:
            command += f' && rsync -ai {mupdf_build}/{n} {build_dir}/'

    if command:
        log(f'build(): Building MuPDF C, C++ and Python libraries with: {command}')
        subprocess.check_call(command, shell=True)
    else:
        log(f'build(): Not building mupdf because mupdf_build={mupdf_build}')

    paths = []
    for name in out_names:
        path = f'{mupdf_dir}/{build_dir}/{name}'
        paths.append((path, name))
    log(f'build(): returning: {paths}')
    return paths

def clean(all_):
    if all_:
        subprocess.check_call('(rm -r build || true)', shell=True)
    else:
        subprocess.check_call(f'(rm -r {build_dir} || true)', shell=True)

pipcl.run(
        argv = sys.argv,
        name = 'mupdf',
        version = mupdf_version(),
        summary = 'Python bindings for MuPDF library',
        description = 'Python bindings for MuPDF library',
        classifiers = [
                'Development Status :: 4 - Beta',
                'Intended Audience :: Developers',
                'License :: OSI Approved :: GNU Affero General Public License v3',
                'Programming Language :: Python :: 3',
                ],
        author = 'Artifex Software, Inc',
        author_email = 'support@artifex.com',
        url_docs = 'https://twiki.ghostscript.com/do/view/Main/MuPDFWrap/',
        url_home = 'https://mupdf.com/',
        url_source = 'https://git.ghostscript.com/?p=mupdf.git',
        url_tracker = 'https://bugs.ghostscript.com/',
        keywords = 'PDF',
        platform = None,
        fn_clean = clean,
        fn_sdist = sdist,
        fn_build = build,
        license_files = 'COPYING',
        )
