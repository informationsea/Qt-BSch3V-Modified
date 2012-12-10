/****************************************************************************
    Qt-BSch3V schematic capture
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


#ifndef _DRAWOBJECT_H
#define _DRAWOBJECT_H

extern QFont g_defaultfont;

// フレームの縁の水平方向のマーク文字(ABCD...)を描画
void g_drawFrameHMarks(QPainter*p,int y,int width,const QColor& color,int m_viewScale,int m_viewScaleMul);

// フレームの縁の垂直方向のマーク文字(1234...)を描画
void g_drawFrameVMarks(QPainter*p,int x,int height,const QColor& color,int m_viewScale,int m_viewScaleMul);

//フレームの描画
void g_drawFrame(QPainter*p, SSize size, const QRect& rcClip,const QColor& color,int m_viewScale,int m_viewScaleMul);

//図面要素の描画
//ジャンクションの描画
void g_drawJunction(QPainter* p,SXBSchObj* pObj,unsigned  nMode,int m_viewScale,int m_viewScaleMul);

//コメントの描画
void g_drawComment(QPainter* p,SXBSchObj* pObj,unsigned  nMode,int m_viewScale,int m_viewScaleMul);

//ラベルの描画
void g_drawLabel(QPainter* p,SXBSchObj* pObj,unsigned  nMode , int m_viewScale,int m_viewScaleMul);


//タグの描画
void g_drawTag(QPainter* p,SXBSchObj* pObj,unsigned  nMode,int m_viewScale,int m_viewScaleMul);

//線の描画
void g_drawLineData(QPainter* p,SXBSchObj* pObj,unsigned  nMode,int m_viewScale,int m_viewScaleMul);

void g_drawXBSchObj(QPainter*p,SXBSchObj* pObj,unsigned  nMode,SRect* prcClip,int m_viewScale,int m_viewScaleMul);
#endif


/*
  Local Variables:
  mode: c++
  End:
 */
