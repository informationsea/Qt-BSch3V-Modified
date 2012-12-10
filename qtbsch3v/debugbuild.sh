name=`find *.pro | sed -e 's/\.pro//'` 

echo "Building... ${name}"
if [ -d "/Application" ];then
	qmake ${name}.pro -spec macx-g++ -r CONFIG+=debug
else
	qmake -r COFNIG+=debug
fi
make -j3
if [ -r Info_mac.plist ] ; then
    cp Info_mac.plist ${name}.app/Contents/Info.plist
fi
cp *.icns ${name}.app/Contents/Resources/
