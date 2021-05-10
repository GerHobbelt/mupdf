#!/usr/bin/env python3

'''
Support for creating Python wheels and uploading to pypi.org.

Overview:

    We expect to be run in the directory containing a package's setup.py and/or
    pyproject.toml.

    On Unix we use a manylinux docker container.

    On Windows we use the 'py' command to find different installed Pythons.

Command-line usage:

    Note that args are handled sequentially. So for example with 'pypackage.py
    --sdist bar.tar.gz --build-wheels --sdist bar.tgz', wheels will be built
    from bar.tar.gz. option.

    Examples:

        Build packages on two remote systems and copy back to local system,
        then check that there is a generated wheel that matches the host system
        and that 'import mypackage' works on the local system, then update
        sdist and all wheels to test.pypi.org using 'twine upload'.

            pypackage.py \
                    --build-sdist \
                    --build-wheels-remote user@foo:build-dir \
                    --build-wheels-remote user@foo-win:build \
                    --test mypackage '' \
                    --pypi-test 1 \
                    --upload

        Install
        pypackage.py --remote-args somehost:testing "--wheels 'pypackage-out/*' --test ''"

    Args:

        --abis <abis>
            Set the list of ABIs for which --build-wheels builds wheels.

            abis:
                Comma-separated list of <abi>.
                On Windows:
                    abi: <cpu>-<python-version>
                        cpu:
                            'x32' or 'x64'.
                On Unix:
                    abi: <python-version>

                python-version:
                    Python version; may contain '.'. E.g. '38' or '3.9'.

            Examples:
                Windows: --abis x32-38,x32-39,x64-39,x64-39
                Unix: --abis 38,39

            Default is:
                Unix: 37,38,39
                Windows: x32-38,x32-39,x64-38,x64-39

        --build-sdist <package-directory>
            Builds sdist for source tree <package-directory> in <outdir>.

            Currently works by running <package-directory>/setup.py.

            Either this or '--sdist <sdist>' must be
            specified before --build-wheels*.

        --build-wheels
            Build wheels and copy them into <outdir>.

            If --sdist has not been specified earlier, we first build an sdist
            in the current tree and copy into <outdir>.

            We extract the sdist (locally on Windows, in a container on Unix)
            and build wheels in the extracted tree, finally copying them into
            <outdir>.

            todo: provide an option to use 'pip wheel <sdist>'. This would
            force clean build each time so can be slow when experimenting, but
            is probably more correct.

        --build-wheels-remote [<options>] <remote>
            Builds wheels on remote host and copies them back to local
            <outdir>.

            remote
                [<user>@]<host>:[<directory>]

            options
                -s 0 | 1
                    If 0 we don't rsync local sdist to remote machine, and
                    assume it is already there.

        --clean <outdir>
            Removes any *.whl or *.tar.gz files from <outdir>. For safety it
            asserts that <outdir> contains 'pypackage'.

        -h
        --help
            Show this help.

        --manylinux-c <container-name>
            [Linux only]
            Name of container to create/start/use. Default is 'pypackage'.

        --manylinux-d 0 | 1
            [Linux only]
            Whether to ensure that docker is installed. Default is 0.

        --manylinux-i <docker-image>
            [Linux only]
            Set docker image; default is 'quay.io/pypa/manylinux2014_x86_64'.

        --manylinux-p 0 | 1
            [Linux only]
            Whether to use 'docker pull' to get/update container image. Default
            is 1.

        -o <outdir>
            Set the directory in which to put generated wheels. Default is
            'pypackage-out'.

        --pypi-test 0 | 1
            Whether we use the pypi test repository for --upload and --test.

            Default is 1.

        --remote-args [<user>@]<host>:[<directory>] <args>
            Runs pypackage.py on remote host in specified directory, passing
            <args>.

            We rsync pypackage.py to the remote host and then use
            ssh to run remote command of the form:

                ssh user@host 'cd test-dir && ./pypackage.py <args>'

            For example:
                --remote-args foo@machine:test-directory "--wheels 'pypackage-out/*' --test ''"
                --remote-args foo@machine:test-directory "--test -i mymodule ''"

        --sdist <sdist>
            Specify path of existing sdist. If specified, wheels are built by
            extracting the sdist (either locally on Windows or inside docker
            container on Unix) and building inside this resulting tree.

            Otherwise we build a new sdist as required by running './setup.py
            sdist' locally.

        --test [-i <package-name>] <code>
            Tests installation of local or pypi wheel by running <code> inside
            a clean Python venv. For example, <code> could be: 'import foo;
            foo.test()'.

            On Windows the venv will use "py"; otherwise we use sys.executable,
            i.e. whatever python is running this script itself.

            If '-i <package-name>' is specified we do 'pip install
            <package-name>', using pypi's test repository depending on previous
            --pypi arg. Otherwise we do 'pip install <wheel>' where <wheel> is
            found by searching <wheels> for a match for the python we run.

            If <code> is an empty string, we infer a package name either from
            the -i arg or from the name of the matching wheel, and set <code>
            to 'import <package-name>'.

        --upload
            Uploads <sdist> and <wheels> to pypi.org or test.pypi.org depending on
            previous --pypi arg, using 'twine upload'.

        --wheels <pattern>
            Specify wheel files that already exist.

            <pattern> should be a glob that matches previously-build wheels; we
            only use matches that end with ".whl" so for example one can use:

                --wheels 'pypackag-out/*'

            The resulting list of wheels is used by later occurrencies of
            --test, --upload etc.

Docker notes:
    Interactive session inside docker instance:
        docker exec -it pypackage bash
'''

import distutils.util
import glob
import os
import platform
import re
import shutil
import subprocess
import sys
import tarfile
import time

try:
    import jlib
except ModuleNotFoundError as e:
    print(f'Unable to import jlib: {e}')
    jlib = None


def log(text=''):
    '''
    Logs lines with prefix.
    '''
    if jlib:
        return jlib.log(text, caller=2)
    for line in text.split('\n'):
        print(f'pypackage.py: {line}')
    sys.stdout.flush()


def system(command, raise_errors=True, return_output=False, prefix=None, caller=1, out_log_caller=1):
    if jlib:
        return jlib.system(
                command,
                verbose=not return_output,
                raise_errors=raise_errors,
                out='return' if return_output else 'log',
                prefix=prefix,
                out_log_caller=caller+1,
                )
    else:
        log(f'Running: {command}')
        ret = subprocess.run(
                command,
                shell=True,
                universal_newlines=True,
                stdout=subprocess.PIPE if return_output else None,
                stderr=subprocess.STDOUT if return_output else None,
                )
        if raise_errors:
            ret.check_returncode()
        if return_output:
            return ret.stdout
        return ret.returncode


def windows():
    s = platform.system()
    return s == 'Windows' or s.startswith('CYGWIN')

def unix():
    return not windows()


def make_tag(py_version=None):
    '''
    py_version:
        None or E.g. '3.9.4'. If None we use native Python version.
    cpu:
    '''
    # Find platform tag used in wheel filename, as described in
    # PEP-0425. E.g. 'openbsd_6_8_amd64', 'win_amd64' or 'win32'.
    #
    tag_platform = distutils.util.get_platform().replace('-', '_').replace('.', '_')

    if py_version is None:
        py_version = platform.python_version()
    # Get two-digit python version, e.g. 3.8 for python-3.8.6.
    #
    tag_python = ''.join(py_version.split('.')[:2])

    # Final tag is, for example, 'py39-none-win32', 'py39-none-win_amd64'
    # or 'py38-none-openbsd_6_8_amd64'.
    #
    tag = f'py{tag_python}-none-{tag_platform}'
    return tag


venv_installed = set()

def venv_run(
        commands,
        venv='pypackage-venv',
        py=None,
        clean=False,
        return_output=False,
        directory=None,
        prefix=None,
        pip_upgrade=True,
        ):
    '''
    Runs commands inside Python venv, joined by &&.

    commands:
        List of commands to run.
    venv:
        Name of venv to create and use. Relative to <directory> if <directory>
        is not None.
    py:
        Python to run. If None we use sys.executable.
    clean:
        If true we first delete any existing <venv>. For safety we assert that
        <venv> starts with 'pypackage-venv'.
    directory:
        Directory in which to create venv and run <commands>.
    prefix:
        Prefix to prepend to each output line.
    pip_upgrade:
        If true (the defautl) we do 'pip install --upgrade pip' before running
        <commands>.
    '''
    if isinstance(commands, str):
        commands = [commands]
    if py is None:
        py = sys.executable
    if windows():
        pre = [
                f'cmd.exe /c "'
                f'{"cd "+directory if directory else "true"}',
                f'{py} -m venv {venv}',
                f'{venv}\\Scripts\\activate.bat',
                ]
        post = [f'deactivate"']
    else:
        pre = [
                f'{"cd "+directory if directory else "true"}',
                f'{py} -m venv {venv}',
                f'. {venv}/bin/activate',
                ]
        post = [f'deactivate']

    if clean or venv not in venv_installed:
        if clean:
            assert venv.startswith('pypackage-venv')
            shutil.rmtree(venv, ignore_errors=1)
        venv_installed.add(venv)

        if pip_upgrade:
            pre += [
                    'pip install --upgrade pip',
                    ]
            if windows():
                # It looks like first attempt to upgrade pip can fail with
                # 'EnvironmentError: [WinError 5] Access is denied'. So we make an
                # extra first attempt.
                #
                commands0 = pre + post
                command0 = '&&'.join(commands0)
                log(f'Running pre-install of pip: {command0}')
                e = system(command0, raise_errors=False, prefix=prefix)
                if e:
                    log(f'Ignoring error from first run of pip install')

    commands = pre + commands + post

    if windows():
        command = '&&'.join(commands)
    else:
        command = ' && '.join(commands)

    return system(command, return_output=return_output, prefix=prefix)


def check_sdist(sdist):
    '''
    Checks sdist with 'twine check'.
    '''
    venv_run([
            f'pip install twine',
            f'twine check {sdist}',
            ])


def check_wheel(wheel):
    '''
    Checks wheel with 'twine check'.
    '''
    # We don't install and use check-wheel-contents, because it thinks
    # top-level .dll files are incorrect.
    venv_run([
            f'pip install twine',
            #f'check-wheel-contents {wheel}',
            f'twine check {wheel}',
            ])


def find_new_file(pattern, t):
    '''
    Finds file matching <pattern> whose mtime is >= t. Raises exception if
    there isn't exactly one such file.
    '''
    paths = glob.glob(pattern)
    paths_new = []
    for path in paths:
        tt = os.path.getmtime(path)
        if tt >= t:
            paths_new.append(path)

    if len(paths_new) == 0:
        raise Exception(f'No new file found matching glob: {pattern}')
    elif len(paths_new) > 1:
        text = 'More than one file found matching glob: {pattern}\n'
        for path in paths_new:
            text += f'    {path}\n'
        raise Exception(text)

    return paths_new[0]


def make_sdist(package_directory, out_directory):
    '''
    Creates sdist from source tree <package_directory>, in <out_directory>. Returns
    the path of the generated sdist.
    '''
    os.makedirs(out_directory, exist_ok=True)
    t = time.time()
    command = (
            f'cd {os.path.relpath(package_directory)}'
            f' && ./setup.py sdist -d "{os.path.relpath(out_directory, package_directory)}"'
            )
    system(command)

    # Find new file in <sdist_directory>.
    sdist = find_new_file(f'{out_directory}/*.tar.gz', t)
    check_sdist(sdist)

    return sdist


def make_unix(
        sdist,
        abis=None,
        out_dir=None,
        test_direct_install=False,
        install_docker=None,
        docker_image=None,
        pull_docker_image=None,
        container_name=None,
        ):
    '''
    Builds Python wheels using a manylinux container.

        sdist:
            Pathname of sdist file; we copy into container and extract there.
        abis
            List of string python versions for which we build
            wheels. Any '.' are removed and we then take the first two
            characters. E.g. ['3.8.4', '39'] is same as ['38', '39'].
        out_dir
            If not None, the directory into which we generated wheels.
        test_direct_install
            If true we run 'pip install <sdist>' in the container.
            If None we default to false.
        install_docker
            If true we attempt to install docker (currently specific to
            Debian/Devuan).
            If None we default to false.
        docker_image
            Name of docker image to use.
            If None we default to: quay.io/pypa/manylinux2014_x86_64
        pull_docker_image
            If true we run 'docker pull ...' to update the image.
            If None we default to true.
        container_name
            Name to use for the container.
            If None we default to 'pypackage'.

    Returns wheels, a list of wheel pathnames.
    '''
    if abis is None:
        abis = ['37', '38', '39']
    if test_direct_install is None:
        test_direct_install = False
    if install_docker is None:
        install_docker = False
    if docker_image is None:
        docker_image='quay.io/pypa/manylinux2014_x86_64'
    if pull_docker_image is None:
        pull_docker_image = True
    if container_name is None:
        container_name = 'pypackage'

    io_directory = 'pypackage-io'

    assert sdist.endswith('.tar.gz')
    sdist_leaf = os.path.basename(sdist)    # e.g. mupdf-1.18.0.20210504.1544.tar.gz
    package_root = sdist_leaf[ : -len('.tar.gz')]   # e.g. mupdf-1.18.0.20210504.1544

    check_sdist(sdist)

    if install_docker:
        # Note that if docker is not already installed, we will
        # need to add user to docker group, e.g. with:
        #   sudo usermod -a -G docker $USER
        system('sudo apt install docker.io')

    # Copy sdist into what will be the container's /io/ directory.
    #
    os.makedirs( io_directory, exist_ok=True)
    system( f'rsync -ai {sdist} {io_directory}/')

    if pull_docker_image:
        # Ensure we have latest version of the docker image.
        #
        system(f'docker pull {docker_image}')

    if 1:
        # Ensure docker instance is created and running. Also give it a name
        # <container_name> so we can refer to it below.
        #
        # '-itd' ensures docker instance runs in background.
        #
        # With '-v HOST:CONTAINER', HOST must be absolute path.
        #
        # It's ok for this to fail - container is already created and running.
        #
        e = system(
            f' docker run'
            f' -itd'
            f' --name {container_name}'
            f' -v {os.path.abspath(io_directory)}:/io'
            f' {docker_image}'
            ,
            raise_errors=False,
            )
        log(f'docker run: ignoring any error. e={e}')

        # Start docker instance if not already started.
        #
        e = system(f'docker start {container_name}', raise_errors=False,)
        log(f'docker start: e={e}')

    def docker_system(command, prefix='', return_output=False):
        '''
        Runs specified command inside container. Runs via bash so <command> can
        contain shell constructs. If out is 'capture', we return the output text.
        '''
        command = command.replace('"', '\\"')
        command = f'docker exec {container_name} bash -c "{command}"'
        log(f'Running: {command}')
        return system(
                command,
                return_output=return_output,
                prefix=None if return_output else f'container: {prefix}',
                out_log_caller=2,
                )

    if test_direct_install:
        # Test direct intall from sdist.
        #
        docker_system( f'pip3 -vvv install /io/{sdist_leaf}')

    container_pythons = []
    for abi in abis:
        vv = abi.replace('.', '')[:2] # E.g. '38' for Python-3.8.
        pattern = f'/opt/python/cp{vv}-cp{vv}*'
        paths = docker_system(f'ls -d {pattern}', return_output=1)
        log(f'vv={vv}: paths={paths!r}')
        paths = paths.strip().split('\n')
        assert len(paths) == 1, f'No single match for glob pattern in container. pattern={pattern}: {paths}'
        container_pythons.append( (vv, paths[0]))

    # In the container, extract <sdist>; we will use the extracted sdist to
    # build wheels.
    #
    docker_system( f'tar -xzf /io/{sdist_leaf}')

    # Build wheels inside container.
    wheels = []
    for vv, container_python in container_pythons:
        log(f'Building wheel with python {vv}: {container_python}')

        # Build wheel.
        t = time.time()
        docker_system(
                f'cd {package_root} && {container_python}/bin/python ./setup.py --dist-dir /io bdist_wheel',
                prefix=f'{container_python}: '
                )

        # Find new wheel.
        wheel = find_new_file(f'{io_directory}/{package_root}-py{vv}-none-*.whl', t)
        wheels.append(wheel)
        check_wheel(wheel)
        if out_dir:
            shutil.copy2(wheel, out_dir)

    log( f'wheels are ({len(wheels)}):')
    for wheel in wheels:
        log( f'    {wheel}')

    return wheels


def make_windows(
        sdist,
        abis=None,
        out_dir=None,
        ):
    '''
    Builds Python wheels on Windows.

    sdist:
        Pathname of sdist; we extract it and generate wheels inside the
        extracted tree. Note that this means that running a second time with
        the same sdist will not be a clean build.

    abis:
        List of <cpu>-<pythonversion> strings.
            cpu:
                'x32' or 'x64'
            pythonversion:
                String version number, may contain '.' characters, e.g. '3.8'
                or '39'.
    out_dir:
        Directory into which we copy the generated wheels. Defaults to
        'pypackage-out'.

    Returns wheels, a list of wheel pathnames.
    '''
    log(f'sdist={sdist} abis={abis} out_dir={out_dir}')
    assert windows()
    if abis is None:
        abis = ['x32-38', 'x32-39', 'x64-38', 'x64-39']
    if out_dir is None:
        out_dir = 'pypackage-out'

    # Extact sdist and run ourselves inside the extracted tree. We make
    # basic checks that this will extract to a single subdirectory.
    log(f'Extracting sdist={sdist}')
    with tarfile.open(sdist, 'r:gz') as t:
        items = t.getnames()
        assert items
        item = items[0]
        #log(f'item={item}')
        assert not item.startswith('.')
        s = item.find('/')
        assert s > 0
        prefix = item[:s+1]
        for item in items:
            assert item.startswith(prefix)
        t.extractall()
        directory = prefix

    os.makedirs(out_dir, exist_ok=True)
    out_dir2 = os.path.relpath(os.path.abspath(out_dir), directory)

    wheels = []
    for abi in abis:
        cpu, python_version = abi.split('-')
        python_version = '.'.join(python_version)
        assert cpu in ('x32', 'x64')
        py = f'py -{python_version}-{cpu[1:]}'
        t = time.time()
        log(f'*** Running venv_run() with directory={directory} os.getcwd()={os.getcwd()}')
        venv_run(
                [
                f'pwd',
                f'python setup.py -d {out_dir2} bdist_wheel',
                ],
                py=py,
                directory=directory,
                prefix=f'{python_version}-{cpu} wheel build: ',
                )
        wheel = find_new_file(f'{out_dir}/*.whl', t)
        check_wheel(wheel)
        wheels.append(wheel)

    return wheels


def test(code, wheels_or_package_name, pypi_test, py=None):
    '''
    Basic testing of wheels.

    Finds wheel in <wheels> that matches <py>, installs in temporary venv, and
    runs 'python <code>'.

    code:
        Python code to run. E.g. 'import foo\nfoo.qwerty()\n'
    wheels_or_package_name:
        If a string, the name of a package to install using pip. Otherwise
        should be a list of wheel paths to be searched for a match for <py>.
    pypi_test:
        Only used if wheels_or_package_name is a string. If true we use pypi's
        test repository.
    py:
        Python executable. If None we use 'py' on Windows else sys.executable.
    '''
    assert wheels_or_package_name
    if py is None:
        py = 'py' if windows() else sys.executable
    script_name = 'pypackage_test.py'

    # Find wheel tag by running ourselves with --tag inside a venv running <py>:
    log(f'Finding wheel tag for {py}')
    text = venv_run(
            f'python {sys.argv[0]} --tag',
            return_output=True,
            venv='pypackage-venv-test',
            py=py,
            clean=True,
            pip_upgrade=False,  # Saves a little time.
            )
    m = re.search('tag: (.+)', text)
    assert m, f'Failed to find expected tag: ... in output text: {text!r}'
    tag = m.group(1)
    assert not '\r' in tag, f'tag is: {tag!r}'
    log(f'tag is: {tag!r}')

    # Run <code> after installing matching wheel or specified package.
    #
    if isinstance(wheels_or_package_name, str):
        package_name = wheels_or_package_name
        pip_install_arg = f'{"--repository testpypi " if pypi_test else ""}{package_name}'
    else:
        pattern = f'^([^-]+)-([^-]+)-{tag}[.]whl$'
        log(f'Looking for wheel matching {pattern!r}')
        for wheel in wheels_or_package_name:
            base = os.path.basename(wheel)
            m = re.match(pattern, base)
            if m:
                log( f'Matching wheel: {wheel}')
                package_name = m.group(1)
                pip_install_arg = wheel
                break
        else:
            text = f'Cannot find wheel matching: {pattern!r}\n'
            text += f'Wheels are ({len(wheels_or_package_name)}):\n'
            for wheel in wheels_or_package_name:
                text += f'    {wheel}\n'
            raise Exception(text)

    if not code:
        code = (
                f'import {package_name}\n'
                f'print("Successfully imported {package_name}")\n'
                )
        log('Testing default code for package_name={package_name}:\n{code}')
    with open(script_name, 'w') as f:
        f.write(code)
    venv_run(
            [
            f'pip install {pip_install_arg}',
            f'{py} {script_name}',
            ],
            venv='pypackage-venv-test',
            py=py,
            clean=True,
            )


def parse_remote(remote):
    '''
    remote:
        [<user>@]<host>:[<directory>]

    Returns <user>@, <host>, <directory>.

    directory will have '/' appended if
    not already present.

    user: if not '', will end with '@'.
    host>: does not end with ':'.
    directory: if not '', will end with '/'. Does not start with ':'.
    '''
    m = re.match('^(([^@]+@))?([^:]+):(.*)$', remote)
    assert m, f'Expected [<user>@]<hostname>:[<directory>] but: {remote!r}'
    user, host, directory = m.group(1), m.group(3), m.group(4)
    if user is None:
        user = ''
    if directory is None:
        directory = ''
    if directory and not directory.endswith('/'):
        directory += '/'
    return user, host, directory


def main():

    sdist = None
    wheels = []
    pypi_test = True
    abis = None
    outdir = 'pypackage-out'
    if windows():
        abis = ['x32-38', 'x32-39', 'x64-38', 'x64-39']
    else:
        abis = ['37', '38', '39']
        manylinux_container_name = None
        manylinux_install_docker = False
        manylinux_docker_image = None
        manylinux_pull_docker_image = None

    args = iter(sys.argv[1:])
    while 1:
        try:
            arg = next(args)
        except StopIteration:
            break
        log(f'Handling arg={arg!r}')

        if 0:
            pass

        elif arg == '--abis':
            abis = next(args).split(',')

        elif arg == '--build-sdist':
            package_directory = next(args)
            sdist = make_sdist(package_directory, outdir)

        elif arg == '--build-wheels':
            if not sdist:
                raise Exception(f'Need to specify --build-sdist or --sdist=... before {arg}.')
            if windows():
                wheels = make_windows(sdist, abis, outdir)

            else:
                wheels = make_unix(
                        sdist,
                        abis,
                        outdir,
                        test_direct_install = False,
                        install_docker = manylinux_install_docker,
                        docker_image = manylinux_docker_image,
                        pull_docker_image = manylinux_pull_docker_image,
                        container_name = manylinux_container_name,
                        )
            log(f'sdist: {sdist}')
            for wheel in wheels:
                log(f'    wheel: {wheel}')

        elif arg == '--build-wheels-remote':
            sync_sdist = True
            while 1:
                a = next(args)
                if not a.startswith('-'):
                    break
                if a == '-s':
                    sync_sdist = int(next(args))
                else:
                    raise Exception('Unrecognised arg: {a!r}')
            user, host, directory = parse_remote(a)
            if sync_sdist:
                if not sdist:
                    raise Exception(f'No sdist specified, specify --build-sdist before {arg}.')
            rsync_remote = f'{user}{host}{directory}'
            local_dir = os.path.dirname(__file__)
            command = (''
                    f'rsync -aP {local_dir}/pypackage.py {local_dir}/jlib.py {sdist if sync_sdist else ""} {user}{host}:{directory}'
                    f' && echo rsync finished'
                    f' && ssh {user}{host} '
                    f'"'
                    f'{"cd "+directory if directory else "true"}'
                    f' && ./pypackage.py --sdist {os.path.basename(sdist)} --build-wheels'
                    f'"'
                    f' && rsync -ai {user}{host}:{directory}pypackage-out/ {outdir}/'
                    )
            system(command, prefix=f'{user}{host}:{directory}: ')

        elif arg == '--clean':
            assert 'pypackage' in outdir
            for leaf in os.listdir(outdir):
                if leaf.endswith('.whl') or leaf.endswith('.tar.gz'):
                    os.remove(os.path.join(outdir, leaf))

        elif arg in ('-h', '--help'):
            print( __doc__)

        elif unix() and arg == '--manylinux-c':
            manylinux_container_name = next(args)

        elif unix() and arg == '--manylinux-d':
            manylinux_install_docker = int(next(args))

        elif unix() and arg == '--manylinux-i':
            manylinux_docker_image = next(args)

        elif unix() and arg == '--manylinux-p':
            manylinux_pull_docker_image = next(args)

        elif arg == '-o':
            outdir = next(args)

        elif arg == '--pypi-test':
            pypi_test = int(next(args))

        elif arg == '--remote-args':
            user, host, directory = parse_remote(next(args))
            remote_args = next(args)
            local_dir = os.path.dirname(__file__)
            command = (
                    f'rsync -aP {local_dir}/pypackage.py {local_dir}/jlib.py {user}{host}:{directory}'
                    f' && ssh {user}{host} '
                    f'"'
                    f'{"cd "+directory if directory else "true"}'
                    f' && ./pypackage.py {remote_args}'
                    f'"'
                    )
            system(command, prefix=f'{user}{host}:{directory}: ')

        elif arg == '--sdist':
            sdist = next(args)

        elif arg == '--tag':
            print(f'tag: {make_tag()}')

        elif arg == '--test':
            package_name = None
            while 1:
                a = next(args)
                if a.startswith('-'):
                    if a == '-i':
                        package_name = next(args)
                    else:
                        raise Exception(f'Unrecognised arg: {a}')
                else:
                    code = a
                    code = code.replace('\\n', '\n')
                    break
            if package_name:
                package_name_or_wheels = package_name
            else:
                if not wheels:
                    raise Exception(f'No wheels specified, so cannot test. E.g. use --build-wheels or --wheels <pattern>.')
                package_name_or_wheels = wheels
            if code:
                log(f'Code to run is:\n{code}')
            else:
                log(f'Code to run is empty so will default to import of inferred package name')
            test(code, package_name if package_name else wheels, pypi_test)

        elif arg in '--upload':
            venv_run([
                    f'pip install twine',
                    f'python -m twine upload {"--repository testpypi" if pypi_test else ""} {sdist} {" ".join(wheels)}',
                    ])

        elif arg == '--wheels':
            pattern = next(args)
            wheels_raw = glob.glob(pattern)
            if not wheels_raw:
                log(f'Warning: no matches found for --wheels pattern {pattern!r}.')
            wheels = []
            for wheel in wheels_raw:
                if wheel.endswith('.whl'):
                    wheels.append(wheel)
            log(f'Found {len(wheels)} wheels with pattern {pattern}:')
            for wheel in wheels:
                log(f'    {wheel}')

        else:
            raise Exception(f'Unrecognised arg: {arg}')


if __name__ == '__main__':
    main()
