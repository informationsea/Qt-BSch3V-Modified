/****************************************************************************
 Qt-BSch3V schematic capture
 Copyright (C) 2002-2006 H.Okada

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

#include <stdio.h>
#include <assert.h>

#include <qcolor.h>
#include <qpoint.h>
#include <qsize.h>
#include <qrect.h>
#include <qpainter.h>
#include <QPolygon>
#include <qfont.h>
#include <qfontinfo.h>

#include <QDesktopWidget>
#include <QApplication>


// #include "xbschview.h"
// #include "xbschdoc.h"
#include "xbschfont.h"
#include "xbschpattern.h"
#include "xbsch.h"
#include "xbschobj.h"
#include "xbschjunc.h"
#include "xbschlabel.h"
#include "xbschcomment.h"
#include "xbschtag.h"
#include "xbschcomponent.h"
#include "drawcomponent.h"
#include "xbschglobal.h"

QFont g_defaultfont;

////////////////////////////////////////////
// 図面のフレームの描画関連

// フレームの縁の水平方向のマーク文字(ABCD...)を描画
void g_drawFrameHMarks(QPainter*p,int y,int width,const QColor& color,int m_viewScale,int m_viewScaleMul)
{
    int nScale    =m_viewScale;
    int nScaleMul    =m_viewScaleMul;

    int x = 100;
    char str[2];
    str[0]='A';
    str[1]='\0';
    while(x < width) {
        g_SmallFont.DrawText(p,x,y,str,color,DRAW_ON,false,0,nScale,nScaleMul);
        x += 200;
        str[0]++;
    }
}

// フレームの縁の垂直方向のマーク文字(1234...)を描画
void g_drawFrameVMarks(QPainter*p,int x,int height,const QColor& color,int m_viewScale,int m_viewScaleMul)
{
    int nScale    =m_viewScale;
    int nScaleMul    =m_viewScaleMul;

    int y = 100;
    char str[2] = "1";
    while(y < height) {
        g_SmallFont.DrawText(p,x,y,str,color,DRAW_ON,false,0,nScale,nScaleMul);
        y += 200;
        if(str[0] == '9') str[0]='0';
        else str[0]++;
    }
}

//フレームの描画
void g_drawFrame(QPainter*p,SSize size,const QRect& rcClip,const QColor& color,int nScale,int nScaleMul)
{
    //SSize size = m_pDoc->SheetSize();
    int h = size.h()*nScaleMul/nScale;
    int w = size.w()*nScaleMul/nScale;
    int fw = 10*nScaleMul/nScale;
    int lineWidth = 1*nScaleMul/nScale;
    if( lineWidth <1) lineWidth =1;
    //p->setRasterOp(Qt::CopyROP);
    QPen pen(color,lineWidth,Qt::SolidLine);
    //QPen pen(color);
    p->setPen(pen);

    //左フレームの描画
    if(rcClip.left() <= fw) {
        p->drawLine(0,0,0,h-1);
        p->drawLine(fw,fw,fw,h-fw);
        if(nScale <= nScaleMul)g_drawFrameVMarks(p,1,size.h(),color,nScale,nScaleMul);
        //g_SmallFont.DrawText(p,50,50,"ABC",QColor("blue"),DRAW_ON);
    }
    //右フレームの描画
    if(rcClip.right() >= (w-fw)) {
        p->drawLine(w-1,0,w-1,h-1);
        p->drawLine(w-fw,fw,w-fw,h-fw);
        if(nScale <= nScaleMul)g_drawFrameVMarks(p,size.w()-9,size.h(),color,nScale,nScaleMul);
    }
    //上フレームの描画
    if(rcClip.top() <= fw) {
        p->drawLine(0,0,w-1,0);
        p->drawLine(fw,fw,w-fw,fw);
        if(nScale <= nScaleMul)g_drawFrameHMarks(p,8,size.w(),color,nScale,nScaleMul);
    }
    //下フレームの描画
    if(rcClip.bottom() >= (h-fw)) {
        p->drawLine(0,h-1,w-1,h-1);
        p->drawLine(fw,h-fw,w-fw,h-fw);
        if(nScale <= nScaleMul)g_drawFrameHMarks(p,size.h()-2,size.w(),color,nScale,nScaleMul);
    }
}

//図面要素の描画
//ジャンクションの描画
void g_drawJunction(QPainter* p,SXBSchObj* pObj,unsigned  nMode,int m_viewScale,int m_viewScaleMul)
{
    if(pObj->id() != SXBSchObj::ID_JUNCTION) return;

    SXBSchJunc* pJunc =(SXBSchJunc*)pObj;

    QColor col;
    //Qt::RasterOp op = Qt::CopyROP;


    switch(nMode & 0xffff) {
    case DRAW_ON:
        col = QColor("#028200");
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
        col = QColor("red");
        break;
    }
    //p->setRasterOp(op);
    int s = 2*m_viewScaleMul/m_viewScale;
    //Qt4    int w = s*2+1;
    int w = s*2;
    int x = pJunc->x1()*m_viewScaleMul/m_viewScale;
    int y = pJunc->y1()*m_viewScaleMul/m_viewScale;

    if(s > 0) {
        p->setPen(col);
        p->setBrush(col);
        p->drawEllipse(x-s,y-s,w,w);//,col);
    } else {
        p->setPen(col);
        p->drawPoint(x,y);
    }

}


//図面要素の描画

//コメントの描画
void g_drawComment(QPainter* p,SXBSchObj* pObj,unsigned  nMode,int m_viewScale,int m_viewScaleMul)
{
    if(pObj->id() != SXBSchObj::ID_COMMENT) return;

    SXBSchComment* pComment =(SXBSchComment*)pObj;
    QFont font;
    QColor col;
    QString strText;
    //Qt::RasterOp op = Qt::CopyROP;

    int pixelSize;
    if(strlen(pComment->fontName())>0) {
        QString strFontName = QString::fromLocal8Bit(pComment->fontName());
        font = QFont(strFontName);
        //    	QFontInfo info( font );
        //      	QString fontFamily = info.family();
        //    	if(fontFamily != strFontName){
        //    	    font = g_defaultfont;
        //    	}
        font.setBold(pComment->fontBold() != 0);
        font.setItalic(pComment->fontItalic() != 0);
        font.setStrikeOut(pComment->fontStrikeOut() != 0);
        font.setUnderline(pComment->fontUnderline() != 0);
        pixelSize = pComment->fontSize();
        //    	printf("pComment->fontSize() %d\n",pixelSize);
    } else {
        font = g_defaultfont;
        pixelSize = g_defaultfont.pixelSize() < 0 ? font.pointSize()*qApp->desktop()->logicalDpiX()/72 : font.pixelSize();
        //    	printf("g_defaultfont.pixelSize() %d\n",pixelSize);
    }


    switch(nMode & 0xffff) {
    case DRAW_ON:
        col = QColor("black");
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
        col = QColor("red");
        break;
    }


    SSize drawSize = ::DrawString(
                         p,    	    	    //出力先
                         pComment->x1(),pComment->y1(),    	    	    //出力位置
                         pComment->text(),    	    	//出力文字列
                         col,    	    //出力色
                         font,
                         pixelSize,
                         //nMode,    	    	    //描画モード
                         false,    	    	    	//垂直フラグ
                         0x0,    	    	    //指定した出力位置が 0:文字列の前 1:文字列の後ろ 2:文字列の中央
                         //    	    	     0:文字列の下 4:文字列の上   8:文字列の中
                         m_viewScale,
                         m_viewScaleMul);
    pComment->setDrawSize(drawSize);


    /*


    pixelSize = pixelSize*m_viewScaleMul/m_viewScale;
    font.setPixelSize(pixelSize);

    p->setPen(col);
    //p->setRasterOp(op);
    p->setFont(font);



    int h,w,x,y,voffset;
    x = pComment->x1()*m_viewScaleMul/m_viewScale;
    y = pComment->y1()*m_viewScaleMul/m_viewScale;

    QRect rcText = p->boundingRect (x,y,0,0,Qt::AlignLeft|Qt::AlignBottom ,QString::fromLocal8Bit(pComment->text()));
    w = rcText.width();
    h = rcText.height();

    QFontMetrics fontMetrics(font);
    voffset = fontMetrics.descent()+1;
    w = w*m_viewScale/m_viewScaleMul;
    h = h*m_viewScale/m_viewScaleMul;
    pComment->setDrawSize(SSize(w,h));

    p->drawText(x,y-voffset,QString::fromLocal8Bit(pComment->text()));
    */
}



//ラベルの描画
void g_drawLabel(QPainter* p,SXBSchObj* pObj,unsigned  nMode , int m_viewScale,int m_viewScaleMul)
{
    if(pObj->id() != SXBSchObj::ID_LABEL) return;

    SXBSchLabel* pLabel =(SXBSchLabel*)pObj;

    QColor col;

    switch(nMode & 0xffff) {
    case DRAW_ON:
        col = QColor("black");
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
        col = QColor("red");
        break;
    }
    //p->setRasterOp(Qt::CopyROP);

    //int h,w,x,y;


    int x = pLabel->x1();
    int y = pLabel->y1();
    SSize sz;
    if(pLabel->horizontal()) {
        sz = g_SmallFont.DrawText(p,x,y-2,pLabel->text(),col,nMode,false,0,m_viewScale,m_viewScaleMul);
    } else {
        sz = g_SmallFont.DrawText(p,x-2,y,pLabel->text(),col,nMode,true,0,m_viewScale,m_viewScaleMul);
    }
    pLabel->setDrawSize(sz);
}


//タグの描画
void g_drawTag(QPainter* p,SXBSchObj* pObj,unsigned  nMode,int m_viewScale,int m_viewScaleMul)
{
    if(pObj->id() != SXBSchObj::ID_TAG) return;

    SXBSchTag* pTag =(SXBSchTag*)pObj;

    QColor colText;
    QColor colFrame;

    //Qt::RasterOp op = Qt::CopyROP;


    switch(nMode & 0xffff) {
    case DRAW_ON:
        colText = QColor("black");
        colFrame = QColor("#BE090A");
        break;
    case DRAW_MONO:
        colText = Qt::color1;
        colFrame = Qt::color1;
        break;
    case DRAW_OFF:
        colText = QColor("white");
        colFrame = QColor("white");
        break;
    case DRAW_DARK:
        colText = QColor("orange");
        colFrame = QColor("orange");
        break;
    case DRAW_TEMP:
        colText = QColor("red");
        colFrame = QColor("red");
        break;
    }
    //p->setRasterOp(op);

    int framex,framey,frameh,framew,textx,texty,textw;

    framex = pTag->x1()*m_viewScaleMul/m_viewScale;
    framey = pTag->y1()*m_viewScaleMul/m_viewScale;
    //hとwは水平のときの値。垂直なら入れ替わる。
    frameh = 10*m_viewScaleMul/m_viewScale/2;
    framew = pTag->width()*m_viewScaleMul/m_viewScale;

    textw = strlen(pTag->text())*8;

    //文字の描画
    if(pTag->horizontal()) {
        if(m_viewScale <=m_viewScaleMul) {
            //textx = pTag->x1()+( pTag->width() -textw)/2;
            textx = pTag->x1()+  pTag->width()/2;
            texty = pTag->y1()+3;
            g_SmallFont.DrawText(p,textx,texty,pTag->text(),colText,nMode,false,2,m_viewScale,m_viewScaleMul);
        }
    } else {
        if(m_viewScale <= m_viewScaleMul) {
            textx = pTag->x1()+3;
            //texty = pTag->y1()-( pTag->width() -textw)/2;
            texty = pTag->y1()-  pTag->width()/2;
            g_SmallFont.DrawText(p,textx,texty,pTag->text(),colText,nMode,true,2,m_viewScale,m_viewScaleMul);
        }
    }

    //枠の描画
    QPolygon ptarray(6);
    int pointCount = 0;
    int tagtype = pTag->tagType();
    //スケールが縮小のときは、タグは常に長方形で描画する。
    if(m_viewScale > m_viewScaleMul) {
        tagtype =SXBSchTag::TAG_SQ;
    }
    if(pTag->horizontal()) {
        switch(tagtype) {
        case SXBSchTag::TAG_SQ:
            pointCount = 4;
            ptarray.setPoint(0,framex,    	    framey-frameh);
            ptarray.setPoint(1,framex + framew,    framey-frameh);
            ptarray.setPoint(2,framex + framew,    framey+frameh);
            ptarray.setPoint(3,framex,    	    framey+frameh);
            break;
        case SXBSchTag::TAG_BI:
            pointCount = 6;
            ptarray.setPoint(0,framex,    	    	    framey    	 );
            ptarray.setPoint(1,framex + frameh,    	    framey-frameh);
            ptarray.setPoint(2,framex + framew - frameh,framey-frameh);
            ptarray.setPoint(3,framex + framew,    	    framey    	 );
            ptarray.setPoint(4,framex + framew - frameh,framey+frameh);
            ptarray.setPoint(5,framex + frameh,    	    framey+frameh);
            break;
        case SXBSchTag::TAG_LU:
            pointCount = 5;
            ptarray.setPoint(0,framex,    	    	    framey    	 );
            ptarray.setPoint(1,framex + frameh,    	    framey-frameh);
            ptarray.setPoint(2,framex + framew,    	    framey-frameh);
            ptarray.setPoint(3,framex + framew,    	    framey+frameh);
            ptarray.setPoint(4,framex + frameh,    	    framey+frameh);
            break;
        case SXBSchTag::TAG_RD:
            pointCount = 5;
            ptarray.setPoint(0,framex,    	    	    framey-frameh);
            ptarray.setPoint(1,framex + framew - frameh,framey-frameh);
            ptarray.setPoint(2,framex + framew,    	    framey    	 );
            ptarray.setPoint(3,framex + framew - frameh,framey+frameh);
            ptarray.setPoint(4,framex,    	    	    framey+frameh);
            break;
        }
    } else {
        switch(tagtype) {
        case SXBSchTag::TAG_SQ:
            pointCount = 4;
            ptarray.setPoint(0,framex - frameh,    framey    	 );
            ptarray.setPoint(1,framex - frameh,    framey-framew);
            ptarray.setPoint(2,framex + frameh,    framey-framew);
            ptarray.setPoint(3,framex + frameh,    framey    	 );
            break;
        case SXBSchTag::TAG_BI:
            pointCount = 6;
            ptarray.setPoint(0,framex,    	    framey    	    	    );
            ptarray.setPoint(1,framex - frameh,    framey -frameh    	    );
            ptarray.setPoint(2,framex - frameh,    framey -framew + frameh    );
            ptarray.setPoint(3,framex,    	    framey -framew    	    );
            ptarray.setPoint(4,framex + frameh,    framey -framew + frameh    );
            ptarray.setPoint(5,framex + frameh,    framey -frameh    	    );
            break;
        case SXBSchTag::TAG_LU:
            pointCount = 5;
            ptarray.setPoint(0,framex - frameh,    framey    	    	    );
            ptarray.setPoint(1,framex - frameh,    framey -framew + frameh    );
            ptarray.setPoint(2,framex,    	    framey -framew    	    );
            ptarray.setPoint(3,framex + frameh,    framey -framew + frameh    );
            ptarray.setPoint(4,framex + frameh,    framey    	    	    );
            break;
        case SXBSchTag::TAG_RD:
            pointCount = 5;
            ptarray.setPoint(0,framex,    	    framey    	    );
            ptarray.setPoint(1,framex - frameh,    framey -frameh    );
            ptarray.setPoint(2,framex - frameh,    framey-framew    );
            ptarray.setPoint(3,framex + frameh,    framey-framew    );
            ptarray.setPoint(4,framex + frameh,    framey -frameh    );
            break;
        }
    }

    int width = 1*m_viewScaleMul/m_viewScale;
    if(width < 1) width = 1;
    //p->setRasterOp(op);
    QPen pen(colFrame,width,Qt::SolidLine);
    p->setPen(pen);
    QBrush brush(Qt::NoBrush);
    p->setBrush(brush);
    //p->drawPolygon(ptarray,false,0,pointCount);
    p->drawPolygon(ptarray.constData(),pointCount,Qt::OddEvenFill);
}


//線の描画
void g_drawLineData(QPainter* p,SXBSchObj* pObj,unsigned  nMode,int m_viewScale,int m_viewScaleMul)
{
    int width = 1*m_viewScaleMul/m_viewScale;

    Qt::PenStyle style = Qt::SolidLine;
    QColor col;
    //Qt::RasterOp op = Qt::CopyROP;

    switch(pObj->id()) {
    case SXBSchObj::ID_WIRE:
        col = QColor("#028200");
        break;

    case SXBSchObj::ID_ENTRY:
        col = QColor("#028200");
        break;
    case SXBSchObj::ID_DASH:
#ifdef Q_WS_MACX
        if (nMode & DRAW_FOR_PRINT) {
            style = Qt::DashDotLine;
        } else {
            style = Qt::DashLine;
        }
        if (nMode & DRAW_FOR_PRINT) {
            //printf("Width:%d\n",width);
            width /= 10;
        }

#else
        style = Qt::DotLine;
#endif
        col = QColor("#1C00C0");
        break;
    case SXBSchObj::ID_BUS:
    case SXBSchObj::ID_BENTRY:
        //style = Qt::DotLine;
        width = 3*m_viewScaleMul/m_viewScale;
        col = QColor("#1E7BC0");
        break;
    default:
        return;
    }


    switch(nMode & 0xffff) {
    case DRAW_ON:
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
        col = QColor("red");
        break;

    }

    if(width < 1) width = 1;

    //p->setRasterOp(op);
    int x1 = pObj->x1()*m_viewScaleMul/m_viewScale;
    int y1 = pObj->y1()*m_viewScaleMul/m_viewScale;
    int x2 = pObj->x2()*m_viewScaleMul/m_viewScale;
    int y2 = pObj->y2()*m_viewScaleMul/m_viewScale;

    QPen pen(col,width,style,Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen);
    p->drawLine(x1,y1,x2,y2);
}

void g_drawXBSchObj(QPainter*p,SXBSchObj* pObj,unsigned  nMode,SRect* prcClip,int m_viewScale,int m_viewScaleMul)
{
    if(!pObj) return;
    switch(pObj->id()) {
    case SXBSchObj::ID_JUNCTION:
        g_drawJunction(p,pObj,nMode, m_viewScale,m_viewScaleMul);
        break;
    case SXBSchObj::ID_LABEL:
        g_drawLabel(p,pObj,nMode, m_viewScale,m_viewScaleMul);
        break;
    case SXBSchObj::ID_COMMENT:
        g_drawComment(p,pObj,nMode, m_viewScale,m_viewScaleMul);
        break;
    case SXBSchObj::ID_TAG:
        g_drawTag(p,pObj,nMode, m_viewScale,m_viewScaleMul);
        break;
    case SXBSchObj::ID_ENTRY:
    case SXBSchObj::ID_BENTRY:
    case SXBSchObj::ID_WIRE:
    case SXBSchObj::ID_DASH:
    case SXBSchObj::ID_BUS:
        g_drawLineData(p,pObj,nMode, m_viewScale,m_viewScaleMul);
        break;
    case SXBSchObj::ID_COMPONENT:
        g_drawComponent(p,(SXBSchComponent*)pObj,nMode,prcClip, m_viewScale,m_viewScaleMul);
        break;
    default:
        break;
    }
}


