/****************************************************************************
    Qt-LCoV Library Editor for Qt-BSch3V
    Copyright (C) 2002-2005 H.Okada

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

#ifndef _COMPEDITDOC_H
#define _COMPEDITDOC_H
//#pragma once

#include <list>
#include <string>
using namespace std;

#include "complib.h"
#include "compeditelm.h"

typedef list<SCompEditElm> SCompEditElmList;
typedef list<SCompEditElm>::iterator SCompEditElmListIterator;
typedef list<SCompEditElm>::reverse_iterator SCompEditElmListRevIterator;

class SCompEditDoc
{
public:
    enum {
        DATATYPE_PIN = 0,
        DATATYPE_PTN = 1
    };
protected:
    int m_nOpCount;

public:
    SCompEditElmList m_listMain;
    SCompEditElmList m_listTemp;
    bool m_dirtyFlag;
    bool m_tempDirtyFlag;
    SSize m_size;        //グリッド単位でのサイズ
    int  m_block;
    SRect m_tempArea;

    //string m_prop_string;


    void setPin(const PSPin* pPinArray, int count);
    void setPtnObj(const PSPtnObj* pPtnArray, int count);
    bool releaseTemp();
    bool delTemp();
    void setupTempArea();
    bool isOnTmpData(const SPoint& pt);
    //int isOnGrip(const SPoint& pt);
    int incrementOpCount() {
        m_nOpCount++;
        return m_nOpCount;
    }
    int opCount() {
        return m_nOpCount;
    }

    bool addPinToTemp(SPin* pPin);//一時データへのピンの追加
    bool addPtnToTemp(SPtnObj* pPtn);//一時データへのパターンの追加

public:
    SCompEditDoc(void);
    virtual ~SCompEditDoc(void);
    void init();
    void loadCompInfo(const SCompInfo* pComp);
    void saveCompInfo(SCompInfo* pComp);

    int doSelect(const SPoint& pt);
    int doSelect(const SRect& rc);

    bool copy();
    bool canUndo();
    bool undo();

    bool move(int offsetx,int offsety);
    bool mirror();
    bool rotate();


    void pinPlacePoint(const SPoint& pt,int &ltrb,int &offset);    //ptに最も近いピン配置座標を得る
    SRect pinArea(int ltrb,int offset);//ピン座標から有効な占有範囲を求める
    SPin* pinConflict(const SPin* pPin);//既存ピンと同じ場所なら、既存ピンのポインタを返す
    SPoint pinPosition(int ltrb,int offset); //ピン座標から部品編集座標を得る
};

#endif
