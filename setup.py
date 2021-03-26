#!/usr/bin/env python3

'''
Installation script for MuPDF Python bindings, using scripts/pipcl.py.

Default behaviour:

    When building an sdist (e.g. with 'pip sdist'), we use clang-python to
    generate C++ source which is then included in the sdist.

    When installing or building a wheel (e.g. 'pip install <sdist>'), we
    assume that generated C++ source is already present. Thus we don't rely on
    clang-python being present.

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

    --mupdf-build-dir <build-dir>
        Sets the type of build by specifing the build directory, for example:
        --mupdf-build-dir build/shared-debug

Environmental variables:

    MUPDF_SETUP_BUILD_DIR
        Sets the build directory; overriden if --mupdf-build-dir is specified.

    MUPDF_SETUP_HAVE_CLANG_PYTHON
        Affects whether we use clang-python when building.

        If set and not '1', we do not attempt to use clang-python, and instead
        assume that generated files are already available in platform/c++/.

        Default is '0'.

    MUPDF_SETUP_HAVE_SWIG
        If set and not '1', we do not attempt to run swig, and instead assume
        that generated files are already available in platform/python/.
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
log(f'== pwd:')
subprocess.check_call('pwd', shell=True)
log(f'== ls -l:')
subprocess.check_call('ls -l', shell=True)

sys.path.append(f'{mupdf_dir}/scripts')
import pipcl


def mupdf_version():

    # If PKG-INFO exists, we are being used to create a wheel from an sdist, so
    # use the version from the PKG-INFO file.
    #
    if os.path.exists('PKG-INFO'):
        items = pipcl._parse_pkg_info('PKG-INFO')
        assert items['Name'] == 'mupdf'
        ret = items['Version']
        log(f'Using version from PKG-INFO: {ret}')
        return ret

    # If we get here, we are in a source tree, e.g. to create an sdist.
    #
    # We use the MuPDF version with a unique(ish) suffix based on the current
    # date and time, so we can make multiple Python releases without requiring
    # an increment to the MuPDF version.
    #
    # This also allows us to easily experiment on test.pypi.org.
    #
    with open(f'{mupdf_dir}/include/mupdf/fitz/version.h') as f:
        text = f.read()
    m = re.search('\n#define FZ_VERSION "([^"]+)"\n', text)
    assert m
    version = m.group(1)
    version += time.strftime(".%Y%m%d.%H%M")
    log(f'Have created version number: {version}')
    return version

# Generated files that are copied into the Python installation.
#
out_names = [
        'libmupdf.so',      # C
        'libmupdfcpp.so',   # C++
        '_mupdf.so',        # Python internals
        'mupdf.py',         # Python
        ]

build_dir = os.environ.get('MUPDF_SETUP_BUILD_DIR', 'build/shared-release')
log(f'MUPDF_SETUP_BUILD_DIR={os.environ.get("MUPDF_SETUP_BUILD_DIR")}')
log(f'build_dir={build_dir}')

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


# pipcl Callbacks.
#

g_test_minimal = False

def sdist():
    '''
    pipcl callback. If we are a git checkout, return all files known to
    git. Otherwise return all files except for those in build/.
    '''
    if g_test_minimal:
        # Cut-down for testing.
        return [
                'setup.py',
                'pyproject.toml',
                'scripts/pipcl.py',
                'include/mupdf/fitz/version.h',
                'COPYING',
                ]

    if not os.path.exists('.git'):
        raise Exception(f'Cannot make sdist because not a git checkout')
    paths = pipcl.git_items( '.', submodules=True)

    # Build C++ files and SWIG C code for inclusion in sdist, so that it can be
    # used on systems without clang-python or SWIG.
    #
    subprocess.check_call(f'./scripts/mupdfwrap.py -d {build_dir} -b 02', shell=True)
    paths += [
            'build/shared-release/mupdf.py',
            'platform/c++/container_classnames.pickle',
            'platform/c++/implementation/classes.cpp',
            'platform/c++/implementation/exceptions.cpp',
            'platform/c++/implementation/functions.cpp',
            'platform/c++/implementation/internal.cpp',
            'platform/c++/include/mupdf/classes.h',
            'platform/c++/include/mupdf/exceptions.h',
            'platform/c++/include/mupdf/functions.h',
            'platform/c++/include/mupdf/internal.h',
            'platform/c++/swig_c.pickle',
            'platform/c++/swig_python.pickle',
            'platform/c++/to_string_structnames.pickle',
            'platform/python/mupdfcpp_swig.cpp',
            ]
    return paths


def build():
    '''
    pipcl callback. Build MuPDF C, C++ and Python libraries, with exact
    behaviour depending on <mupdf_build> and <build_dir>.
    '''
    if g_test_minimal:
        # Cut-down for testing.
        log(f'g_test_minimal set so doing minimal build.')
        os.makedirs(f'{mupdf_dir}/{build_dir}', exist_ok=True)
        path = f'{mupdf_dir}/{build_dir}/mupdf.py'
        with open(path, 'w') as f:
            f.write('print("This is fake mupdf!")')
        return [(path, os.path.basename(path))]

    global mupdf_build
    if mupdf_build is None:
        mupdf_build = os.environ.get('MUPDF_SETUP_BUILD')

    if mupdf_build is None:
        mupdf_build = '1'

    if mupdf_build == '1':
        if os.path.exists(f'{mupdf_dir}/PKG-INFO'):
            # We are in an sdist, so generated C++ source and generated SWIG
            # source files should be in place.
            #
            # We default to assuming that clang-python is not available and
            # SWIG is available.
            #
            have_clang_python = os.environ.get('MUPDF_SETUP_HAVE_CLANG_PYTHON', '0') == '1'
            have_swig = os.environ.get('MUPDF_SETUP_HAVE_SWIG', '1') == '1'
            b = 'm'         # Build C library.
            if have_clang_python:
                b += '0'    # Build C++ source.
            b += '1'        # Build C++ library.
            if have_swig:
                b += '2'    # Build SWIG-generated source.
            b += '3'        # Build SWIG library _mupdf.so.
        else:
            # Not an sdist so can't rely on pre-build C++ source etc, so build
            # everything.
            #
            log(f'Building with "-b all" because we do not appear to be in an sdist')
            b = 'all'
        command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -d {build_dir} -b {b}'

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


mupdf_package = pipcl.Package(
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
        license_files = ['COPYING'],
        fn_clean = clean,
        fn_sdist = sdist,
        fn_build = build,
        )

log(f'mupdf_package={mupdf_package}')

# Things to allow us to function as a PIP-517 backend:
#
def build_wheel( wheel_directory, config_settings=None, metadata_directory=None):
    return mupdf_package.build_wheel(
            wheel_directory,
            config_settings,
            metadata_directory,
            )

def build_sdist( sdist_directory, config_settings=None):
    return mupdf_package.build_sdist(
            sdist_directory,
            config_settings,
            )

# Allow us to be used as a pre-PIP-517 setup.py script.
#
if __name__ == '__main__':
    mupdf_package.handle_argv(sys.argv)
