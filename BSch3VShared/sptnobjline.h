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

#ifndef SPTNONJLINE_H
#define SPTNONJLINE_H


#include "sptnobj.h"

class SPtnObjLine :    public SPtnObj
{
public:
    //共通のインターフェース
    SPtnObjLine(void);
    ~SPtnObjLine(void);
    int id() {
        return VECTPTN_L;
    }
    //    SPtnObjLine(const SPtnObjLine&);                //コピーコンストラクタ
    //    SPtnObjLine& operator=(const SPtnObjLine&);        //代入演算子
    SPtnObj* duplicate();                                //複製を作る
    bool qValid();                                    //有効なデータかどうかを返す。

    //グリップの数を返す
    int gripCount() {
        return 2;
    }
    //グリップの位置を返す
    SPoint gripPos(int grip);
    //グリップの位置を動かす
    void setGripPos(int grip,int x,int y);



    void move(int offsetx,int offsety);                //位置を動かす
    void mirrorH(int x);                             //X座標を指定して左右反転を行う
    void mirrorH();                                    //左右反転を行う
    void rotate(const SPoint& p);                    //XY中心座標を指定して、回転を行う
    void rotate();                                    //回転を行う
    bool testIntersect(const SRect& );                //範囲を指定して選択チェックを行う
    bool testIntersect(const SPoint& );                //点を指定して選択チェックを行う
    SRect area();                                    //占有範囲を返す
    bool readCe3(SReadCE3& rce3);
    bool writeCe3(SWriteCE3& wce3,SPoint* pOrigin=NULL);
public:
    SPoint m_p0;
    SPoint m_p1;
};

#endif


/*
  Local Variables:
  mode: c++
  End:
 */
