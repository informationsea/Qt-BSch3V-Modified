/****************************************************************************
    NUT3W Numbering software for BSch3V
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

#pragma once
#ifdef _MSVC
#pragma warning (disable: 4786)
#endif

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string>
#include <cstring>
#include <algorithm>
#include <list>
using namespace std;
#include "bsch3com/xbschstringlist.h"
#include "ce3io.h"
#include "bsch3com/xbschobj.h"
#include "bsch3com/xbschjunc.h"
#include "bsch3com/xbschlabel.h"
#include "bsch3com/xbschcomment.h"
#include "bsch3com/xbschtag.h"
#include "bsch3com/xbschcomponent.h"

#include "bsch3com/xbschline.h"
#include "bsch3com/xbschwirenl.h"
#include "bsch3com/xbschentry.h"
#include "bsch3com/xbschdelobj.h"
#include "bsch3com/xbschsheetinfo.h"
#include "bsch3com/xbschdoc.h"

#define MAX_SCH_FILES 64 

class SNutDataFile
{
public:
    string m_filename;
    SXBSchDoc* m_pDoc;
    SNutDataFile(){m_pDoc=NULL;}
    ~SNutDataFile();
};

class SNutComponent
{
public:
    int m_index;
    SXBSchComponent* m_pObj;
    bool m_set;
    bool m_powComp;
    string m_prefix;
    int m_suffix;
    SNutComponent();
    void loadRef();
    void saveRef();
    //bool operator<(const SNutComponent &a)const;
};


typedef list<SNutComponent*> NutCompList;
typedef list<SNutComponent*>::iterator NutCompListIterator;
typedef SNutComponent* PSNutComponent;

class SNut
{
    int m_dataFileCount;
    SNutDataFile m_schFiles[MAX_SCH_FILES];

    PSNutComponent* m_compArray;
    int m_compArrayCount;
    //PSNutComponent* m_powcompArray;
    //int m_powcompArrayCount;
    void pickupComponent(bool renumMode);
    void clearCompArray();
    void clearSchFiles();
    void sortPOS();
    void sortREF();
    //void createBackup();

public:
    SNut();
    ~SNut();
    int fileCount(){return m_dataFileCount;}
    int readFile(const char* path);	//読み込みに成功したら0以外を返す
    int saveFiles();
    int anum(int init,int step);
    int renum(int init,int step);

};
