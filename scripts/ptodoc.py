#! /usr/bin/env python3

'''
Builds and tests source/tools/extract_text.c.

Args:

    -b <build-flags>
        <build-flags> is passed to ./scripts/mupdfwrap.py -b.

        Default is 'all' which builds mupdf.so and C++ wrappers and Python
        wrappers. The latter two are very slow, and usually not required after
        first run, in which case '-b m' will build just mupdf.so which usually
        builds very quickly.

    -d <directory>
        Set location of mupdf.so. Also passed to ./scripts/mupdfwrap.py -d.

        E.g. use .../build/shared-release for release build. Default is
        .../build/shared-debug.

    --failat <arg>
        Include <arg> before the executable.

        E.g.:
            --failat MEMENTO_FAILAT=279

    --squeeze <arg>
        Include <arg> before the executable and appends tail suitable for
        memento squeezing (piping through ../ghostpdl/toolbin/squeeze2html.pl
        etc).

        E.g.:
            --squeeze MEMENTO_SQUEEZEAT=1

    --valgrind 0 | 1
        If 1, run with valgrind.

Example:

    ./scripts/ptodoc.py -b m
    libreoffice ../ghostpdl/zlib/zlib.3.pdf.docx


Requirements:

    Packages:
         zip
         unzip
'''

import os
import sys

import jlib
log = jlib.log

if 0:
    jlib.g_log_prefixes.append( jlib.LogPrefixFileLine())

mupdf_root = os.path.abspath( f'{__file__}/../../')



def extract(
        extract_text_exe,
        mupdf_shared_dir,
        path_template,
        path_in,
        valgrind,
        squeeze,
        failat,
        method,
        ):
    '''
    Extracts text, and compares .docx's word/document.xml if reference file
    exists.
    '''
    log(f'Doing text extraction with {path_in}, method={method}')
    path_out = f'{path_in}-{method}.docx'
    path_content = f'{path_out}.content.xml'
    path_intermediate = f'{path_in}.intermediate.xml'

    executable = None
    command = None
    if method == 'trace':
        command = f'build/debug/mutool draw -F trace -o {path_intermediate} {path_in}'
        #jlib.system( command, out=log, verbose=1, prefix='    ')
    elif method == 'raw' or method == 'stext':
        command = f'build/debug/mutool draw -F raw -o {path_intermediate} {path_in}'
        # Run mutool.py to get intermediate xml.
        #command = ''
        #command += f'LD_LIBRARY_PATH={mupdf_shared_dir} PYTHONPATH={mupdf_shared_dir}'
        #command += f' scripts/mutool.py draw -F raw -o {path_intermediate} {path_in}'
    else:
        assert 0
        
    if command:
        jlib.system( command, out=log, verbose=1, prefix='    ')

    command = ''
    command += (''
                f' LD_LIBRARY_PATH=/home/jules/artifex/libbacktrace/.libs'
                f' MEMENTO_HIDE_MULTIPLE_REALLOCS=1'
                )
    if squeeze:
        command += f' {squeeze}'
    if failat:
        command += f' {failat}'
    if valgrind:
        command += f' valgrind --leak-check=full'
    command += (
                f' ./{extract_text_exe}'
                f' -i {path_intermediate}'
                f' -t {path_template}'
                f' -p 1'    # preserve .docx temporary directory.
                f' -c {path_content}'
                f' -m {method}'
                f' -o {path_out}'
                )
    if squeeze:
        command += ' 2>&1 | tee >(perl ../ghostpdl/toolbin/squeeze2html.pl | gzip -9 -c > squeeze.html.gz) | grep "Memory squeezing @"'
        executable='bash'
    jlib.system( command, out=log, verbose=1, prefix='    ', executable=executable)

    path_content = f'{path_out}.content.xml'
    path_content_ref = f'{path_out}.content.ref.xml'
    if os.path.exists(path_content_ref):
        jlib.system(f'diff -u {path_content_ref} {path_content}', out=log, verbose=1, prefix='    ')
    else:
        log(f'*** No reference content {path_content_ref} to compare with generated {path_content}. os.getcwd()={os.getcwd()}')

    path_document_xml = f'{path_out}.dir/word/document.xml'
    path_document_xml_ref = f'{path_out}.word.document.ref.xml'
    if os.path.exists(path_document_xml_ref):
        jlib.system(f'diff -u {path_document_xml_ref} {path_document_xml}', out=log, verbose=1, prefix='    ')
    else:
        log(f'*** No reference document {path_document_xml_ref} to compare with generated {path_document_xml}')


def test(mupdf_shared_dir, so_build, valgrind, squeeze, failat):

    if so_build:
        with jlib.LogPrefixScope('building mupdf.so: '):
            # Build mupdf.so and python wrapper.
            #
            command = f'./scripts/mupdfwrap.py -d build/shared-debug -b {so_build}'
            jlib.system( command, out=log, verbose=1, prefix='    ')
        
    
    with jlib.LogPrefixScope('building extract_text.exe: '):
        # Build extract_text.exe.
        #
        extract_text_c = 'source/tools/extract_text.c'
        extract_text_cc = 'source/tools/extract_text.c.c'
        extract_text_exe = 'extract_text.c.exe'

        memento_c = 'source/fitz/memento.c'
        memento_cc = 'source/fitz/memento.cc'
        if 0:
            jlib.build(
                    extract_text_c,
                    extract_text_cc,
                    f'cc -E -dD -g -o {memento_cc} -DMEMENTO {memento_c} -pthread -I include -I /usr/local/include -W -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function build/shared-debug/libmupdf.so -Wl,--export-dynamic -L /usr/local/lib -lm -lexecinfo',
                    out=log,
                    )

            jlib.build(
                    extract_text_c,
                    extract_text_cc,
                    f'cc -E -dD -g -o {extract_text_cc} -DMEMENTO {extract_text_c} -pthread -I include -I /usr/local/include -W -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function build/shared-debug/libmupdf.so -Wl,--export-dynamic -L /usr/local/lib -lm -lexecinfo',
                    out=log,
                    )

        command = (
                f'cc -g'
                f' -o {extract_text_exe}'
                f' -DMEMENTO'
                f' {extract_text_c}'
                f' source/fitz/memento.c'
                f' -pthread'
                f' -I include'
                f' -I /usr/local/include'
                f' -W -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function'
                f' build/shared-debug/libmupdf.so'
                f' -lm'
                )
        if os.uname()[0] == 'OpenBSD':
            command += ' -Wl,--export-dynamic -L /usr/local/lib -lexecinfo'
        else:
            command += ' -DHAVE_LIBDL -ldl'
        jlib.build(
                (extract_text_c, memento_c),
                extract_text_exe,
                command,
                out=log,
                )

    # Extract text from various input files.
    #
    path_template = '../Untitled1.docx'
    assert os.path.isfile(path_template), '*** We require an empty .docx document template called %s' % path_template
    for in_pdf in (
            f'{mupdf_root}/../ghostpdl/zlib/zlib.3.pdf',
            f'{mupdf_root}/../Python2.pdf',
            ):
        in_pdf_rel = os.path.relpath(in_pdf)

        # 2020-07-27: trace broken by recent changes.
        # for method in 'raw', 'stext', 'trace':
        #
        for method in 'raw', 'stext':
            with jlib.LogPrefixScope(f'{in_pdf_rel} method={method}: '):
                extract(
                        extract_text_exe,
                        mupdf_shared_dir,
                        path_template,
                        in_pdf_rel,
                        valgrind=valgrind,
                        squeeze=squeeze,
                        failat=failat,
                        method=method,
                        )

    log( 'finished')


if __name__ == '__main__':
    args = iter(sys.argv[1:])
    so_build = 'all'
    mupdf_shared_dir = f'{mupdf_root}/build/shared-debug'
    valgrind = None
    squeeze = None
    failat = None
    
    while 1:
        try:
            arg = next(args)
        except StopIteration:
            break
        if arg == '-b':
            so_build = next(args)
        elif arg == '-d':
            mupdf_shared_dir = next(args)
        elif arg == '--failat':
            failat = next(args)
        elif arg in ('-h', '--help'):
            print(__doc__)
        elif arg == '--valgrind':
            valgrind = int(next(args))
        elif arg == '--squeeze':
            squeeze = next(args)
        else:
            assert 0, 'unrecognised arg: %r' % arg
    try:
        test(mupdf_shared_dir, so_build, valgrind, squeeze, failat)
    except Exception:
        print( jlib.exception_info())
        sys.exit(1)
