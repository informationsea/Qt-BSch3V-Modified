set DevEnvDir=c:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\
set Framework35Version=v3.5
set FrameworkDir=C:\Windows\Microsoft.NET\Framework\
set FrameworkDIR32=C:\Windows\Microsoft.NET\Framework\
set FrameworkVersion=v4.0.30319
set FrameworkVersion32=v4.0.30319
set INCLUDE=c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\INCLUDE;C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\include;
set LIB=c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\LIB;C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\lib;
set LIBPATH=C:\Windows\Microsoft.NET\Framework\v4.0.30319;C:\Windows\Microsoft.NET\Framework\v3.5;c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\LIB;
set Path=C:\Qt\4.7.4-VS2010\bin;c:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\;c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\BIN;c:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\Tools;C:\Windows\Microsoft.NET\Framework\v4.0.30319;C:\Windows\Microsoft.NET\Framework\v3.5;c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\VCPackages;C:\Program Files (x86)\HTML Help Workshop;C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\bin\NETFX 4.0 Tools;C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\bin;%PATH%
set VCINSTALLDIR=c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\
set WindowsSdkDir=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\
cls
@echo Qt 4.7.4 with VS 2010


cd ..
del /S /Q BSch3VShared-windows-build
del /S /Q qtbsch3v-windows-build
del /S /Q qtlcov-windows-build
mkdir BSch3VShared-windows-build
mkdir qtbsch3v-windows-build
mkdir qtlcov-windows-build

pushd BSch3VShared-windows-build
qmake ..\BSch3VShared CONFIG+=release
nmake
popd

pushd qtbsch3v-windows-build
qmake ..\qtbsch3v CONFIG+=release
nmake
popd

pushd qtlcov-windows-build
qmake ..\qtlcov CONFIG+=release
nmake
popd

pause
