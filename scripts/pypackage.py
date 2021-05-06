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
    wheels --sdist=...' wheels will be built without the --sdist option.

    Args:

        --build
            Build wheels.

            If --sdist has not been specified earlier, we build an sdist in the
            current tree.

            We extract the sdist (locally on Windows, in a container on Unix)
            and build wheels in the extracted tree.

            todo: provide an option to use 'pip wheel <sdist>'. This would
            force clean build each time so can be slow when experimenting, but
            is probably more correct.


        --test [-w <wheel>] <code>
            Tests wheel by running <code> inside a clean Pyhton venv.

            We install wheel using 'pip install'. If <wheel> is not specified
            we search the list of created wheels for one that matches the
            default python.

        -h
        --help
            Show this help.

        --abis <abis>
            Set the list of ABIs for which we build wheels.

            abis
                Comma-separated list of <abi>
                On Windows:
                    abi: <cpu>-<python-version>
                        cpu:
                            'x32' or 'x64'
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

        --sdist <sdist>
            Specify path of existing sdist. If specified, wheels are built by
            extracting the sdist (either locally on Windows or inside docker
            container on Unix) and building inside this resulting tree.

            Otherwise we build a new sdist as required by running './setup.py
            sdist' locally.

        --wheels <pattern>
            An alternative to building wheels. <pattern> should be a glob that
            matches previously-build wheels. The resulting list of wheels is
            used by later occurrencies of --test, upload-- etc.

        Unix only:
            --manylinux-c <container-name>
                Name of container to create/start/use.

            --manylinux-d 0 | 1
                Whether to ensure that docker is installed.

            --manylinux-i <docker-image>
                Set docker image; default is
                'quay.io/pypa/manylinux2014_x86_64'.

            --manylinux-p 0 | 1
                Whether to use 'docker pull' to get/update container image.

Docker notes:
    Interactive session inside docker instance:
        docker exec -it pypackage bash
'''

import glob
import os
import platform
import re
import shutil
import subprocess
import sys
import tarfile
import time


def log(text=''):
    '''
    Logs lines with prefix.
    '''
    for line in text.split('\n'):
        print(f'pypackage.py: {line}')
    sys.stdout.flush()


def system(command, error_raise=True, return_output=False):
    log(f'Running: {command}')
    if return_output:
        log(f'Running with subprocess.run(): {command}')
        ret = subprocess.run(
                command,
                shell=True,
                universal_newlines=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                )
        if error_raise:
            ret.check_returncode()
        return ret.stdout
    e = os.system(command)
    if error_raise and e:
        raise Exception(f'Command failed ({e}): {command}')
    return e


def windows():
    s = platform.system()
    return s == 'Windows' or s.startswith('CYGWIN')

def unix():
    return not windows()


venv_installed = set()

def venv_run(commands, venv='pypackage-venv', py=None, clean=False, return_output=False):
    '''
    Runs commands inside Pyton venv, joined by &&.
    commands:
        List of commands to run.
    venv:
        Name of venv to create and use.
    py:
        Python to run. If None we use sys.executable.
    clean:
        If true we first delete any existing <venv>. We assert that<venv>
        starts with 'pypackage-venv'.
    '''
    if isinstance(commands, str):
        commands = [commands]
    if py is None:
        py = sys.executable
    if windows():
        pre = [
                f'cmd.exe /c "true',
                f'{py} -m venv {venv}',
                f'{venv}\\Scripts\\activate.bat',
                ]
        post = [f'deactivate"']
    else:
        pre = [
                f'{py} -m venv {venv}',
                f'. {venv}/bin/activate',
                ]
        post = [f'deactivate']

    if clean or venv not in venv_installed:
        if clean:
            assert venv.startswith('pypackage-venv')
            shutil.rmtree(venv, ignore_errors=1)
        venv_installed.add(venv)

        # It looks like first attempt to upgrade pip can fail with
        # 'EnvironmentError: [WinError 5] Access is denied'. So we run twice.
        #
        pre += [
                'pip install --upgrade pip',
                'pip install --upgrade twine',
                ]
        if windows:
            commands0 = pre + post
            command0 = '&&'.join(commands0)
            log(f'Running pre: {command0}')
            system(command0, error_raise=False)

    commands = pre + commands + post

    if windows():
        command = '&&'.join(commands)
    else:
        command = ' && '.join(commands)

    return system(command, return_output=return_output)


def check_sdist(sdist):
    '''
    Checks sdist with 'twine check'.
    '''
    venv_run(f'twine check {sdist}')


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


def make_sdist(out_directory):
    '''
    Creates sdist by running ./setup.py. Returns path of the created sdist.
    '''
    os.makedirs(out_directory, exist_ok=True)
    t = time.time()
    command = f'./setup.py sdist -d "{out_directory}"'
    system(command)

    # Find new file in <sdist_directory>.
    sdist = find_new_file(f'{out_directory}/*.tar.gz', t)
    check_sdist(sdist)

    return sdist


def make_unix(
        abis,
        sdist,
        test_direct_install=False,
        install_docker=None,
        docker_image=None,
        pull_docker_image=None,
        container_name=None,
        ):
    '''
    Builds Python wheels using a manylinux container.

        python_versions
            List of string python versions for which we build
            wheels. Any '.' are removed and we then take the first two
            characters. E.g. ['3.8.4', '39'] is same as ['38', '39'].
        sdist:
            Path of sdist file to copy into container.
            If None we build a new sdist inside the container.
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

    Returns (sdist, wheels).

    '''
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

    sdist_directory = 'pypackage-dist'
    io_directory = 'pypackage-io'

    if not sdist:
        # Create new sdist.
        sdist = make_sdist(sdist_directory)

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
            error_raise=False,
            )
        log(f'docker run: e={e}')

        # Start docker instance if not already started.
        #
        e = system(f'docker start {container_name}', error_raise=False,)
        log(f'docker start: e={e}')

    def docker_system(command, return_output=False):
        '''
        Runs specified command inside container. Runs via bash so <command> can
        contain shell constructs. If out is 'capture', we return the output text.
        '''
        #prefix = f'docker: {prefix}'
        command = command.replace('"', '\\"')
        command = f'docker exec {container_name} bash -c "{command}"'
        log(f'Running: {command}')
        # Set universal_newlines=True so we get text output, not bytes.
        if return_output:
            ret = subprocess.run(command, shell=True, universal_newlines=True, stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT)
            ret.check_returncode()
            return ret.stdout
        else:
            e = os.system(command)
            if e:
                raise Exception(f'Docker command failed: {command}')

    if test_direct_install:
        # Test direct intall from sdist.
        #
        docker_system( f'pip3 -vvv install /io/{sdist_leaf}')
        #docker_system( f'/io/mupdfwrap_test.py /io/Python2.pdf')

    container_pythons = []
    for abi in abis:
        vv = abi.replace('.', '')[:2] # E.g. '38' for Python-3.8.
        pattern = f'ls -d /opt/python/cp{vv}-cp{vv}*'
        paths = docker_system(pattern, return_output=1)
        log(f'vv={vv}: paths={paths!r}')
        paths = paths.strip().split('\n')
        assert len(paths) == 1, f'No single match for glob pattern in container {pattern}: {paths}'
        container_pythons.append( (vv, paths[0]))

    # In the container, extract <sdist>; we will use the extracted sdist to
    # build wheels.
    #
    docker_system( f'tar -xzf /io/{sdist_leaf}')

    # Build wheels.
    wheels = []
    for vv, container_python in container_pythons:
        log(f'Building wheel with python {vv}: {container_python}')

        # Build wheel.
        t = time.time()
        docker_system(f'cd {package_root} && {container_python}/bin/python ./setup.py --dist-dir /io bdist_wheel')

        # Find new wheel.
        wheel = find_new_file(f'{io_directory}/{package_root}-py{vv}-none-*.whl', t)
        wheels.append(wheel)

    log( f'wheels are ({len(wheels)}):')
    for wheel in wheels:
        log( f'    {wheel}')

    return sdist, wheels


def make_windows(
        abis,
        sdist=None,
        ):
    '''
    Builds Python wheels on Windows.

    abis:
        List of <cpu>-<pythonversion> strings.
            cpu:
                'x32' or 'x64'
            pythonversion:
                String version number, may contain '.' characters, e.g. '3.8'
                or '39'.
    sdist:
        If not None, should be path of sdist; we extract it and generate wheels
        inside the extracted tree. Note that this means that running a second
        time with the same sdist will not be a clean build.

    Returns (sdist, wheels).
    '''
    assert windows()
    subdir = None
    if sdist:
        #assert 0, 'Building Windows wheels from sdist not currently supported'
        # Extact sdist and run ourselves inside the extracted tree.
        with tarfile.open(sdist, 'r:gz') as t:
            items = t.getnames()
            assert items
            item = items[0]
            log(f'item={item}')
            s = item.find('/')
            assert s >= 0
            prefix = item[:s+1]
            for item in items:
                assert item.startswith(prefix)
            t.extractall()
            subdir = prefix
    else:
        sdist_directory = 'pypackage-sdist'
        make_sdist(sdist_directory)

    wheel_dir = 'pypackage-wheels'
    os.makedirs(wheel_dir, exist_ok=True)
    wheel_dir2 = f'../{wheel_dir}' if subdir else wheel_dir

    wheels = []
    for abi in abis:
        cpu, python_version = abi.split('-')
        python_version = '.'.join(python_version)
        assert cpu in ('x32', 'x64')
        py = f'py -{python_version}-{cpu[1:]}'
        cd_command = f'cd {subdir}' if subdir else 'true'
        t = time.time()
        venv_run(
                [
                f'pip install clang check-wheel-contents twine',
                f'{cd_command}',
                f'python setup.py -d {wheel_dir2} bdist_wheel',
                ],
                py=py,
                )
        wheel = find_new_file(f'{wheel_dir}/*.whl', t)
        wheels.append(wheel)

    return sdist, wheels


def test(wheels, code, py=None):
    '''
    Basic testing of wheels.

    Finds matching wheel in <wheels>, installs in temporary venv, and runs
    python <code>.

    wheels:
        List of wheel paths.
    code:
        Python code to run. E.g. 'import foo\nfoo.qwert()\n'
    py:
        Python executable. If None we use sys.executable.
    '''
    #python_version = ''.join(platform.python_version().split('.')[:2])
    # Find what Python version is being run by.
    if py is None:
        py = 'py' if windows() else sys.executable
    script_name = 'pypackage_test.py'

    # Find python version and cpu whene we run <python>
    with open(script_name, 'w') as f:
        f.write('import platform, sys\n')
        f.write('print(f"version={platform.python_version()} 64={sys.maxsize==2**63-1}")\n')
    text = venv_run(
            f'python {script_name}',
            return_output=True,
            venv='pypackage-venv-test',
            py=py,
            clean=True,
            )
    log(f'text=\n{text}')
    m = re.search('version=([0-9.]+) 64=((True)|(False))', text)
    assert m, f'No match in text={text!r}'
    python_version = ''.join(m.group(1).split('.')[:2])
    cpu = '64' if m.group(2) == 'True' else '32'

    with open(script_name, 'w') as f:
        f.write(code)
    log(f'Looking for wheel matching cpu={cpu} python_version={python_version}')
    for wheel in wheels:
        base = os.path.basename(wheel)
        m = re.match( f'^.*-py{python_version}-none-.*{cpu}[.]whl$', base)
        log( f'base={base} python_version={python_version} m={m}')
        if m:
            log( f'Testing wheel: {wheel}')
            venv_run(
                    [
                        f'pip install {wheel}',
                        f'{py} {script_name}',
                    ],
                    venv='pypackage-venv-test',
                    py=py,
                    clean=True,
                    )

def main():

    sdist = None
    wheels = []
    pypi = None

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

        if arg in ('-h', '--help'):
            print( __doc__)

        elif arg == '--abis':
            abis = next(args).split(',')

        elif arg == '--pypi':
            pypi = next(args)

        elif arg == '--sdist':
            sdist = next(args)

        elif unix() and arg == '--manylinux-c':
            manylinux_container_name = next(args)

        elif unix() and arg == '--manylinux-d':
            manylinux_install_docker = int(next(args))

        elif unix() and arg == '--manylinux-i':
            manylinux_docker_image = next(args)

        elif unix() and arg == '--manylinux-p':
            manylinux_pull_docker_image = next(args)

        elif arg == '--build':
            if windows():
                sdist, wheels = make_windows(abis, sdist)

            else:
                sdist, wheels = make_unix(
                        abis,
                        sdist,
                        test_direct_install = False,
                        install_docker = manylinux_install_docker,
                        docker_image = manylinux_docker_image,
                        pull_docker_image = manylinux_pull_docker_image,
                        container_name = manylinux_container_name,
                        )
            log(f'sdist: {sdist}')
            for wheel in wheels:
                log(f'    wheel: {wheel}')

        elif arg == '--test':
            test_wheels = wheels
            while 1:
                a = next(args)
                if a.startswith('-'):
                    if a == '-w':
                        test_wheels = [next(args)]
                    else:
                        raise Exception(f'Unrecognised arg: {a}')
                else:
                    code = a
                    code = code.replace('\\n', '\n')
                    break
            log(f'Code to run is:\n{code}')
            test(test_wheels, code)

        elif arg == '--upload':
            venv = ensure_venv()
            if windows():
                command = f'python -m twine upload {sdist} {" ".join(wheels)}'
                venv_run(command)

        elif arg == '--wheels':
            pattern = next(args)
            wheels = glob.glob(pattern)
            if not wheels:
                log(f'Warning: no matches found for --wheels pattern {pattern!r}.')

        else:
            raise Exception(f'Unrecognised arg: {arg}')


if __name__ == '__main__':
    main()
