#!/usr/bin/env python3

'''
Installation script for MuPDF Python bindings.

For creating a sdist, we need to be a git checkout - we use 'git ls-files'.
'''

import os
import sys


if 1:
    import re
    import subprocess
    import sys
    import time

    import pipcl

    log = pipcl.log

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

    out_names = [
            'libmupdf.so',      # C
            'libmupdfcpp.so',   # C++
            '_mupdf.so',        # Python internals
            'mupdf.py',         # Python
            ]

    build_dir = 'build/shared-release'

    def sdist():
        return pipcl.git_items( '.', submodules=True)

    def build():
        if 0:
            # Fake build.
            command = 'true'
            for name in names:
                command += ( ' && rsync -ai'
                        + f' /home/jules/artifex/mupdf/build/shared-release/{name}'
                        + f' {mupdf_dir}/{build_dir}/{name}'
                        )
        else:
            command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -d {build_dir} -b all'

        log(f'Building mupdf C, C++ and Python libraries with: {command}')
        subprocess.check_call(command, shell=True)

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
        return pys, sos, datas

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
            )

    sys.exit()


import io
import os
import re
import shutil
import site
import subprocess
import sys
import tarfile
import time
import zipfile

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

def log(text=''):
    '''
    Logs lines with prefix.
    '''
    for line in text.split('\n'):
        print(f'mupdf/setup.py: {line}', file=sys.stdout)
    sys.stdout.flush()

log(f'== sys.argv={sys.argv}')

mupdf_dir = os.path.abspath(f'{__file__}/..')



# Information about supported build directories.
#
build_dir_allowed = (
        'build/shared-debug',
        'build/shared-debug-extract',
        'build/shared-release',
        'build/shared-release-extract',
        )
build_dir_default = 'build/shared-release'


def metainfo():
    '''
    Returns metainfo text for the 'egg_info' command.

    Important that this does not start with blank line - looks like it is
    treated like an email - headers, blank line, content.
    '''
    ret = f'''Metadata-Version: 1.2
Name: mupdf
Version: {mupdf_version()}
Summary: Python bindings for MuPDF library
Home-page: https://mupdf.com/
Author: Artifex Software, Inc
Author-email: support@artifex.com
License: GNU Affero General Public License v3
Project-URL: Documentation, https://twiki.ghostscript.com/do/view/Main/MuPDFWrap/
Project-URL: Source, https://git.ghostscript.com/?p=mupdf.git
Project-URL: Tracker, https://bugs.ghostscript.com/
Description: Python bindings for MuPDF library
Keywords: PDF
Platform: UNKNOWN
Classifier: Development Status :: 4 - Beta
Classifier: Intended Audience :: Developers
Classifier: License :: OSI Approved :: GNU Affero General Public License v3
Classifier: Programming Language :: Python :: 3
'''
    return ret


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


def main():

    command = None

    opt_egg_base = None
    opt_install_headers = None
    opt_record = None
    opt_d = None
    opt_dist_dir = 'dist'

    opt_mupdf_fake_build = os.environ.get('SETUP_PY_MUPDF_FAKE_BUILD', None)
    if opt_mupdf_fake_build:
        opt_mupdf_fake_build = int(opt_mupdf_fake_build)

    build_dir = build_dir_default

    args = Args(sys.argv[1:])

    while 1:
        arg = args.next(None)
        if arg is None:
            break

        elif arg in ('install', 'sdist', 'egg_info', 'bdist_wheel'):
            assert command is None, 'Two commands specified: {command} and {arg}.'
            command = arg

        elif arg == '--compile':
            pass

        elif arg == '-d':
            opt_d = args.next()

        elif arg == '--dist-dir':
            opt_dist_dir = args.next()

        elif arg == '--egg-base':
            opt_egg_base = args.next()

        elif arg in ('-h', '--help'):
            log(
                    'Options:\n'
                    '    bdist_wheel\n'
                    '        Not implemented.\n'
                    '    clean\n'
                    '        Not implemented.\n'
                    '    egg_info\n'
                    '        Creates files in: <egg-base>/.egg-info/\n'
                    '    install\n'
                    '        Installs into Python\'s site.getsitepackages()[0].'
                    '        Logs to <record> (not implemented).\n'
                    '    sdist\n'
                    '        Make a source distribution:\n'
                    '            <dist-dir>/mupdf-<version>.tar.gz\n'
                    '    --egg-base <egg-base>\n'
                    '        Not implemented.\n'
                    '    -d <d>\n'
                    '        Not implemented.\n'
                    '    --dist-dir <dist-dir>\n'
                    '        Default is "dist".'
                    '    --record <record>\n'
                    '        Ignored.\n'
                    '    --single-version-externally-managed\n'
                    '        Ignored.\n'
                    '    --compile\n'
                    '        Ignored.\n'
                    '    --install-headers <directory>\n'
                    '        Ignored.\n'
                    '\n'
                    f'MuPDF options:\n'
                    f'    --mupdf-build-dir\n'
                    f'        Set mupdf build type/directory; must be one of:\n{build_dir_allowed_text(12)}'
                    f'        Default is: {build_dir_default}\n'
                    )
            return

        elif arg == '--help-commands':
            print('help')
            return

        elif arg == '--install-headers':
            opt_install_headers = args.next()

        elif arg == '--mupdf-build-dir':
            build_dir = args.next()
            if build_dir not in build_dir_allowed:
                raise Exception(f'Unrecognised build-dir {build_dir!r}. Must be one of:\n{build_dir_allowed_text(4)}')

        elif arg == '--mupdf-fake-build':
            opt_mupdf_fake_build = int(args.next())

        elif arg == '--record':
            # todo: do something with this, presumably write a log to the
            # specified file?
            #
            opt_record = args.next()

        elif arg == '--single-version-externally-managed':
            pass

        else:
           raise Exception(f'Unrecognised arg: {arg}')

    if not command:
        return

    # Files that should be copied into site.getsitepackages()[0].
    #
    paths = [
            f'{mupdf_dir}/{build_dir}/libmupdf.so',      # C
            f'{mupdf_dir}/{build_dir}/libmupdfcpp.so',   # C++
            f'{mupdf_dir}/{build_dir}/_mupdf.so',        # Python internals
            f'{mupdf_dir}/{build_dir}/mupdf.py',         # Python
            ]

    log(f'Handling command={command}')
    if 0:
        pass

    elif command == 'bdist_wheel':
        # Can't figure out how bdist_wheel is supposed to work. We can create a
        # zipped .whl below but pip doesn't like it.
        #
        raise Exception(f'bdist_wheel not implemented')

        assert opt_d
        if opt_mupdf_fake_build:
            log(f'*** Doing fake build of sos.')
            command = f'cd {mupdf_dir}'
            command += ' && pwd && ls -la'
            command += f' && mkdir -p {build_dir}'
            for path in paths:
                leaf = os.path.basename(path)
                command += f' && rsync -ai /home/jules/artifex/mupdf/build/shared-release/{leaf} {mupdf_dir}/{build_dir}/'
        else:
            command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -d {build_dir} -b all'

        log(f'Building mupdf C, C++ and Python libraries with: {command}')
        subprocess.check_call(command, shell=True)

        with zipfile.ZipFile(f'{opt_d}/mupdf-{mupdf_version()}-py3-none-any.whl', 'w') as f:
            for path in paths:
                f.write(path, os.path.basename(path))
            d = f'mupdf-{mupdf_version()}.dist-info'
            #f.writestr(f'{d}/WHEEL',
            f.writestr(f'WHEEL',
                    ''
                    + 'Wheel-Version: 1.0\n'
                    + 'Generator: bdist_wheel\n'
                    + 'Root-Is-Purelib: false\n'
                    + 'Tag: py3-none-any\n'
                    )
            f.writestr(f'PKG-INFO', metainfo())

    elif command == 'egg_info':
        assert opt_egg_base
        root = f'{opt_egg_base}/.egg-info'
        os.mkdir(root)
        with open(f'{root}/PKG-INFO', 'w') as f:
            f.write(metainfo())
        #with open(f'{root}/METADTA', 'w') as f:
        #    f.write(metainfo)
        with open(f'{root}/SOURCES.txt', 'w') as f:
            pass
        with open(f'{root}/dependency_links.txt', 'w') as f:
            pass
        with open(f'{root}/top_level.txt', 'w') as f:
            f.write('mupdf\n')

    elif command == 'install':
        if opt_mupdf_fake_build:
            log(f'*** Doing fake build of sos.')
            command = f'cd {mupdf_dir}'
            command += ' && pwd && ls -la'
            command += f' && mkdir -p {build_dir}'
            for path in paths:
                leaf = os.path.basename(path)
                command += f' && rsync -ai /home/jules/artifex/mupdf/build/shared-release/{leaf} {mupdf_dir}/{build_dir}/'
        else:
            command = f'cd {mupdf_dir} && ./scripts/mupdfwrap.py -d {build_dir} -b all'

        log(f'Building mupdf C, C++ and Python libraries with: {command}')
        subprocess.check_call(command, shell=True)

        # Check whether extra files exist.
        #
        missing = []
        for path in paths:
            if not os.path.isfile(path):
                missing.append(path)
        if missing:
            log('Warning: mupdf binaries have not been built and are missing:')
            for path in missing:
                log(f'    {path}')
            log()

        sitepackages = site.getsitepackages()[0]

        # If we create a <opt_record> file, pip complains that we haven't
        # indicated that we've installed an .egg-info directory. So for now we
        # ignore <opt_record>.
        #
        f = 0   #open(opt_record, 'w') if opt_record else None
        try:
            for path in paths:
                log( f'Copying {path} to {sitepackages}')
                dest = f'{sitepackages}/{os.path.basename(path)}'
                shutil.copy2( path, dest)
                if f:
                    f.write(f'{dest}\n')
        finally:
            if f:
                f.close()

    elif command == 'sdist':
        if opt_mupdf_fake_build:
            paths = [
                    'include/mupdf/fitz/version.h',
                    'setup.py',
                    'README',
                    'COPYING',
                    ]
        else:
            sys.path.append(f'{mupdf_dir}/scripts')
            import jlib
            paths = jlib.get_gitfiles( f'{mupdf_dir}', submodules=True)
        shutil.rmtree(f'{mupdf_dir}/{opt_dist_dir}', ignore_errors=True)
        os.mkdir(f'{mupdf_dir}/{opt_dist_dir}')
        version = mupdf_version()
        tarpath = f'{mupdf_dir}/{opt_dist_dir}/mupdf-{version}.tar.gz'
        def add(tar, name, contents):
            '''
            Adds item called <name> to tarfile.TarInfo <tar>, containing
            <contents> (which must be bytes, not str).
            '''
            ti = tarfile.TarInfo(name)
            ti.size = len(contents)
            tar.addfile(ti, io.BytesIO(contents))
        with tarfile.open(tarpath, 'w:gz') as tar:
            for path in paths:
                p = f'{mupdf_dir}/{path}'
                if os.path.isdir(p):
                    # This appears to happen for sub-sub-modules.
                    log(f'*** Ignoring git ls-files item that is actually a directory: {p}')
                else:
                    tar.add( p, path, recursive=False)
            add(tar, 'PKG-INFO', metainfo().encode('utf8'))
        log( f'Have created: {tarpath}')

    else:
        assert 0, f'Unrecognised command: {command}'


if __name__ == '__main__':
    main()
