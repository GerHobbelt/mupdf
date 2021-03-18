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
        print(f'pipcl.py: {line}')
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
        A string, used in metainfo.
    fn_clean
        A function taking a single arg <all_> that cleans generated files.
        <all_> is true iff --all is in argv.
    fn_sdist
        A function taking no args that returns a list of paths, e.g. from
        git_items(), for files that should be copied into the sdist.
    fn_build
        A function taking no args that builds everything.

        Should return a list of items, each item being a tuple of two strings
        (from_, to_), or a single string <path> which is treated as a tuple
        (path, path).

        <from_> should be the path to a file. <to_> identifies what the file
        should be called when generating a wheel or installing.

        The 'bdist_wheel' command will copy <from_> to <to_> in the wheel.

        The 'install' command will copy <from_> to <sitepackages>/<to_> where
        <sitepackages> is Python's site.getsitepackages()[0].

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
        Returns text for .egg-info/PKG-INFO file, or PKG-INFO in an sdist
        tar.gz file, or ...dist-info/METADATA in a wheel.
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

    opt_all = None
    opt_dist_dir = 'dist'
    opt_egg_base = None
    opt_install_headers = None
    opt_record = None

    args = Args(argv[1:])

    while 1:
        arg = args.next(None)
        if arg is None:
            break

        elif arg in ('bdist_wheel', 'clean', 'egg_info', 'install', 'sdist'):
            assert command is None, 'Two commands specified: {command} and {arg}.'
            command = arg

        elif arg == '--all':
            opt_all = True

        elif arg == '--compile':
            pass

        elif arg == '--dist-dir' or arg == '-d':
            opt_dist_dir = args.next()

        elif arg == '--egg-base':
            opt_egg_base = args.next()

        elif arg in ('-h', '--help'):
            log(
                    'Options:\n'
                    '    bdist_wheel\n'
                    '        Creates a wheel called\n'
                    '        <dist-dir>/<name>-<version>-<details>.whl, where\n'
                    '        <dist-dir> is "dist" or as specified by --dist-dir,\n'
                    '        and <details> encodes ABI and platform etc.\n'
                    '    clean\n'
                    '        Cleans build files.\n'
                    '    egg_info\n'
                    '        Creates files in: <egg-base>/.egg-info/, where\n'
                    '        <egg-base> is as specified with --egg-base.\n'
                    '    install\n'
                    '        Installs into Python\'s\n'
                    '        site.getsitepackages()[0]. Writes installation\n'
                    '        information to <record> if --record\n'
                    '        was specified.\n'
                    '    sdist\n'
                    '        Make a source distribution:\n'
                    '            <dist-dir>/<name>-<version>.tar.gz\n'
                    '    --dist-dir | -d <dist-dir>\n'
                    '        Default is "dist".\n'
                    '    --egg-base <egg-base>\n'
                    '        Used by "egg_info".\n'
                    '    --record <record>\n'
                    '        Used by "install".\n'
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
        #assert 0, 'deliberate failure for bdist_wheel'

        os.makedirs(opt_dist_dir, exist_ok=True)

        items = []
        if fn_build:
            items = fn_build()

        path = f'{opt_dist_dir}/{name}-{version}-py3-none-any.whl'
        log(f'creating wheel/zip: {path}')

        with zipfile.ZipFile(path, 'w') as z:

            def add_file(from_, to_):
                z.write(from_, to_)
                record.add_file(from_, to_)

            def add_str(content, to_):
                z.writestr(to_, content)
                record.add_content(content, to_)

            # Add the files returned by fn_build().
            #
            for item in items:
                from_, to_ = fromto(item)
                add_file(from_, to_)

            # Add ...dist-info/*.
            #
            d = f'{name}-{version}.dist-info'
            content = (''
                    + 'Wheel-Version: 1.0\n'
                    + 'Generator: bdist_wheel\n'
                    + 'Root-Is-Purelib: false\n'
                    + 'Tag: py3-none-any\n'
                    )
            add_str(content, f'{d}/WHEEL')
            add_str(metainfo(), f'{d}/METADATA')

            # Add license files.
            #
            if license_files:
                if isinstance(license_files, str):
                    license_files = license_files,
                for l in license_files:
                    from_, to_ = fromto(l)
                    add_file(from_, f'{d}/{to_}')

            # Update ...dist-info/RECORD. This must be last.
            #
            #log(f'RECORD is:\n{record[0]}')
            z.writestr(f'{d}/RECORD', record.get())

    elif command == 'clean':
        if fn_clean:
            fn_clean(opt_all)

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
        items = []
        if fn_build:
            items = fn_build()

        sitepackages = site.getsitepackages()[0]

        record = Record() if opt_record else None
        for item in items:
            from_, to_ = fromto(item)
            to_path = f'{sitepackages}/{to_}'
            log(f'copying from {from_} to {to_path}')
            shutil.copy2( from_, f'{to_path}')
            if record:
                # Could maybe use relative path of to_path from sitepackages/.
                record.add_file(from_, to_)

        if opt_record:
            with open(opt_record, 'w') as f:
                f.write(record.get())

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
                if os.path.abspath(path).startswith(f'{os.path.abspath(opt_dist_dir)}/'):
                    # Ignore files inside <opt_dist_dir>, to save fn_sdist()
                    # from worrying about this.
                    continue
                #log(f'path={path}')
                tar.add( path, path, recursive=False)
            add(tar, 'PKG-INFO', metainfo().encode('utf8'))

        log( f'Have created: {tarpath}')

    else:
        assert 0, f'Unrecognised command: {command}'


class Record:
    '''
    Builds up text suitable for writing to a RECORD item, e.g. within a wheel.
    '''
    def __init__(self):
        self.text = ''

    def add_content(self, content, to_):
        if isinstance(content, str):
            content = content.encode('latin1')
        h = hashlib.sha256(content)
        digest = h.digest()
        digest = base64.urlsafe_b64encode(digest)
        self.text += f'{to_},sha256={digest},{len(content)}\n'

    def add_file(self, from_, to_):
        with open(from_, 'rb') as f:
            content = f.read()
        self.add_content(content, to_)

    def get(self):
        return self.text
