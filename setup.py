#!/usr/bin/env python3

'''
Installation script for MuPDF Python bindings, using scripts/pipcl.py.

Default behaviour:

    When building an sdist (e.g. with 'pip sdist'), we use clang-python to
    generate C++ source which is then included in the sdist.

    When installing or building a wheel (e.g. 'pip install <sdist>'), we
    assume that generated C++ source is already present. Thus we don't rely on
    clang-python being present.

Environmental variables:

    MUPDF_SETUP_BUILD_DIR
        Overrides the default build directory.

    MUPDF_SETUP_USE_CLANG_PYTHON
        Affects whether we use clang-python when building.

        If set, must be '0' or '1', and we override the default and do not
        ('0') / do ('1') use clang-python to generate C++ source code from
        MuPDF headers.

        If we are an sdist we default to not re-generating C++ - the generated
        files will be already available in platform/c++/. Otherwise we default
        to generating C++ source code.

    MUPDF_SETUP_USE_SWIG
        If set, must be '0' or '1', and we do not ('0') / do ('1') attempt to
        run swig.
'''

import os
import platform
import re
import subprocess
import sys
import sys
import time


def log(text=''):
    for line in text.split('\n'):
        print(f'mupdf:setup.py: {line}')
    sys.stdout.flush()

if __name__ == '__main__':
    log(f'== We are being run directly. sys.argv={sys.argv}')
else:
    log(f'== We are being imported.')

mupdf_dir = os.path.abspath(f'{__file__}/..')
sys.path.append(f'{mupdf_dir}/scripts')
import pipcl

os_name = platform.system()
g_windows = (os_name == 'Windows' or os_name.startswith('CYGWIN'))

def mupdf_version():

    # If PKG-INFO exists, we are being used to create a wheel from an sdist, so
    # use the version from the PKG-INFO file.
    #
    if os.path.exists('PKG-INFO'):
        items = pipcl._parse_pkg_info('PKG-INFO')
        assert items['Name'] == 'mupdf'
        ret = items['Version']
        #log(f'Using version from PKG-INFO: {ret}')
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
if g_windows:
    # Windows.
    out_names  = [
            'mupdfcpp.dll', # C and C++.
            '_mupdf.pyd',   # Python internals.
            'mupdf.py',     # Python.
            ]
    if sys.maxsize == 2**31 - 1:
        build_dir = 'build/shared-release-x32'
    else:
        build_dir = 'build/shared-release-x64'
else:
    # Unix.
    out_names = [
            'libmupdf.so',      # C.
            'libmupdfcpp.so',   # C++.
            '_mupdf.so',        # Python internals.
            'mupdf.py',         # Python.
            ]
    build_dir = 'build/shared-release'

build_dir = os.environ.get('MUPDF_SETUP_BUILD_DIR', build_dir)
#log(f'MUPDF_SETUP_BUILD_DIR={os.environ.get("MUPDF_SETUP_BUILD_DIR")}')
#log(f'build_dir={build_dir}')


# pipcl Callbacks.
#

# Set MUPDF_SETUP_MINIMAL=1 to create minimal package for testing
# upload/download etc, avoiding compilation when installing.
#
g_test_minimal = int(os.environ.get('MUPDF_SETUP_MINIMAL', '0'))


def git_info():
    '''
    Returns (current, origin, diff):
        current: git id from 'git show'.
        origin: git id from 'git show origin'.
        diff: diff relative to current.
    '''
    def get_id(command):
        text = subprocess.check_output(command, shell=True)
        text = text.decode('utf8')
        text = text.split('\n', 1)[0]
        text = text.split(' ', 1)[0]
        return text
    current = get_id('git show --pretty=oneline')
    origin = get_id('git show --pretty=oneline origin')
    diff = subprocess.check_output('git diff', shell=True).decode('utf8')
    return current, origin, diff


def sdist():
    '''
    pipcl callback. If we are a git checkout, return all files known to
    git. Otherwise return all files except for those in build/.
    '''
    # Create 'git-info' file containing git ids that identify this tree. For
    # the moment this is a simple text format, but we could possibly use pickle
    # instead, depending on whether we want to include more information, e.g.
    # diff relative to origin.
    #
    git_id, git_id_origin, git_diff = git_info()
    with open('git-info', 'w') as f:
        f.write(f'git-id: {git_id}\n')
        f.write(f'git-id-origin: {git_id_origin}\n')
        f.write(f'git-diff:\n{git_diff}\n')

    if g_test_minimal:
        # Cut-down for testing.
        return [
                'setup.py',
                'pyproject.toml',
                'scripts/pipcl.py',
                'include/mupdf/fitz/version.h',
                'COPYING',
                'git-info',
                ]

    if not os.path.exists('.git'):
        raise Exception(f'Cannot make sdist because not a git checkout')
    paths = pipcl.git_items( mupdf_dir, submodules=True)

    # Build C++ files and SWIG C code for inclusion in sdist, so that it can be
    # used on systems without clang-python or SWIG.
    #
    subprocess.check_call(f'{sys.executable} ./scripts/mupdfwrap.py -d {build_dir} -b 02', shell=True)
    paths += [
            'build/shared-release/mupdf.py',
            'git-info',
            'platform/c++/c_functions.pickle',
            'platform/c++/c_globals.pickle',
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
            'platform/c++/windows_mupdf.def',
            'platform/python/mupdfcpp_swig.cpp',
            ]
    return paths


def get_flag(name, default):
    '''
    name:
        Name of environmental variable.
    default:
        Value to return if <name> undefined.
    Returns False if name is '0', True if name is '1', <default> if
    undefined. Otherwise assert fails.
    '''
    value = os.environ.get(name)
    if value is None:
        return default
    elif value == '0':
        return False
    elif value == '1':
        return True
    else:
        assert 0, f'If set, ${name} must be "0" or "1", but is: {value!r}'


def build():
    '''
    pipcl callback. Build MuPDF C, C++ and Python libraries.
    '''
    if g_test_minimal:
        # Cut-down for testing.
        log(f'g_test_minimal set so doing minimal build.')
        os.makedirs(f'{mupdf_dir}/{build_dir}', exist_ok=True)
        path = f'{mupdf_dir}/{build_dir}/mupdf.py'
        with open(path, 'w') as f:
            f.write('print("This is fake mupdf!")')
        return [(path, os.path.basename(path))]

    # If we are an sdist, default to not trying to run clang-python - the
    # generated files will already exist, and installing/using clang-python
    # might be tricky.
    #
    in_sdist = os.path.exists(f'{mupdf_dir}/PKG-INFO')
    use_clang_python = get_flag('MUPDF_SETUP_USE_CLANG_PYTHON', not in_sdist)
    use_swig = get_flag('MUPDF_SETUP_USE_SWIG', True)

    b = ''
    if not g_windows:
        b = 'm'     # Build C library.
    if use_clang_python:
        b += '0'    # Build C++ source.
    b += '1'        # Build C++ library (also contains C library on Windows).
    if use_swig:
        b += '2'    # Build SWIG-generated source.
    b += '3'        # Build SWIG library _mupdf.so.

    command = (
            f'cd {mupdf_dir}'
            f' && {sys.executable} ./scripts/mupdfwrap.py'
                f'{" --swig-windows-auto" if g_windows else ""}'
                f' -d {build_dir}'
                f' -b {b}'
            )

    do_build = os.environ.get('MUPDF_SETUP_DO_BUILD')
    if do_build == '0':
        # This is a hack for testing.
        log(f'Not doing build because $MUPDF_SETUP_DO_BUILD={do_build}')
    else:
        log(f'build(): Building MuPDF C, C++ and Python libraries with: {command}')
        subprocess.check_call(command, shell=True)

    # Return generated files to copy into wheel.
    #
    paths = []
    for name in out_names:
        paths.append((f'{build_dir}/{name}', name))

    log(f'build(): returning: {paths}')
    return paths


def clean(all_):
    if all_:
        return 'build'
    else:
        return build_dir


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
        fn_build = build,
        fn_clean = clean,
        fn_sdist = sdist,
        root_dir = os.path.dirname(__file__),
        )

#log(f'mupdf_package={mupdf_package}')

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
