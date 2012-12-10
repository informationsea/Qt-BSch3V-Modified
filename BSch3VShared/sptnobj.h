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

#ifndef SPTOBJ_H_
#define SPTOBJ_H_

#include "ce3io.h"
#include "coord.h"

enum VECTPTN {
    VECTPTN_L   = 1,
    VECTPTN_C   = 2,
    VECTPTN_PG  = 3,
    VECTPTN_AR  = 4,
    VECTPTN_TXT = 5
};

class SPtnObj
{
public:
    int m_style;
    int m_width;
    int m_nFill;
public:
    SPtnObj(void);
    virtual ~SPtnObj(void);
public:
    virtual  int id() {
        return 0;
    }
    //複製
    virtual SPtnObj* duplicate() = 0;
    //有効なデータかどうかを返す。
    virtual bool qValid() {
        return true;
    }
    //位置を動かす
    virtual void move(int offsetx,int offsety) = 0;
    //グリップの数を返す
    virtual int gripCount() = 0;
    //グリップの位置を返す
    virtual SPoint gripPos(int grip) = 0;
    //グリップの位置を動かす
    virtual void setGripPos(int grip,int x,int y) = 0;


    //X座標を指定して左右反転を行う
    virtual void mirrorH(int x) = 0;
    //左右反転を行う
    virtual void mirrorH() = 0;
    //XY中心座標を指定して、回転を行う
    virtual void rotate(const SPoint& p) = 0;
    //回転を行う
    virtual void rotate() = 0;
    //範囲を指定して選択チェックを行う
    virtual bool testIntersect(const SRect& ) {
        return false;
    }
    //点を指定して選択チェックを行う
    virtual bool testIntersect(const SPoint& ) {
        return false;
    }
    //占有範囲を返す
    virtual SRect area() = 0;

    virtual bool readCe3(SReadCE3& rce3) = 0;
    virtual bool writeCe3(SWriteCE3& wce3,SPoint* pOrigin=NULL) = 0;

public:
    //XY中心座標を指定して、回転を行う
    static SPoint rotatePoint(SPoint pt,SPoint pCenter);
    //X座標を指定して左右反転を行う
    static SPoint mirrorPoint(SPoint pt,int xCenter);
    //p0とp1を端点座標とする線とpの重なりをテスト
    static bool testIntersectLinePoint(const SPoint& p0,const SPoint& p1,const SPoint& p);

};

#endif

/*
  Local Variables:
  mode: c++
  End:
 */
