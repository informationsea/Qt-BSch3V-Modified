/*******************************************************************************
 *   This file is part of Qt-BSch3V Modified.
 *   Copyright (C) 2002-2005 H.Okada
 *   Copyright (C) 2010-2011 Stork Lab
 *
 *   Qt-BSch3V Modified is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Qt-BSch3V Modified is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <list>
using namespace std;
#include "cfgdata.h"

#include <QApplication>
#include <QDebug>


//#define CSVBUFFLEN 1024

SCfgData g_cfg;

SCfgData::SCfgData():settings(0)
{
}

SCfgData::~SCfgData()
{

}

void SCfgData::clear()
{
    settings->clear();
}

//設定データのロード
bool SCfgData::load(const char* /*filename*/)
{
    if(!settings) {
#ifdef Q_WS_WIN // Windowsではレジストリを使いたくないのでINIファイルを使う
        settings = new QSettings(QSettings::IniFormat,QSettings::UserScope,qApp->organizationName(),qApp->applicationName());
#else
        settings = new QSettings;
#endif
    }
    settings->sync();
    return settings->isWritable();
}

//設定データのファイルへの保存
bool SCfgData::save(const char* /*filename*/)
{
    settings->sync();
    return settings->isWritable();
}

//groupとkeyを指定して、文字列dataを設定する
void SCfgData::setString(const string& sGroup,const string& sKey,const string& sData)
{
    //qDebug() << __func__  << QString::fromStdString(sGroup) << QString::fromStdString(sKey) << QString::fromStdString(sData);
    if( sGroup == "" || sKey == "") return;
    settings->beginGroup(QString::fromStdString(sGroup));
    settings->setValue(QString::fromStdString(sKey),QString::fromStdString(sData));
    settings->endGroup();
}

//groupとkeyを指定して、整数値dataを設定する
//整数値を文字列に変換してからsetStringを呼んでいる
void SCfgData::setInt(const string& sGroup,const string& sKey,int data)
{
    //qDebug() << __func__  << QString::fromStdString(sGroup) << QString::fromStdString(sKey) << data;
    if( sGroup == "" || sKey == "") return;
    settings->beginGroup(QString::fromStdString(sGroup));
    settings->setValue(QString::fromStdString(sKey),data);
    settings->endGroup();
}

bool SCfgData::getString(const string& sGroup,const string& sKey,string& sData)
{
    //qDebug() << __func__ << QString::fromStdString(sGroup) << QString::fromStdString(sKey) << QString::fromStdString(sData) << "OK";
    if( sGroup == "" || sKey == "" ) return false;
    settings->beginGroup(QString::fromStdString(sGroup));
    //bool retvalue = settings.contains(QString::fromStdString(sKey));
    sData = settings->value(QString::fromStdString(sKey),"").toString().toStdString();
    settings->endGroup();
    return sData != "";
}

bool SCfgData::getInt(const string& sGroup,const string& sKey,int& data)
{
    //qDebug() << "getInt" << QString::fromStdString(sGroup) << QString::fromStdString(sKey) << data;
    if( sGroup == "" || sKey == "" ) return false;
    settings->beginGroup(QString::fromStdString(sGroup));
    bool retvalue = settings->contains(QString::fromStdString(sKey));
    data = settings->value(QString::fromStdString(sKey),0).toInt();
    settings->endGroup();
    return retvalue;
}
