#!/usr/bin/env python3

'''
Script for creating a wheel inside a docker instance.
'''

import glob
import os
import sys

import jlib

def log(text=''):
    '''
    Logs lines with prefix.
    '''
    for line in text.split('\n'):
        print(f'pypackage.py: {line}')
    sys.stdout.flush()

def system(command):
    print(f'running: {command}')
    sys.stdout.flush()
    e = os.system(command)
    assert e == 0

def make_manylinux():
    if 1:
        # Create new sdist.
        jlib.ensure_empty_dir('docker-dist')
        jlib.system('./setup.py sdist -d python-docker-dist', prefix='creating sdist: ')
    sdist = glob.glob('python-docker-dist/*')
    assert len(sdist) == 1
    sdist = sdist[0]

    # Note that if docker is not already installed, we will
    # need to add user to docker group, e.g. with:
    #   sudo usermod -a -G docker $USER
    system('sudo apt install docker.io')

    jlib.ensure_empty_dir('python-docker-io')

    # Copy sdist into (what will be) the docker's /io/ directory.
    #
    jlib.system(f'rsync -ai {sdist} python-docker-io/')

    # Ensure we have the docker image available.
    #
    # (Note that older docker image 'manylinux1_x86_64' doesn't seem to be able
    # to run bash.)
    #
    docker_image = 'quay.io/pypa/manylinux2014_x86_64'
    system(f'docker pull {docker_image}')

    # Create a script to run inside the docker instance.
    #
    script = 'python-docker-io/build-wheels.sh'
    with open(script, 'w') as f:
        f.write(
                f'#!/bin/bash\n'
                f'ls -l /io/\n'
                f'yum --assumeyes install python3-devel\n'
                f'true'
                    ' && (rm -r mupdf || true)'
                    ' && mkdir -p mupdf'
                    ' && cd mupdf'
                    ' && tar -xzf /io/*.tar.gz'
                    ' &&'
                        ' MUPDF_SETUP_HAVE_CLANG_PYTHON=0'
                        #' MUPDF_SETUP_HAVE_SWIG=0'
                        ' ./setup.py --dist-dir /io bdist_wheel'
                )
    os.chmod(script, 0o755)

    # Docker notes:
    #
    # With docker run's '-v HOST:CONTAINER' option, HOST must be absolute path.
    #
    # Interactive session inside docker instance:
    #   docker exec -it mupdf-docker bash

    # Run our script inside the docker instance.
    #

    if 1:
        # Ensure container is created and running. Also give it a name so we
        # can refer to it below.
        #
        # '-itd' ensures docker instance runs in background.
        #
        e = os.system(
            f' docker run'
            f' -itd'
            f' --name mupdf-docker'
            f' -v {os.path.abspath("python-docker-io")}:/io'
            f' {docker_image}'
            )
        log(f'docker run: e={e}')

        # Start container if not already started.
        #
        e = os.system('docker start mupdf-docker')
        log(f'docker start: e={e}')

        # Run our script inside the container.
        #
        e = os.system(
                f'docker exec'
                f' mupdf-docker'
                f' /io/{os.path.basename(script)}'
                )
        log(f'docker start: e={e}')
        assert e == 0

    else:
        command = (
                f' docker container run'
                f' -t'
                #f' --rm'
                f' --name python-manylinux'
                f' -e PLAT=manylinux1_x86_64'
                f' -v {os.path.abspath("python-manylinux-io")}:/io'
                f' {docker_image} /io/{os.path.basename(script)}'
                )
        system(command)

if __name__ == '__main__':
    make_manylinux()
