#!/usr/bin/env python3

'''
Script for creating a wheel inside a docker instance.

Args:

    -m [<options>] <items>
        Do a manylinux build using manylinux docker container.

        options:
            -s <sdist>
                Specify existing sdist.
        items:
            s   Build sdist on local machine (not docker container).
            d   Ensure docker is installed.
            i   Use 'docker pull' to get/update container image.
            b   Build wheels inside docker container.
            t   Test that compatible wheel installs and runs on local machine.

'''

import glob
import os
import platform
import re
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


def make_manylinux( items, sdist=None):

    sdist_directory = 'python-docker-dist'
    io_directory = 'python-docker-io'

    if 's' in items:
        # Create new sdist.
        jlib.ensure_empty_dir(sdist_directory)
        jlib.system(f'./setup.py sdist -d {sdist_directory}', prefix='creating sdist: ', verbose=1)

    if not sdist:
        sdist = glob.glob(f'{sdist_directory}/*')
        assert len(sdist) == 1
        sdist = sdist[0]

    assert sdist.endswith('.tar.gz')
    package_root = os.path.basename( sdist[ : -len('.tar.gz')])

    if 'd' in items:
        # Note that if docker is not already installed, we will
        # need to add user to docker group, e.g. with:
        #   sudo usermod -a -G docker $USER
        system('sudo apt install docker.io')

    os.makedirs( io_directory, exist_ok=True)

    # Copy sdist into (what will be) the docker's /io/ directory.
    #
    jlib.system( f'rsync -ai {sdist} {io_directory}/')
    jlib.system( f'rsync -ai scripts/mupdfwrap_test.py {io_directory}/')
    jlib.system( f'rsync -ai thirdparty/extract/test/Python2.pdf {io_directory}/')

    if 1:
        with open( f'{io_directory}/test.cpp', 'w') as f:
            f.write(test_cpp)

    docker_image = 'quay.io/pypa/manylinux2014_x86_64'
    if 'i' in items:
        # Ensure we have the docker image available.
        #
        # (Note that older docker image 'manylinux1_x86_64' doesn't seem to be able
        # to run bash.)
        #
        system(f'docker pull {docker_image}')

    # Docker notes:
    #
    # With docker run's '-v HOST:CONTAINER' option, HOST must be absolute path.
    #
    # Interactive session inside docker instance:
    #   docker exec -it mupdf-docker bash
    #

    # Run our script inside the docker instance.
    #

    if 1:
        # Ensure docker instance is created and running. Also give it a name so
        # we can refer to it below.
        #
        # '-itd' ensures docker instance runs in background.
        #
        # It's ok for this to fail - container already created.
        #
        e = jlib.system(
            f' docker run'
            f' -itd'
            f' --name mupdf-docker'
            f' -v {os.path.abspath(io_directory)}:/io'
            f' {docker_image}'
            ,
            prefix='docker run: ',
            verbose=1,
            raise_errors=0,
            )
        log(f'docker run: e={e}')

        # Start docker instance if not already started.
        #
        e = jlib.system(
                'docker start mupdf-docker',
                prefix='docker start: ',
                verbose=1,
                raise_errors=0,
                )
        log(f'docker start: e={e}')

    def docker_system(command, prefix=''):
        '''
        Runs specified command inside container. Runs via bash so <command> can
        contain shell constructs.
        '''
        prefix = f'docker: {prefix}'
        command = command.replace('"', '\\"')
        jlib.system( f'docker exec mupdf-docker bash -c "{command}"', verbose=1, prefix=prefix, out='log')

    if 0 and 't' in items:
        # Test direct intall from sdist.
        #
        docker_system( f'pip3 -vvv install /io/{os.path.basename(sdist)}')
        docker_system( f'/io/mupdfwrap_test.py /io/Python2.pdf')

    wheels = []
    if 'w' in items:
        # Build wheels.
        #
        sdist_leaf = os.path.basename(sdist)
        assert sdist_leaf.endswith('.tar.gz')
        sdist_prefix = sdist_leaf[ : -len('.tar.gz')]
        docker_system( f'tar -xzf /io/{sdist_leaf}')
        for p in 'cp37-cp37m', 'cp38-cp38', 'cp39-cp39':
            docker_system(
                    f'cd {sdist_prefix} && /opt/python/{p}/bin/python ./setup.py --dist-dir /io bdist_wheel',
                    f'wheel py{p}: ',
                    )
    for i in os.listdir( io_directory):
        m = re.match( f'^{package_root}-py([0-9][0-9])-none-(.*)[.]whl$', i)
        if m:
            wheels.append( os.path.join( io_directory, i))

    log( f'wheels are ({len(wheels)}):')
    for w in wheels:
        log( f'    {w}')

    if 't' in items:
        # Test wheel can be installed into local python.
        python_version = ''.join(platform.python_version().split('.')[:2])
        for wheel in wheels:
            base = os.path.basename(wheel)
            m = re.match( f'^{package_root}-py{python_version}-none-.*[.]whl$', base)
            log( f'base={base} package_root={package_root} python_version={python_version} m={m}')
            if m:
                log( f'Testing wheel: {wheel}')
                jlib.system( f'true'
                        + f' && (rm -r pylocal-wheel-test || true)'
                        + f' && python3 -m venv pylocal-wheel-test'
                        + f' && . pylocal-wheel-test/bin/activate'
                        + f' && python -m pip install --upgrade pip'
                        + f' && pip install {wheel}'
                        + f' && python3 scripts/mupdfwrap_test.py'
                        + f' && deactivate'
                        ,
                        prefix='wheel test: ',
                        verbose=1,
                        out='log',
                        )



def main():
    sdist = None
    args = jlib.Args(sys.argv[1:])
    while 1:
        arg = args.next_or_none()
        if arg is None:
            break
        if arg == '-m':
            while 1:
                a = args.next()
                if a.startswith('-'):
                    if a == '-s':
                        sdist = args.next()
                    else:
                        raise Exception(f'unrecognised -m option: {a}')
                else:
                    items = a
                    break
            make_manylinux(items, sdist)
        elif arg == '-s':
            sdist = args.next()
        else:
            raise Exception(f'Unrecognised arg: {arg}')


if __name__ == '__main__':
    main()
