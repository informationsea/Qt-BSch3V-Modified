/****************************************************************************
    QtNlist   Netlist & Partslist utility for Qt-BSch3V
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
/****************************************************************************
** XBSch図面要素クラスの実装
****************************************************************************/
#include <stdio.h>
//#include <qnamespace.h>
#include "xbschobj.h"


//コンストラクタ
SXBSchObj::SXBSchObj()
{
	m_PlaceNum = 0;					//配置操作番号の初期化
	m_pRefObj = NULL;	//参照オブジェクトへのポインタ
	m_Layer = 0;					//配置レイヤー
	m_Selection = NOT_SELECTED;		//選択状態
	m_p1.setX(-1);
	m_p1.setY(-1);
}

//位置を動かす
void SXBSchObj::move(int offsetx,int offsety)
{
	SPoint p(offsetx,offsety);
	switch(m_Selection & SELECT_ALL){
	case NOT_SELECTED:
	case SELECT_ALL:
		m_p1 += p;
		m_p2 += p;
		break;
	case SELECT_P1:
		m_p1 += p;
		break;
	case SELECT_P2:
		m_p2 += p;
		break;
	}
}	
	
//X座標を指定して左右反転を行う
void SXBSchObj::mirrorH(int x)
{
	m_p1.setX(x*2 -m_p1.x());
	m_p2.setX(x*2 -m_p2.x());
}

//左右反転を行う
void SXBSchObj::mirrorH()
{
	int x= (m_p1.x() + m_p2.x())/2;
	mirrorH(x);
}
	
//XY中心座標を指定して、回転を行う
void SXBSchObj::rotate(const SPoint& p)
{
	int newX,newY;
	newY = p.y() + (m_p1.x() - p.x()) ;
	newX = p.x() - (m_p1.y() - p.y()) ;
	m_p1.setY( newY );
	m_p1.setX( newX );

	newY = p.y() + (m_p2.x() - p.x()) ;
	newX = p.x() - (m_p2.y() - p.y()) ;
	m_p2.setY( newY );
	m_p2.setX( newX );
}

//回転を行う
void SXBSchObj::rotate()
{
	SPoint p;
	p.setX((m_p1.x()+m_p2.x())/2);
	p.setY((m_p1.y()+m_p2.y())/2);
	rotate(p);
}

//選択フラグをセットする
void  SXBSchObj::setSelection(unsigned sel)
{
	m_Selection = sel;
}

//選択フラグをリセットする
void  SXBSchObj::resetSelection()
{
	m_Selection = 0;
}



//pとrcAreaの内外コードを返す
unsigned SXBSchObj::SetOutCode(const SPoint& p,const SRect& rcArea)
{
	unsigned outcode=0;											//  UL  | U | UR
	if		(p.x() < rcArea.l() )	outcode |= OUTCODE_LEFT;	//  ----+---+----
	else if (p.x() > rcArea.r() )	outcode |= OUTCODE_RIGHT;	//   L  | 0 |  R
	if		(p.y() < rcArea.t() )	outcode |= OUTCODE_UPPER;	//  ----+---+----
	else if	(p.y() > rcArea.b() )	outcode |= OUTCODE_LOWER;	//  LL  | L | LR
	return outcode;
}

//点と矩形の重なり情報を、重み情報付きで返す
int SXBSchObj::PointAndRectMag(const SPoint& p,const SRect& rcArea)
{
	int lp,rp,tp,bp,xp,yp;								//   left						  right
	if(( lp = p.x() - rcArea.l()  )<=0) return 0;	// ---+----------P------------------+----
	if(( rp = rcArea.r() - p.x() )<=0) return 0;	//         lp              rp
	if(( tp = p.y() - rcArea.t()   )<=0) return 0;
	if(( bp = rcArea.b()- p.y() )<=0) return 0;

	if(lp<rp) xp=lp; else xp=rp;						//小さい方の数値を「エッジから離れている値」として使う
	if(tp<bp) yp=tp; else yp=bp;
	xp=(xp*16)/(lp+rp);				//最大値は８
	yp=(yp*16)/(tp+bp);				//最大値は８
	return (xp*yp)+1;				//最大値は６５
}

