#!/usr/bin/env python3

import os
import re
import setuptools
import subprocess
import sys


mupdf_dir = os.path.abspath(f'{__file__}/..')

def mupdf_version():
    with open(f'{mupdf_dir}/include/mupdf/fitz/version.h') as f:
        text = f.read()
    m = re.search('\n#define FZ_VERSION "([^"]+)"\n', text)
    assert m
    version = m.group(1)

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
    ret = ''
    for b in build_dir_allowed:
        ret += f'{indentation*" "}{b}'
        if b == build_dir_default:
            ret += ' (default)'
        ret += '\n'
    return ret

# Handle our extra --mupdf-* options. We remove any such options from sys.argv
# after we have handled them.
#
do_build = False
build_dir = None

i = 1
while 1:
    if i == len(sys.argv):
        break
    if sys.argv[i] == '--mupdf-build':
        do_build = True
        del sys.argv[i]
    elif sys.argv[i] == '--mupdf-build-dir':
        build_dir = sys.argv[i+1]
        if build_dir not in build_dir_allowed:
            print(f'Unrecognised build-dir {build_dir!r}. Must be one of:\n{build_dir_allowed_text(4)}')
        del sys.argv[i:i+2]
    elif sys.argv[i] in ('-h', '--help'):
        print(
                'Additional options for mupdf:\n'
                +  '    --mupdf-build\n'
                +  '        Build mupdf C, C++ and Python bindings.\n'
                +  '    --mupdf-build-dir\n'
                + f'        Set mupdf build type/directory; must be one of:\n{build_dir_allowed_text(12)}'
                )
        i += 1
    else:
        i += 1

if do_build:
    command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py'
    if build_dir:
        command += f' -d {build_dir}'
    command += f' -b all'
    print(f'Building mupdf C, C++ and Python bindings with: {command}')
    subprocess.check_call(command, shell=True)


# Set the extra files that should be copied into install directory.
#
data_files_leaf = [
        '_mupdf.so',
        'libmupdf.so',
        'libmupdfcpp.so',
        ]
data_files = [f'{mupdf_dir}/build/shared-release/{leaf}' for leaf in data_files_leaf]

# Check whether extra files exist.
#
missing = []
for f in data_files:
    if not os.path.isfile(f):
        missing.append(f)
if missing:
    print('Warning: mupdf binaries are missing; run with --mupdf-build to build binaries.')


# Run setuptools.
#
setuptools.setup(
        name='mupdf',
        version=mupdf_version(),
        description="Python bindings for MuPDF",
        url='https://mupdf.com/',
        py_modules=['mupdf'],
        data_files=[
                ('', data_files),
                ],
        )
