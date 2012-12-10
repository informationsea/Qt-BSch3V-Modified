/****************************************************************************
    NL3W netlist utility for BSch3V
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

#include <limits.h>
#include "coord.h"

//////////////////////////
//点座標クラス
SPoint::SPoint()
{
	m_x=0;
	m_y=0;
}

SPoint::SPoint(int x,int y)
{
	m_x=x;
	m_y=y;
}

void SPoint::setX(int x)
{
	m_x=x;
}


void SPoint::setY(int y)
{
	m_y=y;
}


int SPoint::x() const
{
	return m_x;
}


int SPoint::y() const
{
	return m_y;
}

SPoint& SPoint::operator += (const SPoint& p)
{
	m_x+=p.x();
	m_y+=p.y();
	return *this;
}

SPoint& SPoint::operator -= (const SPoint& p)
{
	m_x-=p.x();
	m_y-=p.y();
	return *this;
}



bool operator == (const SSize& s1,const SSize& s2)
{
	return (s1.w()==s2.w() && s1.h()==s2.h());
}

bool operator != (const SSize& s1,const SSize& s2)
{
	return (s1.w()!=s2.w() || s1.h()!=s2.h());
}

bool operator == (const SPoint& p1,const SPoint& p2)
{
	return (p1.x()==p2.x() && p1.y()==p2.y());
}

bool operator != (const SPoint& p1,const SPoint& p2)
{
	return (p1.x()!=p2.x() || p1.y()!=p2.y());
}
	



//////////////////////////
//サイズクラス
SSize::SSize()
{
	m_w=0;
	m_h=0;
}

SSize::SSize(int w,int h)
{
	m_w=w;
	m_h=h;
}

void SSize::setW(int w)
{
	m_w=w;
}

void SSize::setH(int h)
{
	m_h=h;
}

int SSize::w() const
{
	return m_w;
}

int SSize::h() const
{
	return m_h;
}

//////////////////////////
//矩形座標クラス
SRect::SRect()
{
	m_l=INT_MAX;
	m_t=INT_MAX;
	m_r=INT_MIN;
	m_b=INT_MIN;
}

SRect::SRect(int l,int t,int w,int h)
{
	m_l=l;
	m_t=t;
	m_r=l+w;
	m_b=t+h;
}

SRect::SRect(const SPoint& p1,const SPoint& p2)
{
	m_l=p1.x();
	m_t=p1.y();
	m_r=p2.x();
	m_b=p2.y();
}

void SRect::setL(int n)
{
	m_l=n;
}

void SRect::setT(int n)
{
	m_t=n;
}

void SRect::setR(int n)
{
	m_r=n;
}

void SRect::setB(int n)
{
	m_b=n;
}

void SRect::setW(int n)
{
	m_r=m_l+n;
}

void SRect::setH(int n)
{
	m_b=m_t+n;
}

int SRect::l() const
{
	return m_l;
}

int SRect::t() const
{
	return m_t;
}

int SRect::r() const
{
	return m_r;
}

int SRect::b() const
{
	return m_b;
}

int SRect::w() const
{
	return m_r-m_l;
}

int SRect::h() const
{
	return m_b-m_t;
}

void SRect::normalize()
{
	int n;
	if(m_l>m_r){
		n	=m_r;
		m_r	=m_l;
		m_l	=n;
	}
	if(m_t>m_b){
		n	=m_b;
		m_b	=m_t;
		m_t	=n;
	}
}

SPoint SRect::center() const
{
	return SPoint((m_l+m_r)/2,(m_t+m_b)/2);
}


//重なっていたらtrueを返す
bool SRect::intersect(const SRect& rc) const
{
	if(rc.l()>m_r) return false;
	if(rc.r()<m_l) return false;
	if(rc.t()>m_b) return false;
	if(rc.b()<m_t) return false;
	return true;
}

//重なっていたらtrueを返す
bool SRect::intersect(const SPoint& pt) const
{
	if(pt.x()>m_r) return false;
	if(pt.x()<m_l) return false;
	if(pt.y()>m_b) return false;
	if(pt.y()<m_t) return false;
	return true;
}


SRect& SRect::operator |= (const SRect& rc)
{
	if(m_l>rc.l()) m_l=rc.l();
	if(m_r<rc.r()) m_r=rc.r();
	if(m_t>rc.t()) m_t=rc.t();
	if(m_b<rc.b()) m_b=rc.b();
	return *this;
}

SRect& SRect::operator += (const SPoint& pt)
{
	m_l += pt.x();
	m_t += pt.y();
	m_r += pt.x();
	m_b += pt.y();
	return *this;
}
