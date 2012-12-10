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

#ifndef DRAWCOMPONENTBASE_H
#define DRAWCOMPONENTBASE_H

#include "sptnobj.h"
#include "sptnobjline.h"
#include "sptnobjcircle.h"
#include "sptnobjarc.h"
#include "sptnobjpolygon.h"
#include "sptnobjtext.h"

//void g_drawComponentBody(QPainter* p,
//                         const SCompIndex* compInfo,
//                         const SPoint& pos,
//                         const SSize& size,
//                         int dir,
//                         unsigned  nMode,
//                         int nScale);
void g_drawComponentPin(QPainter* p,const SPoint& pt,int nLtrb,int nType,unsigned  nMode,int nScale);
//SRect g_drawComponentPinArea(const SPoint& pt,int nLtrb);
SRect g_drawComponentPinNameArea(const SPoint& pt,int nLtrb,const char* name);
SRect g_drawComponentPinNumArea(const SPoint& pt,int nLtrb,const char* name,int pintype);
void g_drawComponentPinNum(QPainter* p,const SRect& rc,int nLtrb,unsigned  nMode,QColor colText,const char* name,int nScale,int nScaleMul);
void g_drawComponentPinName(QPainter* p,const SRect& rc,int nLtrb,unsigned  nMode,QColor colText,const char* name,int nScale,int nScaleMul);



SSize g_drawComponentNumName(QPainter* p,const SRect& rc,unsigned  nMode,bool bVert,QColor colText,const char* name,int nScale,int nScaleMul);

//部品の描画
void g_drawComponentBase(QPainter* p,
                         const SCompIndex* compInfo,
                         const SPoint& pos,
                         const SSize& size,
                         int dir,
                         int nBlock,
                         unsigned  nMode,
                         SRect* prcClip,
                         int nScale,
                         int nScaleMul,
                         bool dispAllNum=false);


void convDirXY(int&rx,int&ry,int dir,int cx,int cy);

void g_DrawPtnLine(
    QPainter*     p,            //描画
    SPtnObjLine*  pPtnObj,      //描画オブジェクト
    const QColor& col,          //描画色
    int dir,
    int cx,
    int cy,
    int x0,
    int y0,
    int nScale,
    int nScaleMul);

void g_DrawPtnCircle(
    QPainter*      p,           //描画
    SPtnObjCircle* pPtnObj,     //描画オブジェクト
    const QColor&  col,         //描画色
    int dir,
    int cx,
    int cy,
    int x0,
    int y0,
    int nScale,                 //
    int nScaleMul);

void g_DrawPtnPolygon(
    QPainter* p,                //描画
    SPtnObjPolygon* pPtnObj,    //描画オブジェクト
    const QColor& col,            //描画色
    int dir,
    int cx,
    int cy,
    int x0,
    int y0,
    int nScale,
    int nScaleMul);

void g_DrawPtnArc(
    QPainter* p,                //描画
    SPtnObjArc* pPtnObj,    //描画オブジェクト
    const QColor& col,            //描画色
    int dir,
    int cx,
    int cy,
    int x0,
    int y0,
    int nScale,
    int nScaleMul);

void g_DrawPtnText(
    QPainter* p,                //描画
    SPtnObjText* pPtnObj,    //描画オブジェクト
    const QColor& col,            //描画色
    int dir,
    int cx,
    int cy,
    int x0,
    int y0,
    int nScale,
    int nScaleMul);


void g_drawComponentPin(
    QPainter* p,
    const SPoint& pt,
    int nLtrb,
    int nType,
    unsigned  nMode,
    int nScale,
    int nScaleMul);

#endif

/*
  Local Variables:
  mode: c++
  End:
 */
