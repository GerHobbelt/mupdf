#!/usr/bin/env python3

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
        jlib.remove('python_manylinux_demo-dist')
        os.mkdir('python_manylinux_demo-dist')
        jlib.system('./setup.py sdist -d python_manylinux_demo-dist')
    sdist = glob.glob('python_manylinux_demo-dist/*')
    assert len(sdist) == 1
    sdist = sdist[0]

    # need to add user to docker group, e.g. with:
    #   sudo usermod -a -G docker $USER
    system('sudo apt install docker.io')
    python_manylinux_demo = 'python-manylinux-demo'
    if not os.path.exists(python_manylinux_demo):
        jlib.system(f'git clone https://github.com/pypa/python-manylinux-demo.git {python_manylinux_demo}')
    else:
        jlib.system(f'cd {python_manylinux_demo} and git pull -r')
    #command = ''
    #command += ' DOCKER_IMAGE=quay.io/pypa/manylinux1_x86_64'
    #command += ' PLAT=manylinux1_x86_64'

    if 0:
        # Don't delete in case docker instance already running and we want to
        # reuse it.
        #
        jlib.system('sudo rm -rf python-manylinux-demo-io')
    os.makedirs('python-manylinux-demo-io', exist_ok=True)
    os.system('sudo rm -rf python-manylinux-demo-io/*')

    jlib.system(f'rsync -ai {sdist} python-manylinux-demo-io/')

    # Looks like python-clang isn't available on manylinux2014_x86_64 so we
    # manually copy it into /io/:
    #
    jlib.system(f'rsync -ai /usr/lib/python2.7/dist-packages/clang python-manylinux-demo-io/')

    # This doesn't seem to be able to run bash:
    docker_image = 'quay.io/pypa/manylinux1_x86_64'

    # This can run bash.
    docker_image = 'quay.io/pypa/manylinux2014_x86_64'

    system(f'docker pull {docker_image}')

    script = 'python-manylinux-demo-io/build-wheels.sh'
    with open(script, 'w') as f:
        f.write(
                f'#!/bin/bash\n'
                f'ls -l /io/\n'
                f'yum --assumeyes install python3-devel\n'

                # for manylinux2014_x86_64.
                #f'yum --assumeyes install llvm-toolset-7.0-clang-libs\n'
                #f'yum --assumeyes install clang\n'

                # but no python-clang
                #f'git clone https://github.com/llvm-mirror/clang.git'

                # 'yum install pip' fails to find pip, in quay.io/pypa/manylinux2014_x86_64.
                # f'yum install pip\n'
                # f'pip wheel /io/{os.path.basename(sdist)}\n'

                f'true'
                    #' && (rm -r mupdf || true)'
                    ' && mkdir -p mupdf'
                    ' && cd mupdf'
                    ' && tar -xzf /io/*.tar.gz'
                    #' && PYTHONPATH=/io'
                    #    ' MUPDFWRAP_CLANG_SO=/opt/rh/llvm-toolset-7.0/root/usr/lib64/libclang.so'
                    #    ' MUPDFWRAP_CLANG_INCLUDE=/opt/rh/llvm-toolset-7.0/root/usr/local/include'
                    #    ' LD_LIBRARY_PATH=/opt/rh/llvm-toolset-7.0/root/usr/lib64'
                    #    ' ./setup.py --dist-dir /io bdist_wheel'
                    f' && MUPDF_SETUP_HAVE_CLANG_PYTHON=0 MUPDF_SETUP_HAVE_SWIG=0'
                        ' ./setup.py --dist-dir /io bdist_wheel'
                )
    os.chmod(script, 0o755)

    # With '-v HOST:CONTAINER', HOST must be absolute path.
    # "... -it sh" to get interactive?
    # docker ps -a
    # docker rm ID
    # docker run -it -d --name python-manylinux-demo IMAGE bash
    # docker exec

    if 1:
        # Interactive with:
        #   docker exec -it python-manylinux bash
        #

        # Create and start container.
        e = os.system(
            f' docker run'
            f' -t'
            f' --name python-manylinux'
            f' -e PLAT=manylinux1_x86_64'
            f' -v {os.path.abspath("python-manylinux-demo-io")}:/io'
            f' {docker_image}'
            )
        log(f'docker run: e={e}')

        # Start container.
        e = os.system('docker start python-manylinux')
        log(f'docker start: e={e}')

        # Run command in container.
        e = os.system(
                f'docker exec'
                #f' -e PLAT=manylinux1_x86_64'
                f' python-manylinux'
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
                f' -v {os.path.abspath("python-manylinux-demo-io")}:/io'
                f' {docker_image} /io/{os.path.basename(script)}'
                )
        system(command)

if __name__ == '__main__':
    make_manylinux()
