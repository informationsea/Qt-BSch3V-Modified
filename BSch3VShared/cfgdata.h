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

#ifndef CFGDATA_H
#define CFGDATA_H

//#include <qlist.h>
//#include <string.h>
#include <stdio.h>
#include <list>
#include <string>

#include <QString>
#include <QSettings>

using namespace std;

class SCfgData
{
private:
    QSettings *settings;

protected:
    void clear();

public:
    SCfgData();
    ~SCfgData();
    bool load(const char* filename);
    bool save(const char* filename);

    void setString(const string& sGroup,const string& sKey,const string& sData);
    void setInt(const string& sGroup,const string& sKey,int data);
    bool getString(const string& sGroup,const string& sKey,string& sData);
    bool getInt(const string& sGroup,const string& sKey,int& data);
    QSettings *getSettings() {
        return settings;
    }
    //void dump();
};

extern  SCfgData g_cfg;
#endif
