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
#include "sptnobjarc.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

SPtnObjArc::SPtnObjArc(void)
{
    m_ptC = SPoint(10,10);    	//中心座標
    m_nR  = 10;    	    	    //半径
    m_nBegin = 0;    	    	//開始角
    m_nEnd    = 360*16;    	    //終了角
    m_ptBegin = AngleToPoint500(m_nBegin);
    m_ptEnd   = AngleToPoint500(m_nEnd);
}

SPtnObjArc::~SPtnObjArc(void)
{
}

//複製を作る
SPtnObj* SPtnObjArc::duplicate()
{
    return (SPtnObj*)new SPtnObjArc(*this);
}

//有効なデータかどうかを返す。
bool SPtnObjArc::qValid()
{
    if(m_nR <= 0) return false;
    else return true;
}

//位置を動かす
void SPtnObjArc::move(int offsetx,int offsety)
{
    m_ptC.setX(m_ptC.x()+offsetx);
    m_ptC.setY(m_ptC.y()+offsety);
}


//X座標を指定して左右反転を行う
void SPtnObjArc::mirrorH(int x)
{
    normalizeAngle();
    m_ptC = mirrorPoint(m_ptC,x);
    int b,e;
    if(m_nBegin <= 180*16) {
        e = 180*16 - m_nBegin;
    } else {
        e=(360+180)*16 - m_nBegin;
    }
    if(m_nEnd <= 180*16) {
        b = 180*16 - m_nEnd;
    } else {
        b=(360+180)*16 - m_nEnd;
    }
    m_nBegin = b;
    m_nEnd = e;
    m_ptBegin = AngleToPoint500(m_nBegin);
    m_ptEnd   = AngleToPoint500(m_nEnd);
}

//左右反転を行う
void SPtnObjArc::mirrorH()
{
    SRect rc = area();
    int x = (rc.l()+rc.r())/2;
    mirrorH(x);
}

//XY中心座標を指定して、回転を行う
void SPtnObjArc::rotate(const SPoint& p)
{
    m_ptC = rotatePoint(m_ptC,p);
    m_nBegin += 270*16;
    m_nEnd += 270*16;
    normalizeAngle();
    m_ptBegin = AngleToPoint500(m_nBegin);
    m_ptEnd   = AngleToPoint500(m_nEnd);
}


//回転を行う
void SPtnObjArc::rotate()
{
    SRect rc = area();
    SPoint p = rc.center();
    rotate(p);
}

//範囲を指定して選択チェックを行う
bool SPtnObjArc::testIntersect(const SRect& rc)
{
    SRect rc1 = area();
    if(!rc.intersect( SPoint(rc1.l(),rc1.t()) ) ) return false;
    if(!rc.intersect( SPoint(rc1.r(),rc1.b()) ) ) return false;
    return true;
}

//点を指定して選択チェックを行う
bool SPtnObjArc::testIntersect(const SPoint& ptest)
{
    double dx = ptest.x() - m_ptC.x();
    double dy = ptest.y() - m_ptC.y();
    double r = sqrt(dx*dx + dy*dy);
    double e = fabs(r-m_nR);
    //    double e = abs(r-m_nR);


    if(e > 1) return false;

    double rad = PointRad(m_ptC.x(),m_ptC.y(),r,ptest.x(),ptest.y());
    int deg = (180.0*16*rad)/M_PI+0.5;
    int nEnd;
    normalizeAngle();
    if(m_nBegin > m_nEnd) {
        nEnd = m_nEnd+360*16;
        deg += 360*16;
    } else {
        nEnd = m_nEnd;
    }
    if(m_nBegin <= deg && deg <= m_nEnd) return true;
    else return false;
}

//占有範囲を返す
SRect SPtnObjArc::area()
{
    int l,t,r,b;
    normalizeAngle();
    if(m_nBegin == m_nEnd) {
        l = m_ptC.x()-m_nR;
        t = m_ptC.y()-m_nR;
        r = m_ptC.x()+m_nR;
        b = m_ptC.y()+m_nR;
    } else {
        int xb,yb,xe,ye,nEnd,n,m;
        SPoint pt;
        pt = AngleToPoint(m_nBegin);
        xb = pt.x();
        yb = pt.y();
        pt = AngleToPoint(m_nEnd);
        xe = pt.x();
        ye = pt.y();

        //double angle;
        //angle = m_nBegin*M_PI/(180*16);
        //xb =m_ptC.x() + m_nR*cos(angle);
        //yb =m_ptC.y() - m_nR*sin(angle);
        //angle = m_nEnd*M_PI/(180*16);
        //xe =m_ptC.x() + m_nR*cos(angle);
        //ye =m_ptC.y() - m_nR*sin(angle);

        l=(xb < xe ? xb : xe);
        r=(xb > xe ? xb : xe);
        t=(yb < ye ? yb : ye);
        b=(yb > ye ? yb : ye);


        //始点、終点が0度をまたいでいるかどうか
        if(m_nBegin > m_nEnd) {    //またいでいる場合
            r = m_ptC.x()+m_nR;
            //以降の判定のための準備
            nEnd = m_nEnd+360*16;
        } else {
            //t=(xb > xe ? xb : xe);
            //以降の判定のための準備
            nEnd = m_nEnd;
        }

        n = 90*16;
        m=(90+360)*16;
        //始点、終点が90度をまたいでいるかどうか
        if((m_nBegin <= n && nEnd >= n)||(m_nBegin <= m && nEnd >= m)) {    //またいでいる場合
            t = m_ptC.y()-m_nR;
        }

        //始点、終点が180度をまたいでいるかどうか
        n = 180*16;
        m=(180+360)*16;
        if((m_nBegin <= n && nEnd >= n)||(m_nBegin <= m && nEnd >= m)) {    //またいでいる場合
            l = m_ptC.x()-m_nR;
        }

        //始点、終点が270度をまたいでいるかどうか
        n = 270*16;
        m=(270+360)*16;
        if((m_nBegin <= n && nEnd >= n)||(m_nBegin <= m && nEnd >= m)) {    //またいでいる場合
            b = m_ptC.y()+m_nR;
        }
    }
    return SRect(l,t,r-l,b-t);
}


void SPtnObjArc::normalizeAngle()
{
    m_nBegin = m_nBegin %5760;
    m_nEnd     = m_nEnd   %5760;
}

double SPtnObjArc::PointRad(double cx,double cy,double r,double x0,double y0)
{
    double x = (x0-cx)/r;
    double y = -(y0-cy);
    double rad = acos(x);
    if(y < 0) rad = 2*M_PI-rad;
    return rad;
}

//開始角を座標で設定。先に中心を決めておくこと
void SPtnObjArc::setBeginPoint(int x,int y)
{
    double dx = x - m_ptC.x();
    double dy = y - m_ptC.y();
    double r = sqrt(dx*dx + dy*dy);

    double rad = PointRad(m_ptC.x(),m_ptC.y(),r,x,y);
    m_nBegin = (180.0*16*rad)/M_PI+0.5;
    m_ptBegin = AngleToPoint500(m_nBegin);
}

//終了角を座標で設定。先に中心を決めておくこと
void SPtnObjArc::setEndPoint(int x,int y)
{
    double dx = x - m_ptC.x();
    double dy = y - m_ptC.y();
    double r = sqrt(dx*dx + dy*dy);

    double rad = PointRad(m_ptC.x(),m_ptC.y(),r,x,y);
    m_nEnd = (180.0*16*rad)/M_PI+0.5;
    m_ptEnd   = AngleToPoint500(m_nEnd);
}

//角度(deg*16)から円周上の点を求める
SPoint SPtnObjArc::AngleToPoint(int nAngle)
{
    int x,y;
    double angle;
    angle = nAngle*M_PI/(180*16);
    x =m_ptC.x() + ((double)m_nR*cos(angle));
    y =m_ptC.y() - ((double)m_nR*sin(angle));
    return SPoint(x,y);
}

SPoint SPtnObjArc::AngleToPoint500(int nAngle)
{
    int x,y;
    double angle;
    angle = nAngle*M_PI/(180*16);
    x =m_ptC.x() + (500.0*cos(angle));
    y =m_ptC.y() - (500.0*sin(angle));
    return SPoint(x,y);
}


bool SPtnObjArc::readCe3(SReadCE3& rce3)
{
    int width = 1;
    int style = 0;
    int cx = 10;
    int cy = 10;
    int r  = 10;
    int beginDeg = 0;
    int endDeg = 0;

    string str("");
    while(rce3.ReadRecord(str) != EOF) {
        if(str[0] == '-') {
            if(str != "-AR") {
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
                } else if(var == "X") {
                    cx = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                } else if(var == "Y") {
                    cy = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                } else if(var == "R") {
                    r = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                } else if(var == "B") {
                    beginDeg = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                } else if(var == "E") {
                    endDeg = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                }
            }
        }
    }

    this->m_width    = width;
    this->m_style    = style;
    this->m_ptC    	= SPoint(cx,cy);
    this->m_nR    	= r;
    this->m_nBegin    = beginDeg;
    this->m_nEnd    = endDeg;

    m_ptBegin = AngleToPoint500(beginDeg);
    m_ptEnd   = AngleToPoint500(endDeg);

    return true;
}


bool SPtnObjArc::writeCe3(SWriteCE3& wce3,SPoint* pOrigin)
{
    SPoint center = this->m_ptC;
    if(pOrigin != NULL) {
        center -= *pOrigin;
    }
    wce3.WriteRecord("+AR");
    wce3.WriteRecordInt("W",this->m_width);
    wce3.WriteRecordInt("S",this->m_style);
    wce3.WriteRecordInt("X",center.x());
    wce3.WriteRecordInt("Y",center.y());
    wce3.WriteRecordInt("R",this->m_nR);
    wce3.WriteRecordInt("B",this->m_nBegin);
    wce3.WriteRecordInt("E",this->m_nEnd);
    wce3.WriteRecord("-AR");
    wce3.WriteEOL();
    return true;
}

//グリップの位置を返す
SPoint SPtnObjArc::gripPos(int grip)
{
    switch(grip) {
    case 0:
        return AngleToPoint(m_nBegin);
    case 1:
        return AngleToPoint(m_nEnd);
    case 2:
        return m_ptC;
    case 3:    //0度
        return SPoint(m_ptC.x()+m_nR,m_ptC.y()     );
    case 4:    //90度
        return SPoint(m_ptC.x()     ,m_ptC.y()-m_nR);
    case 5:    //180度
        return SPoint(m_ptC.x()-m_nR,m_ptC.y()     );
    default:    //case 6:    //270度
        return SPoint(m_ptC.x()     ,m_ptC.y()+m_nR);
    }
}

//グリップの位置を動かす
void SPtnObjArc::setGripPos(int grip,int x,int y)
{
    switch(grip) {
    case 0:
        setBeginPoint(x,y);
        break;
    case 1:
        setEndPoint(x,y);
        break;
    case 2:
        m_ptC = SPoint(x,y);
        break;
    case 3:    //0度
    case 5:    //180度
        m_nR =  abs(m_ptC.x()-x);
        break;
    case 4:    //90度
    default:    //case 6:    //270度
        m_nR =  abs(m_ptC.y()-y);
        break;
    }
}



