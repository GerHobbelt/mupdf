#!/usr/bin/env python3

import os
import re
import setuptools
import subprocess

mupdf_dir = os.path.abspath(f'{__file__}/..')

def version():
    with open(f'{mupdf_dir}/include/mupdf/fitz/version.h') as f:
        text = f.read()
    m = re.search('\n#define FZ_VERSION "([^"]+)"\n', text)
    assert m
    version = m.group(1)

# Build MuPDF's C++ and Python bindings:
def build():
    subprocess.check_call(f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -b all', shell=1)

def install():
    print(f'version={version}')


if 0:
    args = iter(sys.argv[1:])
    while 1:
        try:
            arg = next(args)
        except StopIteration:
            break
        if arg == 'build':
            build()
        elif arg == 'install':
            install()
        else:
            raise Exception(f'Unrecognised arg: {arg}')

if 1:
    setuptools.setup(
            name='mupdf',
            version=version(),
            description="Python bindings for MuPDF",
            url='https://mupdf.com/',
            py_modules=['mupdf'],
            package_dir={
                    '': f'{mupdf_dir}/build/shared-release',
                    },
            #ext_modules=[
            #        setuptools.Extension('_mupdf', []),
            #        ],
            #package_data={
            #        '_mupdf.so': ['_mupdf.so'],
            #        },
            data_files=[
                    ('', [
                        f'{mupdf_dir}/build/shared-release/_mupdf.so',
                        f'{mupdf_dir}/build/shared-release/libmupdf.so',
                        f'{mupdf_dir}/build/shared-release/libmupdfcpp.so',
                        ]),
                    ],
            )
