#-------------------------------------------------
#
# Project created by QtCreator 2011-08-02T14:05:53
#
#-------------------------------------------------

QT = core gui
TARGET = BSch3VShared
TEMPLATE = lib
CONFIG += staticlib

VERSION = 0.45

DEFINES += BSCH3VSHARED_LIBRARY _CRT_SECURE_NO_WARNINGS

SOURCES += bsch3vshared.cpp \
    sheetmessagebox.cpp \
    ce3io.cpp \
    cfgdata.cpp \
    coord.cpp \
    sptnobjtext.cpp \
    sptnobjpolygon.cpp \
    sptnobjline.cpp \
    sptnobjcircle.cpp \
    sptnobjarc.cpp \
    sptnobj.cpp \
    xbschpattern.cpp \
    xbschfont.cpp \
    drawcomponentbase.cpp \
    complib.cpp

HEADERS += bsch3vshared.h\
        BSch3VShared_global.h \
    sheetmessagebox.h \
    ce3io.h \
    cfgdata.h \
    coord.h \
    sptnobjtext.h \
    sptnobjpolygon.h \
    sptnobjline.h \
    sptnobjcircle.h \
    sptnobjarc.h \
    sptnobj.h \
    stdafx.h \
    xbschpattern.h \
    xbschfont.h \
    drawcomponentbase.h \
    xbsch.h \
    complib.h
