'''
Support for implementing a setup.py package script.
'''

import base64
import hashlib
import io
import os
import shutil
import site
import subprocess
import sys
import tarfile
import zipfile


def log(text=''):
    '''
    Logs lines with prefix.
    '''
    for line in text.split('\n'):
        print(f'pipcl.py: {line}', file=sys.stdout)
    sys.stdout.flush()


def git_items( directory, submodules=False):
    '''
    Helper for run()'s fn_sdist() callback.

    <directory> must be somewhere within a git checkout.

    Returns list of paths for all files known to git within <directory>. Each
    path starts with <directory>.

    We use the 'git ls-files' command.
    '''
    assert os.path.isdir( '%s/.git' % directory)
    command = 'cd ' + directory + ' && git ls-files'
    if submodules:
        command += ' --recurse-submodules'
    text = subprocess.check_output( command, shell=True)
    ret = []
    for path in text.decode('utf8').strip().split( '\n'):
        path2 = os.path.join(directory, path)
        if os.path.isdir(path2):
            log(f'*** Ignoring git ls-files item that is actually a directory: {path2}')
        else:
            ret.append(path2)
    return ret


def run(
        argv,
        name,
        version,
        summary = None,
        description = None,
        classifiers = None,
        author = None,
        author_email = None,
        url_docs = None,
        url_home = None,
        url_source = None,
        url_tracker = None,
        keywords = None,
        platform = None,
        fn_clean = None,
        fn_sdist = None,
        fn_build = None,
        license_files = None,
        ):
    '''
    Handles command-line argv passed to setup.py.

    The args we are passed define the Python package that we work with.

    argv
        E.g. sys.argv. argv[0] is ignored.
    name
        A string, the name of the Python package.
    version
        A string containing only 0-9 and '.'.
    summary
        A string.
    description
        A string.
    classifiers
        A list of strings.
    url_home
    url_source
    url_docs
    utl_tracker
        Strings containing particular URLs.
    keywords
        A string containing space-separated keywords, or a list of keywords.
    platform
        A string.
    fn_clean
        A function taking no args that cleans generated files.
    fn_sdist
        A function taking no args that returns a list of paths, e.g. from
        git_items(), for files that should be copied into the sdist.
    fn_build
        A function taking no args that builds everything.

        Should return (pys, sos, datas), each a list of items, each item being
        either a string <path> or a tuple (from_, to_). <path> is treated as a
        tuple (path, path).

            pys: .py files. For each item, we copy from_ to
            .../site-packages/<to_>.

            sos: .so files. For each item, we copy <from_> to
            .../site-packages/<to_>.

            datas: misc files. For each item, we copy <from_> to
            .../site-packages/<to_>.

        E.g. fn_build() could return:

                (
                    [
                        ('build/foo/__init__.py', 'foo/__init__.py'),
                        ('build/bar.py', 'bar.py'),
                        'wibble.py',
                    ],
                    [
                        ('build/objs/_wibble.so', '_wibble.so'),
                    ],
                    [
                        ('src/data.txt', 'data/data.txt'),
                    ]
                )

        And this would result in the installed package looking like:

            .../site-packages/
                foo/__init__.py
                bar.py
                wibble.py
                _wibble.so
                data/data.txt
    license_files
        Either string name of license file or list of names of license files.
    '''

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

    def metainfo():
        '''
        Returns text for PKG-INFO file (e.g. in .egg-info/ directory or
        top-level in an sdist tar.gz file).
        '''
        # We use an array here so that local add() function can modify ret[0].
        #
        ret = ['']
        def add(key, value):
            if value is not None:
                ret[0] += f'{key}: {value}\n'
        add('Metadata-Version', '1.2')
        add('Name', name)
        add('Version', version)
        add('Summary', summary)
        add('Home-page', url_home)
        add('Platform', platform)
        add('Author', author)
        add('Author-email', author_email)
        if url_source:  add('Home-page', f'Source, {url_source}')
        if url_docs:    add('Home-page', f'Source, {url_docs}')
        if url_tracker: add('Home-page', f'Source, {url_tracker}')
        if keywords:
            keywords2 = keywords
            if isinstance(keywords2, str):
                keywords2 = keywords2.split()
            keywords2 = ' '.join(keywords2)
            add('Keywords', keywords2)
        if classifiers:
            classifiers2 = classifiers
            if isinstance(classifiers2, str):
                classifiers2 = classifiers2.split('\n')
            for classifier in classifiers2:
                add('Classifier', classifier)
        return ret[0]

    command = None

    opt_egg_base = None
    opt_install_headers = None
    opt_record = None
    opt_d = None
    opt_dist_dir = 'dist'

    args = Args(argv[1:])

    while 1:
        arg = args.next(None)
        if arg is None:
            break

        elif arg in ('bdist_wheel', 'clean', 'egg_info', 'install', 'sdist'):
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
                    '    clean\n'
                    '        Cleans build files.\n'
                    '    egg_info\n'
                    '        Creates files in: <egg-base>/.egg-info/\n'
                    '    install\n'
                    '        Installs into Python\'s site.getsitepackages()[0].\n'
                    '        Logs to <record> (not implemented).\n'
                    '    sdist\n'
                    '        Make a source distribution:\n'
                    '            <dist-dir>/<name>-<version>.tar.gz\n'
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
                    )
            return

        elif arg == '--help-commands':
            print('help')
            return

        elif arg == '--install-headers':
            opt_install_headers = args.next()

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

    def fromto(p):
        if isinstance(p, str):
            return p, p
        elif len(p) == 2:
            from_, to_ = p
            if isinstance(from_, str) and isinstance(to_, str):
                return from_, to_
        assert 0, 'p should be str or (str, str), but is: {p}'

    log(f'Handling command={command}')
    if 0:
        pass

    elif command == 'bdist_wheel':
        # Can't figure out how bdist_wheel is supposed to work. We can create a
        # zipped .whl below but pip doesn't like it.
        #
        #raise Exception(f'bdist_wheel not implemented')
        if opt_d is None:
            opt_d = 'dist'
        os.makedirs(opt_d, exist_ok=True)

        psy = sos = datas = []
        if fn_build:
            pys, sos, datas = fn_build()

        path = f'{opt_d}/{name}-{version}-py3-none-any.whl'
        log(f'creating wheel/zip: {path}')

        with zipfile.ZipFile(path, 'w') as z:

            record = ['']

            def record_update(content, to_):
                if isinstance(content, str):
                    content = content.encode('latin1')
                h = hashlib.sha256(content)
                digest = h.digest()
                log(f'digest={digest}')
                digest2 = base64.urlsafe_b64encode(digest)
                log(f'digest2={digest2}')
                record[0] += f'{to_},sha256={digest2},{len(content)}\n'

            def add_file(from_, to_):
                z.write(from_, to_)
                with open(from_, 'rb') as f:
                    content = f.read()
                record_update(content, to_)

            def add_str(content, to_):
                z.writestr(to_, content)
                record_update(content, to_)

            for item in pys + sos + datas:
                from_, to_ = fromto(item)
                to_ = f'{name}/{to_}'
                add_file(from_, to_)

            d = f'{name}-{version}.dist-info'
            content = (''
                    + 'Wheel-Version: 1.0\n'
                    + 'Generator: bdist_wheel\n'
                    + 'Root-Is-Purelib: false\n'
                    + 'Tag: py3-none-any\n'
                    )
            add_str(content, f'{d}/WHEEL')

            add_str(metainfo(), f'{d}/METADATA')

            if license_files:
                if isinstance(license_files, str):
                    license_files = license_files,
                for l in license_files:
                    from_, to_ = fromto(l)
                    add_file(from_, f'{d}/{to_}')

            log(f'RECORD is:\n{record[0]}')
            z.writestr(f'{d}/RECORD', record[0])

    elif command == 'clean':
        if fn_clean:
            fn_clean()

    elif command == 'egg_info':
        assert opt_egg_base
        root = f'{opt_egg_base}/.egg-info'
        log(f'{command}: creating files in root={root}')
        os.mkdir(root)
        with open(f'{root}/PKG-INFO', 'w') as f:
            f.write(metainfo())
        with open(f'{root}/SOURCES.txt', 'w') as f:
            pass
        with open(f'{root}/dependency_links.txt', 'w') as f:
            pass
        with open(f'{root}/top_level.txt', 'w') as f:
            f.write(f'{name}\n')

    elif command == 'install':
        if fn_build:
            pys, sos, datas = fn_build()

        sitepackages = site.getsitepackages()[0]

        # If we create a <opt_record> file, pip complains that we haven't
        # indicated that we've installed an .egg-info directory. So for now we
        # ignore <opt_record>.
        #
        f = 0   #open(opt_record, 'w') if opt_record else None
        try:
            # For now, we copy everything into .../site-packages/.
            for item in pys + sos + datas:
                from_, to_ = fromto(item)
                to_path = f'{sitepackages}/{to_}'
                log( f'Copying {from_} to {to_path}')
                shutil.copy2( from_, f'{to_path}')
                if f:
                    f.write(f'{to_path}\n')
        finally:
            if f:
                f.close()

    elif command == 'sdist':
        if fn_sdist:
            paths = fn_sdist()
        else:
            paths = []
        shutil.rmtree(f'{opt_dist_dir}', ignore_errors=True)
        os.mkdir(f'{opt_dist_dir}')

        def add(tar, name, contents):
            '''
            Adds item called <name> to tarfile.TarInfo <tar>, containing
            <contents> (which must be bytes, not str).
            '''
            ti = tarfile.TarInfo(name)
            ti.size = len(contents)
            tar.addfile(ti, io.BytesIO(contents))

        tarpath = f'{opt_dist_dir}/{name}-{version}.tar.gz'
        with tarfile.open(tarpath, 'w:gz') as tar:
            for path in paths:
                tar.add( path, path, recursive=False)
            add(tar, 'PKG-INFO', metainfo().encode('utf8'))

        log( f'Have created: {tarpath}')

    else:
        assert 0, f'Unrecognised command: {command}'
