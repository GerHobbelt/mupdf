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


test_cpp = '''
#include <iostream>

#include "platform/c++/include/mupdf/classes.h"

static void show_stext(mupdf::Document& document)
{
    for (int p=0; p<document.count_pages(); ++p)
    {
        mupdf::Page page(document.load_page(p);
        StextOptions    options;
        StextPage   stextpage(page, options);
        for (mupdf::StextBlock stextblock: stextpage)
        {
            for (mupdf::StextLine stextline: stextblock)
            {
                for (mupdf::StextChar stextchar: stextline)
                {
                    std::cout << "char:"
                            << " " << stextchar.m_internal->c
                            << " " << (int) stextchar.m_internal->c
                            << " " << stextchar.m_internal->color
                            << " " << mupdf::Point(stextchar.m_internal->origin)
                            << " " << mupdf::Quad(stextchar.m_internal->quad)
                            << " " << stextchar.m_internal->size
                            << "\n";
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    for (int i=1; i<argv; ++i)
    {
        mupdf::Document document(argv[i]);
        show_stext(document);
    }
}
'''


def make_manylinux():

    # This affects all builds we do later on.
    #
    os.environ['MUPDF_SETUP_BUILD_DIR'] = 'build/shared-debug'

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
    jlib.system(f'rsync -ai scripts/mupdfwrap_test.py python-docker-io/')
    jlib.system(f'rsync -ai thirdparty/extract/test/Python2.pdf python-docker-io/')

    if 1:
        with open('python-docker-io/test.cpp', 'w') as f:
            f.write(test_cpp)

    # Ensure we have the docker image available.
    #
    # (Note that older docker image 'manylinux1_x86_64' doesn't seem to be able
    # to run bash.)
    #
    docker_image = 'quay.io/pypa/manylinux2014_x86_64'
    system(f'docker pull {docker_image}')

    # Create a script to run inside the docker instance, which will use pip to
    # install from our sdist file.
    #
    # We ensure that python3-devel is installed. We don't attempt to
    # install swig because swig-4.0.2 seems to be already part of
    # quay.io/pypa/manylinux2014_x86_64, and the version installed by yum is
    # old - swig-2.0.10.
    #
    script = 'python-docker-io/pypackage-install.sh'
    with open(script, 'w') as f:
        f.write(
                f'#!/bin/bash\n'
                f'set -e\n' # Exit if any command fails.
                )
        if os.environ.get('MUPDF_SETUP_BUILD_DIR'):
            f.write(f'export MUPDF_SETUP_BUILD_DIR={os.environ["MUPDF_SETUP_BUILD_DIR"]}\n')
        f.write(
                f'echo MUPDF_SETUP_BUILD_DIR=$MUPDF_SETUP_BUILD_DIR\n'
                f'yum --assumeyes install python3-devel\n'
                f'pip3 -vvv install /io/*.tar.gz\n'
                f'echo running mupdfwrap_test.py on Python2.pdf\n'
                f'/io/mupdfwrap_test.py /io/Python2.pdf\n'
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
        # Ensure docker instance is created and running. Also give it a name so
        # we can refer to it below.
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

        # Start docker instance if not already started.
        #
        e = os.system('docker start mupdf-docker')
        log(f'docker start: e={e}')

        # Run our script inside the docker instance.
        #
        jlib.system(
                f'docker exec'
                    f' mupdf-docker'
                    f' /io/{os.path.basename(script)}'
                ,
                prefix='docker: ',
                verbose=1,
                )

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
