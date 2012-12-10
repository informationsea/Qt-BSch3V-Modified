/****************************************************************************
    BSch3V schematic capture
    Copyright (C) 1997-2005 H.Okada

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

#include "stdafx.h"
#include "SPtnObj.h"

SPtnObj::SPtnObj(void)
{
	m_style = 0;
	m_width = 1;
	m_nFill = -1;
}

SPtnObj::~SPtnObj(void)
{
}


//XY中心座標を指定して、回転を行う
SPoint SPtnObj::rotatePoint(SPoint pt,SPoint pCenter)
{
	int newX,newY;
	newY = pCenter.y() + (pt.x() - pCenter.x()) ;
	newX = pCenter.x() - (pt.y() - pCenter.y()) ;

	return SPoint(newX,newY);
}

//X座標を指定して左右反転を行う
SPoint SPtnObj::mirrorPoint(SPoint pt,int xCenter)
{
	int newX;
	newX = xCenter*2 -pt.x();
	return SPoint(newX,pt.y());
}

//p0とp1を端点座標とする線とpの重なりをテスト
bool SPtnObj::testIntersectLinePoint(const SPoint& p0,const SPoint& p1,const SPoint& p)
{
	int l=p.x()-1;
	int t=p.y()-1;
	int r=p.x()+1;
	int b=p.y()+1;
	SRect rcArea = SRect(p0,p1);
	rcArea.normalize();

	if( (l>rcArea.r()) ||  (t>rcArea.b()) || (r<rcArea.l()) ||  (b<rcArea.t()) ) return false;

	double dA =((double)p1.y()-(double)p0.y())/((double)p1.x()-(double)p0.x());
	double dB =(double)p0.y() - dA*(double)p0.x();

	double y0 =dA*l+dB;
	double y1 =dA*r+dB;

	if((y0>b && y1>b) || (y0<t && y1<t)) return false;

	return true;
}
