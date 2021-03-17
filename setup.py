#!/usr/bin/env python3

'''
Installation script for MuPDF Python bindings, using scripts/pipcl.py.

For creating a sdist, we need to be a git checkout - we use 'git ls-files'.
'''

import os
import re
import subprocess
import sys
import sys
import time

mupdf_dir = os.path.abspath(f'{__file__}/..')

sys.path.append(f'{mupdf_dir}/scripts')
import pipcl

log = pipcl.log

log(f'== sys.argv={sys.argv}')


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

class ArgsRaise:
    pass

class Args:
    '''
    Iterates over argv items.
    '''
    def __init__( self, argv):
        self.items = iter( argv)
    def next( self, eof=ArgsRaise):
        '''
        Returns next arg. If no more args, we return eof or raise an exception
        if eof == ArgsRaise.
        '''
        try:
            return next( self.items)
        except StopIteration:
            if eof == ArgsRaise:
                raise Exception('Not enough args')
            return eof

mupdf_do_build = None

i = 1
while 1:
    if i == len(sys.argv):
        break
    if sys.argv[i] == '--mupdf-build':
        mupdf_do_build = sys.argv[i+1]
        del sys.argv[i:i+2]
    else:
        i += 1

def sdist():
    # If we are a git checkout, return all files known to git. Otherwise return
    # all files outside of build/.
    #
    if os.path.exists('.git'):
        return pipcl.git_items( '.', submodules=True)
    log(f'No .git so including all files apart from build/.')
    ret = []
    for dirpath, dirnames, filenames in os.walk('.'):
        if dirpath.startswith('./build'):
            continue
        for filename in filenames:
            path = os.path.join(dirpath, filename)
            ret.append(path)
    return ret

def build():
    global mupdf_do_build
    if mupdf_do_build is None:
        mupdf_do_build = os.environ.get('MUPDF_SETUP_BUILDFROM')

    if mupdf_do_build is None:
        mupdf_do_build = '1'

    if mupdf_do_build == 'fake':
        mupdf_do_build = '/home/jules/artifex/mupdf/build/shared-release'

    if mupdf_do_build == '1':
        command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -d {build_dir} -b all'
    elif mupdf_do_build:
        log(f'Building by copying files from {mupdf_do_build} into {build_dir}/')
        command = (
                f'echo "building by copying from {mupdf_do_build}"'
                f' && pwd'
                f' && mkdir -p {build_dir}'
                )
        for n in out_names:
            command += f' && rsync -ai {mupdf_do_build}/{n} {build_dir}/'
    else:
        command = None

    if command:
        log(f'Building mupdf C, C++ and Python libraries with: {command}')
        subprocess.check_call(command, shell=True)
    else:
        log(f'Not building mupdf because mupdf_do_build={mupdf_do_build}')

    pys = []
    sos = []
    datas = []
    for name in out_names:
        path = f'{mupdf_dir}/{build_dir}/{name}'
        if path.endswith('.py'):
            pys.append( (path, name))
        elif path.endswith('.so'):
            sos.append( (path, name))
        else:
            datas.append( (path, name))
    ret = pys, sos, datas
    log(f'returning pys={pys} sos={sos} datas={datas}')
    return ret

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
        fn_clean = None,
        fn_sdist = sdist,
        fn_build = build,
        license_files = 'COPYING',
        )
