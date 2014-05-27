#!/bin/sh

if type qmake > /dev/null 2>&1; then
    qmake -v
else
    echo "qmake is not found."
    exit 1
fi

BUILDDIR=build-`uname`
#rm -r $BUILDDIR
#mkdir -p $BUILDDIR
#
pushd $BUILDDIR
#qmake ../bsch3v.pro
#make -j5
#
#for i in qtbsch3v qtlcov; do
#    macdeployqt $i/$i.app
#    cp ../$i/Info_mac.plist $i/$i.app/Contents/Info.plist
#done
#
#cp ../qtbsch3v/circuit.icns qtbsch3v/qtbsch3v.app/Contents/Resources/
#cp ../qtlcov/library.icns qtlcov/qtlcov.app/Contents/Resources/

mkdir -p dist
pushd dist

cp -R ../qtbsch3v/qtbsch3v.app .
cp -R ../qtlcov/qtlcov.app .
cp -R ../../Documents/* .
popd
popd

git archive --format=tgz --output=$BUILDDIR/dist/src.tar.gz HEAD



