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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <qcolor.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <QPolygon>
#include <list>
#include <string>
using namespace std;

#include "coord.h"
#include "xbsch.h"
#include "complib.h"
//#include "xbschobj.h"
//#include "xbschcomponent.h"
#include "xbschfont.h"
#include "xbschpattern.h"
//#include "drawcomponentbase.h"


static int PinDataNormal[4][4]= {
    {
        //L
        -10,    	0,
        0,    	0
    },
    {
        //T
        0,    	-10,
        0,    	0
    },
    {
        //R
        10,    	0,
        0,    0
    },
    {
        //B
        0,    	10,
        0,    	0
    }
};



#define CLK_DRAW_SIZE 4
static int PinDataClock[4][6]= {
    {
        //L
        0,    	    	    	CLK_DRAW_SIZE,
        CLK_DRAW_SIZE,    	    0,
        0,    	    	    	-CLK_DRAW_SIZE
    },
    {
        //T
        -CLK_DRAW_SIZE,    	    0,
        0,    	    	    	CLK_DRAW_SIZE,
        CLK_DRAW_SIZE,    	    0
    },
    {
        //R
        0,    	    	    	CLK_DRAW_SIZE,
        -CLK_DRAW_SIZE,    	    0,
        0,    	    	    	-CLK_DRAW_SIZE
    },
    {
        //B
        -CLK_DRAW_SIZE,    	    0,
        0,    	    	    	-CLK_DRAW_SIZE,
        CLK_DRAW_SIZE,    	    0
    }
};

#define INVCIR_SIZE 3
static int PinDataInvert[4][8]= {
    {
        //L
        -10,    	    	    0,
        -INVCIR_SIZE*2,    	    0,
        -INVCIR_SIZE*2,    	    -INVCIR_SIZE,
        0,    	    	    INVCIR_SIZE,
    },
    {
        //T
        0,    	    	    -10,
        0,    	    	    -INVCIR_SIZE*2,
        -INVCIR_SIZE,    	-INVCIR_SIZE*2,
        INVCIR_SIZE,    	0
    },
    {
        //R
        10,    	    	    0,
        INVCIR_SIZE*2,    	0,
        0,    	    	-INVCIR_SIZE,
        INVCIR_SIZE*2,INVCIR_SIZE,
    },
    {
        //B
        0,    	    	    10,
        0,    	    	    INVCIR_SIZE*2,
        -INVCIR_SIZE,    	0,
        INVCIR_SIZE,    	INVCIR_SIZE*2
    }
};


void convDirXY(int&rx,int&ry,int dir,int cx,int cy)
{
    int x,y,w;
    switch(dir&3) {
    case 0:
        x= rx;
        y= ry;
        w= cx;
        break;
    case 1:
        x=-ry+cy;
        y= rx;
        w= cy;
        break;
    case 2:
        x=-rx+cx;
        y=-ry+cy;
        w= cx;
        break;
    case 3:
    default:
        x= ry;
        y=-rx+cx;
        w= cy;
        break;
    }
    if(dir&4) x=-x+w;

    rx= x;
    ry= y;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//パターンのTextの描画
void g_DrawPtnText(
    QPainter* p,    	    	    //描画
    SPtnObjText* pObj,    	    //描画オブジェクト
    const QColor& col,    	    //描画色
    int  dir,
    int  cx,
    int  cy,
    int  x0,
    int  y0,
    int  nScale,
    int  nScaleMul)
{
    int xp,yp;
    xp = pObj->m_p0.x();
    yp = pObj->m_p0.y();
    convDirXY(xp,yp,dir,cx,cy);
    xp += x0;
    yp += y0;
    bool vflag = !(pObj->m_bHoriz);
    int align = pObj->m_align;
    if((align&2) == 0) {
        if(vflag) {
            if(dir&2) {
                align ^= 1;
            }
        } else {
            if((dir&3) == 1 || (dir&3) == 2) {
                align ^= 1;
            }
        }
    }
    if((align&8) == 0) {
        if(vflag) {
            if(dir&2) {
                align ^= 4;
            }
        } else {
            if((dir&3) == 1 || (dir&3) == 2) {
                align ^= 4;
            }
        }
    }

    if(dir&1) {
        vflag = !vflag;
    }
    if(dir&4) {
        if(!vflag && (align&2) == 0) {
            align ^= 1;
        }
        if(vflag && (align&8) == 0) {
            align ^= 4;
        }

    }
    SSize size;
    if(pObj->m_fontName.length() == 0) {
        size = g_SmallFont.DrawText(p,xp,yp,
                                    pObj->m_strText.c_str(),
                                    col,DRAW_ON,vflag,align,nScale,nScaleMul);
    } else {
        QFont font;
        QString strFontName = QString::fromLocal8Bit(pObj->m_fontName.c_str());
        font = QFont(strFontName);
        int pixelSize = pObj->m_fontSize;
        font.setBold(pObj->m_fontBold);
        font.setItalic(pObj->m_fontItalic);
        SSize drawSize = ::DrawString(
                             p,    	    	    //出力先
                             xp,yp,    	    	    //出力位置
                             pObj->m_strText.c_str(),    	    	//出力文字列
                             col,    	    //出力色
                             font,
                             pixelSize,
                             //DRAW_ON,    	    	    //描画モード
                             vflag,    	    	    	//垂直フラグ
                             align,    	    	    //指定した出力位置が 0:文字列の前 1:文字列の後ろ 2:文字列の中央
                             //    	    	     0:文字列の下 4:文字列の上   8:文字列の中
                             nScale,
                             nScaleMul);
        pObj->setDrawSize(drawSize);
    }
}





//パターンのLineの描画
void g_DrawPtnLine(
    QPainter* p,    	    	    //描画
    SPtnObjLine* pPtnObj,    	    //描画オブジェクト
    const QColor& col,    	    //描画色
    int  dir,
    int  cx,
    int  cy,
    int  x0,
    int  y0,
    int  nScale,
    int  nScaleMul)
{
    int x1,y1,x2,y2;
    Qt::PenStyle style = Qt::SolidLine;

    int width = (pPtnObj->m_width * nScaleMul)/nScale;

    if(pPtnObj->m_style != 0) {
#ifdef Q_WS_MACX
        style = Qt::DashLine;
#else
        style = Qt::DotLine;
#endif
    }

    //QPen pen(col,width,style);
    QPen pen(col,width,style,Qt::RoundCap, Qt::RoundJoin);

    x1 = pPtnObj->m_p0.x();
    y1 = pPtnObj->m_p0.y();
    convDirXY(x1,y1,dir,cx,cy);
    x1 += x0;
    y1 += y0;
    x1 = (x1*nScaleMul)/nScale;
    y1 = (y1*nScaleMul)/nScale;

    x2 = pPtnObj->m_p1.x();
    y2 = pPtnObj->m_p1.y();
    convDirXY(x2,y2,dir,cx,cy);
    x2 += x0;
    y2 += y0;
    x2 = (x2*nScaleMul)/nScale;
    y2 = (y2*nScaleMul)/nScale;

    p->setPen(pen);
    p->drawLine(x1,y1,x2,y2);

}

void g_DrawPtnCircle(
    QPainter* p,    	    	//描画
    SPtnObjCircle* pPtnObj,    //描画オブジェクト
    const QColor& col,    	    //描画色
    int     dir,
    int     cx,
    int  cy,
    int     x0,
    int  y0,
    int  nScale,    	    	//
    int  nScaleMul)    	    //
{
    int x1,y1,x2,y2,h,w;

    int width = (pPtnObj->m_width * nScaleMul ) / nScale;
    Qt::PenStyle style = Qt::SolidLine;


    if(pPtnObj->m_style != 0) {
#ifdef Q_WS_MACX
        style = Qt::DashLine;
#else
        style = Qt::DotLine;
#endif
    }


    int fill = pPtnObj->m_nFill;
    if(fill == -1) {
        p->setBrush( Qt::NoBrush );
    } else {
        p->setBrush( col );
    }

    x1 = pPtnObj->m_p0.x();
    y1 = pPtnObj->m_p0.y();
    convDirXY(x1,y1,dir,cx,cy);
    x1 += x0;
    y1 += y0;
    x1 = (x1*nScaleMul ) / nScale;
    y1 = (y1*nScaleMul ) / nScale;

    x2 = pPtnObj->m_p1.x();
    y2 = pPtnObj->m_p1.y();
    convDirXY(x2,y2,dir,cx,cy);
    x2 += x0;
    y2 += y0;
    x2 = (x2*nScaleMul ) / nScale;
    y2 = (y2*nScaleMul ) / nScale;

    int l=(x1 < x2 ? x1 : x2);
    int t=(y1 < y2 ? y1 : y2);

    w = abs(x2-x1);
    h = abs(y2-y1);
    if(width <= 1) {
        //Qt4    	w++;
        //Qt4    	h++;
        width = 1;
    }

    QPen pen(col,width,style);
    p->setPen(pen);
    p->drawEllipse(l,t,w,h);
}

void g_DrawPtnPolygon(
    QPainter* p,    	    	//描画
    SPtnObjPolygon* pPtnObj,    //描画オブジェクト
    const QColor& col,    	    //描画色
    int     dir,
    int     cx,
    int  cy,
    int     x0,
    int  y0,
    int  nScale,    	    	    //
    int  nScaleMul)    	    	//
{
    static QPolygon pa(VECTPLOYGON_MAXNODE);

    int node = pPtnObj->m_nCount;
    if(node < 2||node > VECTPLOYGON_MAXNODE) return;

    int x1,y1;

    int width = (pPtnObj->m_width * nScaleMul ) / nScale;
    if(width < 1) width = 1;

    Qt::PenStyle style = Qt::SolidLine;

    if(pPtnObj->m_style != 0) {
#ifdef Q_WS_MACX
        style = Qt::DashLine;
#else
        style = Qt::DotLine;
#endif
    }

    QPen pen(col,width,style,Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen);

    int fill = pPtnObj->m_nFill;
    if(fill == -1) {
        p->setBrush( Qt::NoBrush );
    } else {
        p->setBrush( col );
    }
    int i;
    for(i = 0; i < node; i++) {
        x1 = pPtnObj->m_pPt[i].x();
        y1 = pPtnObj->m_pPt[i].y();
        convDirXY(x1,y1,dir,cx,cy);
        x1 += x0;
        y1 += y0;
        x1 = (x1*nScaleMul ) / nScale;
        y1 = (y1*nScaleMul ) / nScale;
        pa.setPoint(i,x1,y1);
    }

    //p->drawPolygon (pa,true, 0, node );
    {
        p->drawPolygon( pa.constData(), node, Qt::WindingFill);
    }

}

void g_DrawPtnArc(
    QPainter* p,    	    	//描画
    SPtnObjArc* pPtnObj,    //描画オブジェクト
    const QColor& col,    	    //描画色
    int     dir,
    int     cx,
    int  cy,
    int     x0,
    int  y0,
    int  nScale,    	    	    //
    int  nScaleMul)    	    	//
{
    //    CPen newPen;
    //    CPen* pPenOld;

    int x,y,r,x1,y1,x2,y2;//,x3,y3,x4,y4;

    int width = (pPtnObj->m_width * nScaleMul ) / nScale;
    if(width < 1) width = 1;

    Qt::PenStyle style = Qt::SolidLine;

    if(pPtnObj->m_style != 0) {
#ifdef Q_WS_MACX
        style = Qt::DashLine;
#else
        style = Qt::DotLine;
#endif
    }

    QPen pen(col,width,style,Qt::RoundCap, Qt::RoundJoin);
    //QPen pen(col,width,style);
    p->setPen(pen);

    x = pPtnObj->m_ptC.x();
    y = pPtnObj->m_ptC.y();
    r = pPtnObj->m_nR;

    x1 = x-r;
    y1 = y-r;

    convDirXY(x1,y1,dir,cx,cy);
    x1 += x0;
    y1 += y0;
    x1 = (x1* nScaleMul ) / nScale;
    y1 = (y1* nScaleMul ) / nScale;


    x2 = x+r;
    y2 = y+r;
    convDirXY(x2,y2,dir,cx,cy);
    x2 += x0;
    y2 += y0;
    x2 = (x2* nScaleMul ) / nScale;
    y2 = (y2* nScaleMul ) / nScale;


    int a = pPtnObj->m_nBegin;
    int ae = pPtnObj->m_nEnd;
    if(ae < a) ae += 360*16;
    int alen = ae-a;

    a=(a-(dir&3)*90*16)%(360*16);
    if(dir&4) {
        a = 180*16-a;
        alen=-alen;
    }

    int l=(x1 < x2 ? x1 : x2);
    int t=(y1 < y2 ? y1 : y2);

    //Qt4    p->drawArc(l,t,abs(x2-x1)+1,abs(y2-y1)+1,a,alen);
    p->drawArc(l,t,abs(x2-x1),abs(y2-y1),a,alen);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void g_drawComponentBody(QPainter* p,
                         const SCompIndex* compInfo,
                         const SPoint& pos,
                         const SSize& size,
                         int dir,
                         unsigned  int nMode,
                         int nScale,
                         int nScaleMul)
{
    QColor col;
    //Qt4    Qt::RasterOp op = Qt::CopyROP;
    const SPtn* pPtn =NULL;
    SSize ssize(10,10);
    if(compInfo) {
        pPtn =compInfo->pattern();
        ssize = compInfo->size();
    }



    switch(nMode & 0xffff) {
    case DRAW_ON:
        col = QColor("firebrick");
        break;
    case DRAW_MONO:
        col = Qt::color1;
        break;
    case DRAW_OFF:
        col = QColor("white");
        break;
    case DRAW_DARK:
        col = QColor("orange");
        break;
    case DRAW_TEMP:
    default:
        col = QColor("red");
        break;
    }




    //胴部矩形の描画
    //パターンが存在しない場合は矩形を描画
    if(pPtn == NULL ) {    //g_bDisplayPartFrame != FALSE){
        int width = 1*nScaleMul / nScale;
        int w = size.w();
        int h = size.h();
        int x = (pos.x() - w) * nScaleMul / nScale;
        int y = (pos.y() - h) * nScaleMul / nScale;
        w = w * nScaleMul / nScale;
        h = h * nScaleMul / nScale;


        //    	p->setRasterOp(op);
        QPen pen(col,width,Qt::SolidLine);
        p->setPen(pen);
        QBrush brush(Qt::NoBrush);
        p->setBrush(brush);
        //Qt4    	p->drawRect(x,y,w+1,h+1);
        p->drawRect(x,y,w,h);
    } else {
        if(pPtn->m_ppvPtn) {
            int cx = ssize.w()*10;
            int cy = ssize.h()*10;
            int x = pos.x()-size.w();
            int y = pos.y()-size.h();
            int i = 0;
            int retv;
            while(i < pPtn->m_vPtnCount) {
                SPtnObj* pPtnObj = pPtn->m_ppvPtn[i];
                retv = 1;
                switch(pPtnObj->id()) {
                case VECTPTN_L:
                    g_DrawPtnLine(p,(SPtnObjLine*)pPtnObj,col,dir,cx,cy,x,y,nScale,nScaleMul);
                    break;
                case VECTPTN_C:
                    g_DrawPtnCircle(p,(SPtnObjCircle*)pPtnObj,col,dir,cx,cy,x,y,nScale,nScaleMul);
                    break;
                case VECTPTN_PG:
                    g_DrawPtnPolygon(p,(SPtnObjPolygon*)pPtnObj,col,dir,cx,cy,x,y,nScale,nScaleMul);
                    break;
                case VECTPTN_AR:
                    g_DrawPtnArc(p,(SPtnObjArc*)pPtnObj,col,dir,cx,cy,x,y,nScale,nScaleMul);
                    break;
                case VECTPTN_TXT:
                    g_DrawPtnText(p,(SPtnObjText*)pPtnObj,col,dir,cx,cy,x,y,nScale,nScaleMul);
                    break;
                default:
                    retv = 0;
                }
                if(retv == 0) break;
                i++;
            }
        } else if(pPtn->m_pBuff) {
            g_put_bmp(
                p,    	    	    //出力先
                pos.x() - size.w(),
                pos.y() - size.h(),    	//出力位置
                pPtn->m_pBuff,    	//出力するビットマップ(ワードアラインメント モノクロ)
                pPtn->m_size.w(),    //ピクセル幅
                pPtn->m_size.h(),    //ピクセル高
                dir,    	//向き
                nScale,    	    	    //スケール 1,2,5のどれか
                nScaleMul,
                col,    	//出力色
                nMode    	    	//描画モード
            );

            return;
        }
    }
}



void g_drawComponentPin(QPainter* p,const SPoint& pt,int nLtrb,int nType,unsigned  nMode,int nScale,int nScaleMul)
{
    int x0 = pt.x();
    int y0 = pt.y();
    int x1,y1,x2,y2;
    QColor col;
    QPolygon pa(3);

    int lineWidth;
    if(nType & PIN_TYPE_ZLENG) {
        if((nMode&DRAW_FOR_PRINT) || (nScaleMul < nScale)) return;
        lineWidth = 1;
    } else {
        lineWidth = nScaleMul/nScale;
    }

    switch(nMode & 0xffff) {
    case DRAW_ON:
        if(nType & PIN_TYPE_ZLENG) {
            col = QColor("orange");
        } else {
            col = QColor("firebrick");
        }
        break;
    case DRAW_MONO:
        col = Qt::color1;
        break;
    case DRAW_OFF:
        col = QColor("white");
        break;
    case DRAW_TEMP:
        col = QColor("red");
        break;
    case DRAW_DARK:
        col = QColor("orange");
        break;
    default:
        return;
    }


    //    int lineWidth = nScaleMul/nScale;
    if(lineWidth < 1)lineWidth = 1;

    QPen pen(col,lineWidth,Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin);
    //    QPen pen(col,lineWidth,Qt::SolidLine);
    p->setPen(pen);

    if(nType & PIN_TYPE_ZLENG) {
        x1=((x0-2)*nScaleMul)/nScale;
        y1=((y0-2)*nScaleMul)/nScale;
        x2=((x0+2)*nScaleMul)/nScale;
        y2=((y0+2)*nScaleMul)/nScale;
        p->setBrush(Qt::NoBrush);
        p->drawRect(x1,y1,x2-x1,y2-y1);
        return;
    }

    if((nType & PIN_TYPE_NEGATIVE) == 0) {
        x1=((PinDataNormal[nLtrb][0]+x0)*nScaleMul)/nScale;
        y1=((PinDataNormal[nLtrb][1]+y0)*nScaleMul)/nScale;
        x2=((PinDataNormal[nLtrb][2]+x0)*nScaleMul)/nScale;
        y2=((PinDataNormal[nLtrb][3]+y0)*nScaleMul)/nScale;
        p->drawLine(x2,y2,x1,y1);
    } else {
        x1=((PinDataInvert[nLtrb][0]+x0)*nScaleMul)/nScale;
        y1=((PinDataInvert[nLtrb][1]+y0)*nScaleMul)/nScale;
        x2=((PinDataInvert[nLtrb][2]+x0)*nScaleMul)/nScale;
        y2=((PinDataInvert[nLtrb][3]+y0)*nScaleMul)/nScale;
        p->drawLine(x2,y2,x1,y1);

        p->setBrush( Qt::NoBrush );
        x1=((PinDataInvert[nLtrb][4]+x0)*nScaleMul)/nScale;
        y1=((PinDataInvert[nLtrb][5]+y0)*nScaleMul)/nScale;
        //Qt4    	x2=((PinDataInvert[nLtrb][6]+x0)*nScaleMul)/nScale+1;
        //Qt4    	y2=((PinDataInvert[nLtrb][7]+y0)*nScaleMul)/nScale+1;
        x2=((PinDataInvert[nLtrb][6]+x0)*nScaleMul)/nScale;
        y2=((PinDataInvert[nLtrb][7]+y0)*nScaleMul)/nScale;
        p->drawEllipse(x1,y1,x2-x1,y2-y1);
    }
    if(nType & PIN_TYPE_CLOCK) {
        x1=((PinDataClock[nLtrb][0]+x0)*nScaleMul)/nScale;
        y1=((PinDataClock[nLtrb][1]+y0)*nScaleMul)/nScale;
        pa.setPoint(0,x1,y1);
        x1=((PinDataClock[nLtrb][2]+x0)*nScaleMul)/nScale;
        y1=((PinDataClock[nLtrb][3]+y0)*nScaleMul)/nScale;
        pa.setPoint(1,x1,y1);
        x1=((PinDataClock[nLtrb][4]+x0)*nScaleMul)/nScale;
        y1=((PinDataClock[nLtrb][5]+y0)*nScaleMul)/nScale;
        pa.setPoint(2,x1,y1);
        p->drawPolyline(pa);
    }
}

SRect g_drawComponentPinArea(const SPoint& pt,int nLtrb)
{
    int l,t,w,h;
    switch(nLtrb) {
    case PIN_LOC_L:
        l = pt.x();
        t = pt.y()-4;
        w = 16;
        h = 9;
        break;
    case PIN_LOC_T:
        l = pt.x()-4;
        t = pt.y();
        w = 9;
        h = 16;
        break;
    case PIN_LOC_R:
        l = pt.x()-16;
        t = pt.y()-4;
        w = 16;
        h = 9;
        break;
    default:
        l = pt.x()-4;
        t = pt.y()-16;
        w = 9;
        h = 16;
        break;
    }
    return SRect(l,t,w,h);
}

SRect g_drawComponentPinNameArea(const SPoint& pt,int nLtrb,const char* name)
{
    if(name == NULL || *name == '\0') return SRect();
    int len = strlen(name)*8;
    int l,t,w,h;
    switch(nLtrb) {
    case PIN_LOC_L:
        l = pt.x()+15;
        t = pt.y()-7;
        w = len;
        h = 10;
        break;
    case PIN_LOC_T:
        l = pt.x()-7;
        t = pt.y()+15;
        w = 10;
        h = len;
        break;
    case PIN_LOC_R:
        l = pt.x()-15-len;
        t = pt.y()-7;
        w = len;
        h = 10;
        break;
    default:
        l = pt.x()-7;
        t = pt.y()-15-len;
        w = 10;
        h = len;
        break;
    }
    return SRect(l,t,w,h);
}


SRect g_drawComponentPinNumArea(const SPoint& pt,int nLtrb,const char* name,int pintype)
{
    if(name == NULL || *name == '\0') return SRect();
    int nOffset;
    int len = strlen(name)*8;
    int l,t,w,h;

    if(pintype & (PIN_TYPE_SMALL|PIN_TYPE_ZLENG)) {
        nOffset = 2;
    } else {
        nOffset = 9;
    }

    switch(nLtrb) {
    case PIN_LOC_L:
        l = pt.x()+10-nOffset-len;
        t = pt.y()-9;
        w = len;
        h = 8;
        break;
    case PIN_LOC_T:
        l = pt.x()-9;
        t = pt.y()+10-nOffset-len;
        w = 8;
        h = len;
        break;
    case PIN_LOC_R:
        l = pt.x()-10+nOffset;
        t = pt.y()-9;
        w = len;
        h = 8;
        break;
    default:
        l = pt.x()-9;
        t = pt.y()-10+nOffset;
        w = 8;
        h = len;
        break;
    }
    return SRect(l,t,w,h);
}

//ピン名の描画
void g_drawComponentPinName(QPainter* p,const SRect& rc,int nLtrb,unsigned  nMode,QColor colText,const char* name,int nScale,int nScaleMul)
{
    int x;
    int y;
    int pointpos;

    switch(nLtrb) {
    case 0: // L
        x = rc.l();
        y = rc.b();
        pointpos = 0;
        break;
    case 1: // T
        x = rc.r();
        y = rc.t();
        pointpos = 1;
        break;
    case 2: // R
        x = rc.r();
        y = rc.b();
        pointpos = 1;
        break;
    case 3: // B
        x = rc.r();
        y = rc.b();
        pointpos = 0;
        break;
    default:
        return;
    }

    g_SmallFont.DrawText(p,x,y,name,colText,nMode,nLtrb&1,pointpos,nScale,nScaleMul,true);
}

// ピン番号の描画
//void g_drawComponentPinNumName(QPainter* p,const SRect& rc,int nLtrb,unsigned  nMode,QColor colText,const char* name,int nScale,int nScaleMul)
void g_drawComponentPinNum(QPainter* p,const SRect& rc,int nLtrb,unsigned  nMode,QColor colText,const char* name,int nScale,int nScaleMul)
{
    int x;
    int y;
    int pointpos;

    switch(nLtrb) {
    case 0: // L
        x = rc.r();
        y = rc.b();
        pointpos = 1;
        break;
    case 1: // T
        x = rc.r();
        y = rc.b();
        pointpos = 0;
        break;
    case 2: // R
        x = rc.l();
        y = rc.b();
        pointpos = 0;
        break;
    case 3: // B
        x = rc.r();
        y = rc.t();
        pointpos = 1;
        break;
    default:
        return;
    }

    g_SmallFont.DrawText(p,x,y,name,colText,nMode,nLtrb&1,pointpos,nScale,nScaleMul);

}

SSize g_drawComponentNumName(QPainter* p,const SRect& rc,unsigned  nMode,bool bVert,QColor colText,const char* name,int nScale,int nScaleMul)
{
    int x = (bVert ? rc.r() : rc.l());
    return g_SmallFont.DrawText(p,x,rc.b(),name,colText,nMode,bVert,0,nScale,nScaleMul,false);
}


//部品の描画
void g_drawComponentBase(QPainter* p,
                         const SCompIndex* compInfo,
                         const SPoint& pos,
                         const SSize& rsize,//compInfo がNULLのときだけ使う
                         int dir,
                         int nBlock,
                         unsigned  nMode,
                         SRect* prcClip,
                         int nScale,
                         int nScaleMul,
                         bool dispAllNum)
{
    SSize size;
    if(compInfo == NULL) {
        size = rsize;
    } else {
        size = compInfo->sizePlace(dir);
    }
    //SRect rc = pObj->area();
    SRect rc(pos.x()-size.w(),pos.y()-size.h(),size.w(),size.h());
    if(prcClip == NULL || rc.intersect(*prcClip)) {
        g_drawComponentBody(p,
                            compInfo,
                            pos,
                            size,
                            dir,
                            nMode,nScale,nScaleMul);
    }
    if(compInfo == NULL) return;
    //int maxpin = pObj->pinCount();
    int maxpin = compInfo->pinCount();
    int pinindex;

    QColor colText;
    switch(nMode & 0xffff) {
    case DRAW_ON:
        colText = QColor("#060035");
        break;
    case DRAW_MONO:
        colText = Qt::color1;
        break;
    case DRAW_OFF:
        colText = QColor("white");
        break;
    case DRAW_DARK:
        colText = QColor("orange");
        break;
    case DRAW_TEMP:
        colText = QColor("red");
        break;
        //    case DRAW_XOR:    colText = QColor("red");    break;
    }

    QColor colHidePin("red");

    //int nBlock = pObj->block();
    for(pinindex = 0; pinindex < maxpin ; pinindex++) {
        SPoint pinLoc;
        const SPin* pPinInfo;
        int ltrb;
        pPinInfo = compInfo->pin(pinindex,dir,ltrb,pinLoc);
        //pPinInfo = pObj->pinEnd(pinindex,ltrb,pinend);
        pinLoc += pos;
        if(pPinInfo == NULL)break;

        SPoint pinEnd(pinLoc);    //pinEndはゼロ長であっても、ボディから10座標離れたポイントを指す。
        switch(ltrb) {
        case 0:
            pinEnd.setX(pinEnd.x()-10);
            break;
        case 1:
            pinEnd.setY(pinEnd.y()-10);
            break;
        case 2:
            pinEnd.setX(pinEnd.x()+10);
            break;
        default:
            pinEnd.setY(pinEnd.y()+10);
            break;
        }
        rc = g_drawComponentPinArea(pinEnd,ltrb);
        if(prcClip == NULL || rc.intersect(*prcClip)) {
            g_drawComponentPin(p,pinLoc,ltrb,pPinInfo->type(),nMode,nScale,nScaleMul);
        }
        //    	if(((nMode & 0xffff) != DRAW_XOR)||(nMode & DRAW_SINGLE)){    //2005/05/15 削除
        if(nScale <= nScaleMul) {    //ピン名,ピン番号は縮小時は描画しない
            rc = g_drawComponentPinNameArea(pinEnd,ltrb,pPinInfo->pinName());
            if(prcClip == NULL || rc.intersect(*prcClip)) {
                g_drawComponentPinName(p,rc,ltrb,nMode,colText,pPinInfo->pinName(),nScale,nScaleMul);
            }
            if((pPinInfo->type() & PIN_TYPE_NUMHIDE) == 0) {
                rc = g_drawComponentPinNumArea(pinEnd,ltrb,pPinInfo->pinNum(nBlock),pPinInfo->type());
                if(prcClip == NULL || rc.intersect(*prcClip)) {
                    g_drawComponentPinNum(p,rc,ltrb,nMode,colText,pPinInfo->pinNum(nBlock),nScale,nScaleMul);
                }
            } else if(dispAllNum) {
                rc = g_drawComponentPinNumArea(pinEnd,ltrb,pPinInfo->pinNum(nBlock),pPinInfo->type());
                if(prcClip == NULL || rc.intersect(*prcClip)) {
                    g_drawComponentPinNum(p,rc,ltrb,nMode,colHidePin,pPinInfo->pinNum(nBlock),nScale,nScaleMul);
                }
            }
        }

        //    	}
    }
}

