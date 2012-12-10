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

#ifndef COORD_H
#define COORD_H


//サイズクラス
class SSize
{
protected:
	int m_w;
	int m_h;
public:
	SSize();
	SSize(int w,int h);
	void setW(int w);
	void setH(int h);
	int w() const;
	int h() const;
};

bool operator == (const SSize& s1,const SSize& s2);
bool operator != (const SSize& s1,const SSize& s2);


//点座標クラス
class SPoint
{
protected:
	int m_x;
	int m_y;
public:
	SPoint();
	SPoint(int x,int y);
	void setX(int x);
	void setY(int y);
	int x() const;
	int y() const;
	SPoint& operator += (const SPoint&);
	SPoint& operator -= (const SPoint&);
};

bool operator == (const SPoint& p1,const SPoint& p2);
bool operator != (const SPoint& p1,const SPoint& p2);



//矩形座標クラス
class SRect
{
protected:
	int m_l;
	int m_t;
	int m_r;
	int m_b;
public:
	SRect();
	SRect(int l,int t,int w,int h);
	SRect(const SPoint& p1,const SPoint& p2);
	void setL(int n);
	void setT(int n);
	void setR(int n);
	void setB(int n);
	void setW(int n);
	void setH(int n);
	int l() const;
	int t() const;
	int r() const;
	int b() const;
	int w() const;
	int h() const;
	void normalize();
	SPoint center() const;
	bool intersect(const SRect& rc) const;//重なっていたらtrueを返す
	bool intersect(const SPoint& pt) const;//重なっていたらtrueを返す
	SRect& operator |= (const SRect&);
	SRect& operator += (const SPoint&);
};

#endif
