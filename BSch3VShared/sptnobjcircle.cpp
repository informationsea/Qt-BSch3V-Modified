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

#include "stdafx.h"
#include "sptnobjcircle.h"
#include <math.h>
#include <stdlib.h>

SPtnObjCircle::SPtnObjCircle(void)
{
    m_p0 = SPoint(-1,-1);
    m_p1 = SPoint(-1,-1);
    //m_nFill    = -1;
}

SPtnObjCircle::~SPtnObjCircle(void)
{
}

//複製を作る
SPtnObj* SPtnObjCircle::duplicate()
{
    return new SPtnObjCircle(*this);
}


//有効なデータかどうかを返す。
bool SPtnObjCircle::qValid()
{
    if(m_p0 == m_p1) return false;
    else return true;
}


//位置を動かす
void SPtnObjCircle::move(int offsetx,int offsety)
{
    m_p0.setX(m_p0.x()+offsetx);
    m_p0.setY(m_p0.y()+offsety);
    m_p1.setX(m_p1.x()+offsetx);
    m_p1.setY(m_p1.y()+offsety);
}


//X座標を指定して左右反転を行う
void SPtnObjCircle::mirrorH(int x)
{
    m_p0 = mirrorPoint(m_p0,x);
    m_p1 = mirrorPoint(m_p1,x);
}

//左右反転を行う
void SPtnObjCircle::mirrorH()
{
    return;
}


//XY中心座標を指定して、回転を行う
void SPtnObjCircle::rotate(const SPoint& p)
{
    m_p0 = rotatePoint(m_p0,p);
    m_p1 = rotatePoint(m_p1,p);
}



//回転を行う
void SPtnObjCircle::rotate()
{
    int x = (m_p0.x()+m_p1.x())/2;
    int y = (m_p0.y()+m_p1.y())/2;
    SPoint p(x,y);
    rotate(p);
}

//範囲を指定して選択チェックを行う
bool SPtnObjCircle::testIntersect(const SRect& rc)
{
    if(!rc.intersect(m_p0)) return false;
    if(!rc.intersect(m_p1)) return false;
    return true;
}


//点を指定して選択チェックを行う
bool SPtnObjCircle::testIntersect(const SPoint& p)
{
    int l,t,r,b;
    l = p.x()-2;
    r = p.x()+2;
    t = p.y()-2;
    b = p.y()+2;


    double xc,yc,k,r2,x0,y0,a0,a1;
    xc=((double)(m_p0.x()+m_p1.x()))/2;
    yc=((double)(m_p0.y()+m_p1.y()))/2;
    //    k = abs((double)(m_p0.x()-m_p1.x())/(double)(m_p0.y()-m_p1.y()));
    k = fabs((double)(m_p0.x()-m_p1.x())/(double)(m_p0.y()-m_p1.y()));
    r2=(((double)(m_p0.y()-m_p1.y()))/2);
    r2 = r2*r2;

    if(m_nFill < 0) {
        x0=(p.x()-xc)/k;
        y0 = t-yc;
        a0 = x0*x0+y0*y0;
        y0 = b-yc;
        a1 = x0*x0+y0*y0;
        if((a0 < r2)&&(r2 < a1)) return true;
        if((a0 > r2)&&(r2 > a1)) return true;

        x0=(l-xc)/k;
        y0 = p.y()-yc;
        a0 = x0*x0+y0*y0;
        x0=(r-xc)/k;
        a1 = x0*x0+y0*y0;
        if((a0 < r2)&&(r2 < a1)) return true;
        if((a0 > r2)&&(r2 > a1)) return true;
    } else {
        x0=(p.x()-xc)/k;
        y0 = p.y()-yc;
        a0 = x0*x0+y0*y0;
        if(a0 <= r2) return true;
    }
    return false;
}


//占有範囲を返す
SRect SPtnObjCircle::area()
{
    SRect rc(m_p0,m_p1);
    rc.normalize();
    return rc;
}

//グリップの位置を返す
SPoint SPtnObjCircle::gripPos(int grip)
{
    if(grip == 0) return m_p0;
    else    	return m_p1;
}

//グリップの位置を動かす
void SPtnObjCircle::setGripPos(int grip,int x,int y)
{
    if(grip == 0) m_p0 = SPoint(x,y);
    else    	m_p1 = SPoint(x,y);
}


bool SPtnObjCircle::readCe3(SReadCE3& rce3)
{
    bool f_y1 = false;
    int i;

    int width = 1;
    int style = 0;
    int fill  = -1;
    int x0    = 0;
    int y0    = 0;
    int x1    = 0;
    int y1    = 0;

    string str("");
    while(rce3.ReadRecord(str) != EOF) {
        if(str[0] == '-') {
            if(str != "-C") {
                return false;
            } else {
                break;
            }
        } else if(str[0] == '+') {
            str[0]='-';
            if(rce3.SkipTo(str) == EOF) return false;
        } else {
            int n;
            int l = str.length();
            for(n = 0; n < l; n++) {
                if(str[n] == ':') break;
            }
            if(0 < n && n<(l-1)) {    // : で分けられたレコードである
                string var = str.substr(0,n);    	    	//先頭から:の手前まで
                //int nParam = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                if(var == "W") {
                    width = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                } else if(var == "S") {
                    style = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                } else if(var == "F") {
                    fill = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                } else if(var == "X") {
                    i = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                    if(!f_y1) {
                        x0 = i;
                    } else {
                        x1 = i;
                    }
                } else if(var == "Y") {
                    i = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                    if(!f_y1) {
                        y0 = i;
                        f_y1 = true;
                    } else {
                        y1 = i;
                    }
                }
            }
        }
    }
    m_width = width;
    m_style = style;
    m_nFill = fill;
    m_p0 = SPoint(x0,y0);
    m_p1 = SPoint(x1,y1);
    return true;
}

bool SPtnObjCircle::writeCe3(SWriteCE3& wce3,SPoint* pOrigin)
{
    SPoint p0 = m_p0;
    SPoint p1 = m_p1;

    if(pOrigin != NULL) {
        p0 -= *pOrigin;
        p1 -= *pOrigin;
    }
    wce3.WriteRecord("+C");
    wce3.WriteRecordInt("W",m_width);
    wce3.WriteRecordInt("S",m_style);
    wce3.WriteRecordInt("F",m_nFill);
    wce3.WriteRecordInt("X",p0.x());
    wce3.WriteRecordInt("Y",p0.y());
    wce3.WriteRecordInt("X",p1.x());
    wce3.WriteRecordInt("Y",p1.y());
    wce3.WriteRecord("-C");
    wce3.WriteEOL();
    return true;
}
