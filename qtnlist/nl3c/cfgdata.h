/****************************************************************************
    QtNlist   Netlist & Partslist utility for Qt-BSch3V
    Copyright (C) 2004-2005 H.Okada

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*****************************************************************************/
#ifndef CFGDATA_H
#define CFGDATA_H

//#include <qlist.h>
//#include <string.h>
#include <stdio.h>
#include <list>
#include <string>
using namespace std;

//設定データの入出力を行う
class SCfgDataElm{
public:
	string group;
	string	key;
	string	data;
	SCfgDataElm(){}
	~SCfgDataElm(){}
};	


typedef list<SCfgDataElm*> SCfgList;
typedef list<SCfgDataElm*>::iterator SCfgListIte;

class SCfgData{
protected:
	SCfgList listCfgData;
	void clear();
	SCfgDataElm* findExisting(const string& sGroup,const string& sKey);

public:
	SCfgData();
	~SCfgData();
	bool load(const char* filename);
	bool save(const char* filename);
	void setString(const string& sGroup,const string& sKey,const string& sData);
	void setInt(const string& sGroup,const string& sKey,int data);
	bool getString(const string& sGroup,const string& sKey,string& sData);
	bool getInt(const string& sGroup,const string& sKey,int& data);
	//void dump();
};
#endif
