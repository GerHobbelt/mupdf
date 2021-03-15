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
import site
import shutil
import subprocess
import sys
import tarfile
import time

import distutils.core

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

# Important that this does not start with blank line - looks like it is treated
# like an email - headers, blank line, content.
#
metainfo = f'''Metadata-Version: 1.2
Name: mupdf
Version: {mupdf_version()}
Summary: Python bindings for MuPDF
Home-page: https://mupdf.com/
Author: Artifex Software, Inc
Author-email: support@artifex.com
License: GNU Affero General Public License v3
Project-URL: Documentation, https://twiki.ghostscript.com/do/view/Main/MuPDFWrap/
Project-URL: Source, https://git.ghostscript.com/?p=mupdf.git
Project-URL: Tracker, https://bugs.ghostscript.com/
Description: UNKNOWN
Keywords: pdf
Platform: UNKNOWN
Classifier: Development Status :: 4 - Beta
Classifier: Intended Audience :: Developers
Classifier: License :: OSI Approved :: GNU Affero General Public License v3
Classifier: Programming Language :: Python :: 3
'''


def build_dir_allowed_text(indentation):
    '''
    Returns string describing allowed build directories.
    '''
    ret = ''
    for b in build_dir_allowed:
        ret += f'{indentation*" "}{b}\n'
    return ret

class ArgsRaise:
    pass
class Args:
    '''
    Iterates over argv items.
    '''
    def __init__( self, argv):
        self.items = iter( argv)
    class Raise:
        pass
    def next( self, eof=ArgsRaise):
        try:
            return next( self.items)
        except StopIteration:
            if eof == ArgsRaise:
                raise Exception('Not enough args')
            return eof

def main():

    do = None

    opt_egginfo = None
    opt_install_headers = None
    opt_record = None

    build_dir = build_dir_default
    args = Args(sys.argv[1:])
    while 1:
        arg = args.next(None)
        if arg is None:
            break
        if arg in ('install', 'sdist', 'egg_info', 'bdist_wheel'):
            assert do is None
            do = arg
        elif arg == '--record':
            opt_record = args.next()
        elif arg in (
                '--single-version-externally-managed',
                '--compile',
                ):
            pass
        elif arg == '--install-headers':
            opt_install_headers = args.next()
        elif arg == '--egg-base':
            opt_egginfo = args.next()
        elif arg == '--mupdf-build-dir':
            build_dir = args.next()
            if build_dir not in build_dir_allowed:
                raise Exception(f'Unrecognised build-dir {build_dir!r}. Must be one of:\n{build_dir_allowed_text(4)}')
        elif arg in ('-h', '--help'):
            log(
                    'Additional options for mupdf:\n'
                    +  '    --mupdf-build 0 | 1\n'
                    +  '        Whether to build mupdf C, C++ and Python libraries. Default is 1.\n'
                    +  '    --mupdf-build-dir\n'
                    + f'        Set mupdf build type/directory; must be one of:\n{build_dir_allowed_text(12)}'
                    + f'        Default is: {build_dir_default}\n'
                    )
            return
        elif arg == '--help-commands':
            print('help')
            return
        else:
           raise Exception(f'Unrecognised arg: {arg}')

    if not do:
        return


    # Extra files that should be copied into install directory.
    #
    so_files = [
            f'{mupdf_dir}/{build_dir}/libmupdf.so',      # C
            f'{mupdf_dir}/{build_dir}/libmupdfcpp.so',   # C++
            f'{mupdf_dir}/{build_dir}/_mupdf.so',        # Python internals
            f'{mupdf_dir}/{build_dir}/mupdf.py',         # Python
            ]

    if 0:
        pass

    elif do == 'egg_info':
        log(f'doing egg_info')
        assert opt_egginfo
        root = f'{opt_egginfo}/.egg-info'
        os.mkdir(root)
        with open(f'{root}/PKG-INFO', 'w') as f:
            f.write(metainfo)
        #with open(f'{root}/METADTA', 'w') as f:
        #    f.write(metainfo)
        with open(f'{root}/SOURCES.txt', 'w') as f:
            pass
        with open(f'{root}/dependency_links.txt', 'w') as f:
            pass
        with open(f'{root}/top_level.txt', 'w') as f:
            f.write('mupdf\n')

    elif do == 'sdist':
        if 1:
            paths = [
                    'include/mupdf/fitz/version.h',
                    'setup.py',
                    'README',
                    'COPYING',
                    ]
        else:
            paths = jlib.get_gitfiles( f'{mupdf_dir}', submodules=True)
        shutil.rmtree(f'{mupdf_dir}/dist', ignore_errors=True)
        os.mkdir(f'{mupdf_dir}/dist')
        version = mupdf_version()
        tarpath = f'{mupdf_dir}/dist/mupdf-{version}.tar.gz'
        with tarfile.open(tarpath, 'w:gz') as tar:
            for path in paths:
                tar.add( f'{mupdf_dir}/{path}', path, recursive=False)
        log( f'Have created: {tarpath}')

    elif do == 'install':
        if 1:
            log(f'*** Doing fake build of sos.')
            command = f'cd {mupdf_dir}'
            command += ' && pwd && ls -la'
            command += f' && mkdir -p {build_dir}'
            for so in so_files:
                leaf = os.path.basename(so)
                command += f' && rsync -ai /home/jules/artifex/mupdf/build/shared-release/{leaf} {mupdf_dir}/{build_dir}/'
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

        sitepackages = site.getsitepackages()
        assert len(sitepackages) == 1
        sitepackages = sitepackages[0]
        for so_file in so_files:
            log( f'Copying {so_file} to {sitepackages}')
            shutil.copy2( so_file, sitepackages)

    else:
        assert 0, f'Unrecognised do={do}'

if __name__ == '__main__':
    main()
