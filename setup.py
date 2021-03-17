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

mupdf_do_build = '1'

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
    return pipcl.git_items( '.', submodules=True)

def build():
    if mupdf_do_build == 'fake':
        command = 'true'
        for name in names:
            command += ( ' && rsync -ai'
                    + f' /home/jules/artifex/mupdf/build/shared-release/{name}'
                    + f' {mupdf_dir}/{build_dir}/{name}'
                    )
    elif mupdf_do_build == '1':
        command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -d {build_dir} -b all'
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
