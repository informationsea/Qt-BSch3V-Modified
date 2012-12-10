name=`find *.pro | sed -e 's/\.pro//'` 

echo "Building... ${name}"
qmake ${name}.pro -spec macx-g++ -r CONFIG+=debug
make -j3
if [ -r Info_mac.plist ] ; then
    cp Info_mac.plist ${name}.app/Contents/Info.plist
fi
cp *.icns ${name}.app/Contents/Resources/
