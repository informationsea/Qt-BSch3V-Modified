#!/usr/bin/env bash

if [ $# -ne 2 ]; then
    echo "Please set project name and type."
    exit 1
fi

# export PATH=${HOME}/QtSDK/Desktop/Qt/473/gcc/bin:$PATH
if [ -d /usr/local/Trolltech/Qt-4.8.3-sql/bin ];then
    export PATH=/usr/local/Trolltech/Qt-4.8.3-sql/bin:$PATH
fi


name=$1
src=$PWD/$1
build=$PWD/$1-build-desktop
dist=$PWD/dist
apps=$PWD/dist


if [ "$2" == "clean" ];then
    rm -r $build
    exit
fi

export COPYFILE_DISABLE=true

mkdir -p $dist
mkdir -p $apps

rm -r $build
mkdir -p $build

echo 'Building...'

if [ -d /Volumes ];then
    spec="-spec macx-g++"
else
    spec=
fi

pushd $build
qmake $src/$1.pro $spec -config release BUILD_VERSION=${VERSION}
echo qmake $src/$1.pro $spec -config release BUILD_VERSION=${VERSION}
make -j5 > log$1.txt

if [ $2 == 'app' ];then

    if [ -r $src/Info_mac.plist ] ; then
	cp $src/Info_mac.plist ${name}.app/Contents/Info.plist
	cp $src/*.icns ${name}.app/Contents/Resources
    fi

    echo 'Deploying...'

    macdeployqt ${name}.app 

    cp -r ${name}.app ${apps}/
elif [ $2 == 'exe' ];then
    if [ -f ${name}.exe ];then
        cp ${name}.exe ${apps}/
    else
        cp ${name} ${apps}/
    fi
fi
popd



