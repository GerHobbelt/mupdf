#!/usr/bin/env python3

'''
Support for creating Python wheels and uploading to pypi.org.

Overview:

    We expect to be run in the directory containing a package's setup.py and
    pyproject.toml.

    On Unix we use a manylinux docker container.

    On Windows we use the 'py' command to find different installed Pythons.


Use as a Python module:

    Use these functions:

        make_unix()
        make_windows()


Command line usage:

    Args:

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
            Specify path of existing sdist. If not specified we build a new sdist
            locally by running './setup.py sdist'. The sdist is copied into the
            container where it is used to build the wheels.

            Not currently supported on Windows.

        Unix only:
            --manylinux-c <container-name>
                Name of container to create/start/use.

            --manylinux-d 0 | 1
                Whether to ensure that docker is installed.

            --manylinux-i <docker-image>
                Set docker image; default is 'quay.io/pypa/manylinux2014_x86_64'.

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
import subprocess
import sys
import time


def log(text=''):
    '''
    Logs lines with prefix.
    '''
    for line in text.split('\n'):
        print(f'pypackage.py: {line}')
    sys.stdout.flush()


def system(command, error_raise=True):
    log(f'Running: {command}')
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

def venv_run(commands, venv='pypackage-venv'):
    '''
    Runs commands inside Pyton venv, joined by &&.
    commands:
        List of commands.
    '''
    if isinstance(commands, str):
        commands = [commands]

    if windows():
        pre = [
                f'cmd.exe /c "true',
                f'py -m venv {venv}',
                f'{venv}\\Scripts\\activate.bat',
                ]
        post = [f'deactivate"']
    else:
        pre = [
                f'python3 -m venv {venv}',
                f'. {venv}/bin/activate',
                ]
        post = [f'deactivate']

    if venv not in venv_installed:
        venv_installed.add(venv)
        pre += ['pip install --upgrade pip twine']

    commands = pre + commands + post

    if windows():
        command = '&&'.join(commands)
    else:
        command = ' && '.join(commands)

    system(command)


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
            List of
        sdist:
            If None we build sdist inside container. Otherwise should be path
            of sdist file to copy into the container.
        test_direct_install
            If true we run 'pip install <sdist>' in the container.
        install_docker
            If true we attempt to install docker (currently specific to
            Debian/Devuan).
        docker_image
            If not None, the name of docker image to use. Default is
            quay.io/pypa/manylinux2014_x86_64.
        pull_docker_image
            If true we run 'docker pull ...' to update the image.
        container_name
            Name to use for the container.

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

    # Extract sdist into directory shared with container.
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
        If not None, should be path of sdist. (Not currently supported.)

    Returns (sdist, wheels).
    '''
    assert windows()
    if sdist:
        assert 0, 'Building Windows wheels from sdist not currently supported'

    sdist_directory = 'pypackage-sdist'
    make_sdist(sdist_directory)

    wheel_dir = 'pypackage-wheels'
    os.makedirs(wheel_dir, exist_ok=True)

    wheels = []
    for abi in abis:
        cpu, python_version = abi.split('-')
        assert cpu in ('x32', 'x64')
        py = f'py -{python_version}-{cpu[1:]}'
        t = time.time()
        venv_run([
                f'pip install clang check-wheel-contents twine',
                f'{py} setup.py -d {wheel_dir} bdist_wheel',
                ])
        wheel = find_new_file(f'{wheel_dir}/*.whl')
        wheels.append(wheel)

    return sdist, wheels


def main():

    s = platform.system()

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

        elif arg == 'build-wheels':
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

        elif arg == 'upload':
            venv = ensure_venv()
            if windows():
                command = f'python -m twine upload {sdist} {" ".join(wheels)}'
                venv_run(command)

        else:
            raise Exception(f'Unrecognised arg: {arg}')




if __name__ == '__main__':
    main()
