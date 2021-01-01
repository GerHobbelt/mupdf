#! /bin/bash
#
# set up all submodules: 
# - init/update them
# - checkout to local tracking master branch
# - run the checkout-to-registered-branch script to switch to the appropriate branch where necessary
# 

cd "$(dirname "$0")"
cd ..
BASEDIR=$( pwd )

echo "git submodule update --init --recursive"
git submodule update --init --recursive

# git submodule foreach aborts at the first error; we don't want that
# so we use it to get the directories of the submodules only 
# and take it from there.
for f in $( git submodule foreach --quiet --recursive pwd ) ; do
	echo "SUBMODULE DIR: $f"
	cd "$f"

	if ! git checkout master ; then
		echo "Creating local tracking 'master' branch..."
		git checkout --track origin/master
	fi
done

cd "$BASEDIR"

echo "git submodule sync --recursive"
git submodule sync --recursive

echo "submodules: checkout to appropriate branch for development"
bash util/checkout_to_known_git_branches_recursive.sh -c
