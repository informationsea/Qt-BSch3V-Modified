#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Please set version."
    exit 1
fi

version=$1
dist=$PWD/dist
apps=$PWD/dist
current=$PWD

export VERSION=$1

if [ "$1" == "clean" ];then
    rm -r dist
    ./build.sh BSch3VShared clean
    ./build.sh qtbsch3v clean
    ./build.sh qtlcov clean
    ./build.sh qtnlist clean
    ./build.sh qtnut clean
    exit
fi

if [ -d $dist ] ; then
    rm -r $dist
fi
if [ -d $apps ] ; then
    rm -r $apps
fi

if [ -d /Volumes ];then
    TYPE=app
else
    TYPE=exe
fi

./build.sh BSch3VShared lib
./build.sh qtbsch3v $TYPE
./build.sh qtlcov $TYPE
./build.sh qtnlist $TYPE
./build.sh qtnut $TYPE

echo "Copying Documents..."
cp -r Documents/* $dist
cp README.md $dist

git archive --format zip HEAD > dist/src.zip

#/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker -d installer.pmdoc -o dist/Installer.pkg

echo "Compressing..."
cd $dist
tar -cjf $current/bsch3vm-${version}.tar.bz2 *
