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

#include <stdio.h>
#include <assert.h>

#include <qcolor.h>
#include <qpoint.h>
#include <qsize.h>
#include <qrect.h>
#include <qpainter.h>
#include <qfont.h>
#include <qfontinfo.h>


#include <QDebug>

#include "xbschview.h"
#include "xbschdoc.h"
//#include "xbschpattern.h"
#include "xbsch.h"
#include "xbschobj.h"
#include "xbschjunc.h"
#include "xbschlabel.h"
#include "xbschcomment.h"
#include "xbschtag.h"
#include "xbschcomponent.h"
#include "drawcomponent.h"
#include "drawobject.h"
#include "xbschglobal.h"


//グリッドの描画
void SXBSchView::drawGrid(QPainter*p,const QRect& rcClip,const QColor& color)
{
    if(m_viewScale > m_viewScaleMul) return;
    int l,t,r,b,x,y,px,py;

    //printf("SXBSchView::drawGrid() %d,%d \n",rcClip.left(),rcClip.top());

    SSize size = m_pDoc->SheetSize();

    l=(rcClip.left()/10)*10;
    t=(rcClip.top()/10)*10;
    r = rcClip.right();
    b = rcClip.bottom();

    if(l < 20)l = 20;
    if(t < 20)t = 20;
    if(r>(size.w()-20)) r = size.w()-20;
    if(b>(size.h()-20)) b = size.h()-20;


    //p->setRasterOp(CopyROP);
    QPen pen(color);
    p->setPen(pen);
    for( x = l ; x <= r; x += 10) {
        px = x * m_viewScaleMul / m_viewScale;
        for( y = t; y <= b; y += 10) {
            py = y * m_viewScaleMul / m_viewScale;
            p->drawPoint( px , py );
        }
    }
}

//一時データのフレームの描画      	    	    	xbschdrawobject.cpp
void SXBSchView::drawTempDataFrame(QPainter*p)
{
    //    p->setRasterOp(Qt::XorROP);
    //    QPen pen(QColor("black"),1,DotLine);
    //    p->setPen(pen);

    QPen pen(QColor("gray"));
    p->setPen(pen);
    p->setBrush( Qt::NoBrush );
    QRect tempArea(m_pDoc->tempArea().l(),m_pDoc->tempArea().t(),m_pDoc->tempArea().w()+1,m_pDoc->tempArea().h()+1);
    p->drawRect(SheetToContents(tempArea));
}

//布線時の一時線の描画    	      	    	    	    	xbschdrawobject.cpp
void SXBSchView::drawTempLine(QPainter*p)
{
    int width = 1;
    if(m_toolmode == TOOL_BUS) {
        //if(m_viewScale == 1){
        width = 3*m_viewScaleMul/m_viewScale;
        //}
    } else {
        width = 1*m_viewScaleMul/m_viewScale;
    }
    if(width < 1) width = 1;

    Qt::PenStyle style = Qt::SolidLine;

    if(m_toolmode == TOOL_DASH) {
#ifdef Q_WS_MACX
        style = Qt::DashLine;
#else
        style = Qt::DotLine;
#endif
    }

    QPen pen(QColor("red"),width,style);
    p->setPen(pen);

    QPoint ptStart = m_ptLButtonDownAtSheet;
    if(m_bSnapGrid) ptStart = snapPoint(ptStart);
    ptStart = SheetToContents(ptStart);

    QPoint ptEnd = m_ptLastMoveAtSheet;
    if(m_bSnapGrid) ptEnd = snapPoint(ptEnd);
    ptEnd = SheetToContents(ptEnd);

    QPoint passPoint;
    if(m_nTempLineOrder >= 0) {    //水平線を先に描画する
        passPoint.setX(ptEnd.x());
        passPoint.setY(ptStart.y());
    } else {    	    	    	//垂直線を先に描画する
        passPoint.setX(ptStart.x());
        passPoint.setY(ptEnd.y());
    }
    p->drawLine(ptStart,passPoint);
    p->drawLine(passPoint,ptEnd);

    //    p->moveTo(ptStart);
    //    p->lineTo(passPoint);
    //    p->lineTo(ptEnd);

}

//選択枠の描画
void SXBSchView::drawSelectFrame(QPainter*p)//,const QColor& color)
{
    //    p->setRasterOp(Qt::XorROP);
    //    QPen pen(QColor("white"),1,DotLine);
    //    p->setPen(pen);

    QPen pen(QColor("gray"));
    p->setPen(pen);

    QPoint ptStart = SheetToContents(m_ptLButtonDownAtSheet);

    if(ptStart == m_ptLastMoveAtContents) return;    	//2003/07/27
    QRect rc(ptStart,m_ptLastMoveAtContents);
    rc = rc.normalized();
    //printf("drawSelectFrame  %d   %d\n",rc.width(),rc.height());
    p->drawRect(rc);
}

//リスト上のデータの描画 xbschdrawobject.cpp
void SXBSchView::drawMainXBSchObj(QPainter*p,unsigned  nMode,SRect* prcClip,bool darkenNonActive, int nScale, int nScaleMul)
{
    unsigned visibleLayer = m_pDoc->visibleLayer();
    int      editLayer    = (m_pDoc->editLayer() & 7);
    static unsigned layerTable[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
    if(darkenNonActive) {
        XBSchObjListIterator pObjIte = m_pDoc->m_listMain.begin();
        while(pObjIte != m_pDoc->m_listMain.end()) {
            if(    	((*pObjIte)->refObj() == NULL)    //参照オブジェクトが存在しないものを描画
                    &&    (layerTable[(*pObjIte)->layer() & 7] & visibleLayer)
                    &&    ((*pObjIte)->layer() != editLayer)) {
                if(prcClip == NULL || (*pObjIte)->qRedraw(*prcClip)) {
                    g_drawXBSchObj(p,*pObjIte,DRAW_DARK,prcClip, nScale, nScaleMul);
                }
            }
            pObjIte++;
        }
        pObjIte = m_pDoc->m_listMain.begin();
        while(pObjIte != m_pDoc->m_listMain.end()) {
            if(    	((*pObjIte)->refObj() == NULL)    //参照オブジェクトが存在しないものを描画
                    &&    (layerTable[(*pObjIte)->layer() & 7] & visibleLayer)
                    &&    ((*pObjIte)->layer() == editLayer)) {
                if(prcClip == NULL || (*pObjIte)->qRedraw(*prcClip)) {
                    g_drawXBSchObj(p,*pObjIte,nMode,prcClip, nScale, nScaleMul);
                }
            }
            pObjIte++;
        }
    } else {
        XBSchObjListIterator pObjIte = m_pDoc->m_listMain.begin();
        while(pObjIte != m_pDoc->m_listMain.end()) {
            if(    	((*pObjIte)->refObj() == NULL)    //参照オブジェクトが存在しないものを描画
                    &&    (layerTable[(*pObjIte)->layer() & 7] & visibleLayer) ) {
                if(prcClip == NULL || (*pObjIte)->qRedraw(*prcClip)) {
                    g_drawXBSchObj(p,*pObjIte,nMode,prcClip, nScale, nScaleMul);
                }
            }
            pObjIte++;
        }
    }

    //    XBSchObjListIterator pObjIte = m_pDoc->m_listMain.begin();
    //    while(pObjIte != m_pDoc->m_listMain.end()){
    //    	if((*pObjIte)->refObj() == NULL){    //参照オブジェクトが存在しないものを描画
    //    	    if(prcClip == NULL || (*pObjIte)->qRedraw(*prcClip)){
    //    	    	g_drawXBSchObj(p,*pObjIte,nMode,prcClip, nScale, nScaleMul);
    //    	    }
    //    	}
    //    	pObjIte++;
    //    }
}

void SXBSchView::drawTempXBSchObj(QPainter*p,unsigned  nMode,SRect* prcClip)
{
    XBSchObjListIterator pObjIte = m_pDoc->m_listTemp.begin();
    if(m_pDoc->m_listTemp.size() == 1) {
        nMode |= DRAW_SINGLE;
    }
    while(pObjIte != m_pDoc->m_listTemp.end()) {
        if(prcClip == NULL || (*pObjIte)->qRedraw(*prcClip)) {
            g_drawXBSchObj(p,*pObjIte,nMode,prcClip, m_viewScale, m_viewScaleMul);
        }
        pObjIte++;
    }
}




//カーソルにつきまとう図面オブジェクトの描画に関連するもの xbschdrawobject.cpp
void SXBSchView::drawCursorObj(QPainter*p)
{
    if(m_pCursorObj) {
        g_drawXBSchObj(p,m_pCursorObj,DRAW_TEMP,NULL, m_viewScale, m_viewScaleMul);
    }
}

//スクロールにともなうバッファの再描画
//渡される無効エリアの座標はコンテンツ座標系
void SXBSchView::updateViewBuffer(bool bForceUpdate)//int clipx,int clipy, int clipw, int cliph )
{


    bool update = bForceUpdate;
    int x = 0;
    int y = 0;
    int w = width();
    int h = height();
    int opCount = m_pDoc->opCount();

    if(m_oldContentsX != x || m_oldContentsY != y) {
        update = true;
        m_oldContentsX = x;
        m_oldContentsY = y;
    }

    if(m_oldOpCount    != opCount) {
        update = true;
        m_oldOpCount = opCount;
    }

    if(m_oldViewScale != m_viewScale || m_oldViewScaleMul != m_viewScaleMul) {
        update = true;
        m_oldViewScale = m_viewScale;
        m_oldViewScaleMul = m_viewScaleMul;
    }

    if(update)drawBuffer(x,y,x,y,w,h);
    m_applicationWindow->setWindowModified(m_pDoc->isDirty());
}

void SXBSchView::print(QPrinter*  pPrinter)
{
    m_pDoc->resetSelect();
    updateViewBuffer(true);
    QPainter paint;
    if(paint.begin(pPrinter ) ) {
        int dpi;
#ifdef Q_WS_MACX
        int dpix,dpiy;;
        dpix = pPrinter->logicalDpiX();
        dpiy = pPrinter->logicalDpiY();
        dpi=((dpix < dpiy) ? dpix : dpiy);
#else
        dpi = pPrinter->resolution();
#endif

        paint.setRenderHint(QPainter::Antialiasing, true);


        //    	QRect rcPaper = pPrinter->paperRect();
        //    printf("paperRect %d,%d,%d,%d\n",rcPaper.left(),rcPaper.top(),rcPaper.width(),rcPaper.height());
        QRect rcVp = paint.viewport();
        QRect rcPg = pPrinter->pageRect();
        //    printf("pageRect %d,%d,%d,%d\n",rcPg.left(),rcPg.top(),rcPg.width(),rcPg.height());
        //    	int orientation = pPrinter->orientation();
        //    printf("orientation %d\n",orientation);

        int vpWidth, vpHeight;
        int pgLeft, pgTop, pgWidth, pgHeight;

        vpWidth    =rcVp.width();
        vpHeight = rcVp.height();

        pgLeft    =rcPg.left();
        pgTop    =rcPg.top();
        pgWidth    =rcPg.width();
        pgHeight = rcPg.height();


        //印刷時に印刷の向きを変えてもpageRect()の返す値が変わらないQtのバグ(?)の対策
        if(    	(vpWidth > vpHeight && pgWidth < pgHeight)
                ||    (vpWidth < vpHeight && pgWidth > pgHeight)    ) {
            int swapn;
            swapn = pgLeft;
            pgLeft = pgTop;
            pgTop = swapn;

            swapn = pgWidth;
            pgWidth = pgHeight;
            pgHeight = swapn;
        }

        QSettings *settings = g_cfg.getSettings();

        int leftMargin = 15;
        int topMargin = 15;
        int rightMargin = 15;
        int bottomMargin = 15;

        settings->beginGroup("PrintOption");
        bool color = settings->value("Color",true).toBool();
        settings->endGroup();


        settings->beginGroup("PrintMargin");
        topMargin = settings->value("Top",15).toInt();
        bottomMargin = settings->value("Bottom",15).toInt();
        leftMargin = settings->value("Left",15).toInt();
        rightMargin = settings->value("Right",15).toInt();
        settings->endGroup();

        if      (topMargin    < 0)   topMargin = 0;
        else if (topMargin    > 50)  topMargin = 50;
        if      (bottomMargin < 0)   bottomMargin = 0;
        else if (bottomMargin > 50)  bottomMargin = 50;
        if      (leftMargin   < 0)   leftMargin = 0;
        else if (leftMargin   > 50)  leftMargin = 50;
        if      (rightMargin  < 0)   rightMargin = 0;
        else if (rightMargin  > 50)  rightMargin = 50;

        topMargin = dpi * 10 * topMargin / 254;
        bottomMargin = dpi * 10 * bottomMargin / 254;
        leftMargin = dpi * 10 * leftMargin / 254;
        rightMargin = dpi * 10 * rightMargin / 254;




        //    printf("SXBSchView::print() dpi:%d\n",dpi);

        paint.save();

        paint.setViewTransformEnabled (true);
        paint.resetMatrix();

        SSize size = m_pDoc->SheetSize();
        int w = size.w();
        int h = size.h();

        int dw = w*10;
        int dh = h*10;
        //    p->setWindow( 0,0, dw, dh );
        //    QRect rc = paint.viewport();

        int rightWidth = vpWidth-(pgLeft+pgWidth);
        if(rightWidth < 0) rightWidth = 0;
        int bottomWidth = vpHeight-(pgTop+pgHeight);
        if(bottomWidth < 0) bottomWidth = 0;

        leftMargin -= pgLeft;
        if(leftMargin < 0)leftMargin = 0;

        topMargin -= pgTop;
        if(topMargin < 0) topMargin = 0;

        rightMargin -= rightWidth;
        if(rightMargin < 0) rightMargin = 0;

        bottomMargin -= bottomWidth;
        if(bottomMargin < 0) bottomMargin = 0;

        int vw = pgWidth-(leftMargin+rightMargin);
        int vh = pgHeight-(topMargin+bottomMargin);


        double sheetRatio=(double)w / (double)h;
        double viewRatio=(double)vw / (double)vh;

        int newW;
        int newH;

        if(sheetRatio > viewRatio) {
            newW = vw;
            newH = (int)(vw / sheetRatio);
        } else {
            newH = vh;
            newW = (int)(vh *  sheetRatio);
        }
        //    printf("newW,H=%d,%d\n",newW,newH);
        //     p->setViewport(     rc.left() + (vw-newW)/2,
        //    	    	    rc.top() + (vh-newH)/2,
        //    	    	    newW, newH );
        //    QRect rcvp = p->viewport();
        //    printf("x,y,w,h  %d,%d,%d,%d\n",rcvp.x(),rcvp.y(),rcvp.width(),rcvp.height());

        paint.setViewport( leftMargin+ (vw-newW)/2,
                           topMargin + (vh-newH)/2,
                           newW, newH );
        paint.setWindow( 0,0, dw, dh );


        //    rcvp = p->viewport();
        //    printf("x,y,w,h  %d,%d,%d,%d\n",rcvp.x(),rcvp.y(),rcvp.width(),rcvp.height());


        QRect rcClip = QRect(0,0,dw,dh);
        SRect srcClip =SRect(0,0,w,h);

        paint.setBackground(Qt::white);
        paint.eraseRect(0,0,dw,dh);
        g_drawFrame(&paint,size,rcClip,Qt::black,1,10);
        drawMainXBSchObj(&paint,(color ? DRAW_ON : DRAW_MONO)|DRAW_FOR_PRINT,&srcClip,false,1,10);
        paint.restore();

        paint.end();
    }
}


//バッファの再描画
//渡される無効エリアの座標はコンテンツ座標系
void SXBSchView::drawBuffer(int x,int y,int clipx,int clipy, int clipw, int cliph )
{
    //    qDebug("SXBSchView::drawBuffer\n");
    QPainter painter;
    QRect rcClip = QRect(clipx,clipy,clipw,cliph);
    QRect rcClipSheet = ContentsToSheet(rcClip);
    SRect srcClipSheet =SRect(rcClipSheet.left(),rcClipSheet.top(),rcClipSheet.width(),rcClipSheet.height());
    //qDebug() << m_viewBuffer.size();
    painter.begin(&m_viewBuffer);
    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.translate(-x,-y);
    painter.eraseRect(clipx,clipy,clipw,cliph);

    //    painter.setClipping(true);
    clipx -= x;
    clipy -= y;
    //    painter.setClipRect(clipx,clipy,clipw,cliph);
    SSize size = m_pDoc->SheetSize();
    g_drawFrame(&painter,size,rcClip,QColor("#7E2020"),m_viewScale,m_viewScaleMul);
    if(m_bDisplayGrid) {
        drawGrid(&painter,rcClipSheet,QColor("#FC4343"));
    }
    drawMainXBSchObj(&painter,DRAW_ON,&srcClipSheet,m_fEditHighLight, m_viewScale,m_viewScaleMul);

    if((m_moveOption == MOVE_OPTION_COMPONENT_NAME)  || (m_moveOption == MOVE_OPTION_COMPONENT_NUM)) {
        int modeOption = 0;
        if(m_moveOption == MOVE_OPTION_COMPONENT_NAME) {
            modeOption = DRAW_INH_PARTNAME;
        } else {
            modeOption = DRAW_INH_PARTNUM;
        }
        drawTempXBSchObj(&painter,DRAW_TEMP|modeOption,&srcClipSheet);
    }

    painter.end();
}


//再描画
void SXBSchView::drawContents(QPainter*p,int clipx,int clipy,int clipw,int cliph)
{
    //    qDebug("SXBSchView::drawContents\n");

    updateViewBuffer();// clipx, clipy, clipw, cliph );

    QRect rcClip = QRect(clipx,clipy,clipw,cliph);
    QRect rcClipSheet = ContentsToSheet(rcClip);
    SRect srcClipSheet =SRect(rcClipSheet.left(),rcClipSheet.top(),rcClipSheet.width(),rcClipSheet.height());


    //int vclipx,vclipy;
    //    p->setClipping(true);
    //contentsToViewport ( clipx,clipy,vclipx,vclipy);
    //    p->setClipRect(vclipx,vclipy,clipw,cliph);

    p->drawPixmap(0,0,m_viewBuffer);
    //    p->drawPixmap(clipx,clipy,m_viewBuffer,vclipx,vclipy,clipw,cliph);
    //Qt4    	bitBlt(viewport(),clipx,clipy,&m_viewBuffer,clipx,clipy,clipw,cliph);//,Qt::CopyROP,false);




    //////////////////////////////////////////////////////////////
    /*
        p->eraseRect(clipx,clipy,clipw,cliph);

        SSize size = m_pDoc->SheetSize();
        g_drawFrame(p,size,rcClip,QColor("black"),m_viewScale,m_viewScaleMul);
        if(m_bDisplayGrid){
                drawGrid(p,rcClipSheet,QColor("green4"));
        }
        drawMainXBSchObj(p,DRAW_ON,&srcClipSheet, m_viewScale,m_viewScaleMul);

        if((m_moveOption == MOVE_OPTION_COMPONENT_NAME)  || (m_moveOption == MOVE_OPTION_COMPONENT_NUM)){
                int modeOption = 0;
                if(m_moveOption == MOVE_OPTION_COMPONENT_NAME){
                        modeOption = DRAW_INH_PARTNAME;
                }else{
                        modeOption = DRAW_INH_PARTNUM;
                }
                drawTempXBSchObj(p,DRAW_TEMP|modeOption,&srcClipSheet);
        }


    */
    //////////////////////////////////////////////////////////////
    //他のすべての描画が終わってからTempデータの描画を行う
    int modeOption = 0;
    if(m_pDoc->m_listTemp.size()>0) {
        if(m_bCompleteDrawTemp) {
            if(m_moveOption == MOVE_OPTION_COMPONENT_NAME) {
                modeOption = (DRAW_INH_PARTNUM | DRAW_INH_PARTBODY);
            } else if(m_moveOption == MOVE_OPTION_COMPONENT_NUM) {
                modeOption = (DRAW_INH_PARTNAME | DRAW_INH_PARTBODY);
            }

            drawTempXBSchObj(p,DRAW_TEMP|modeOption,&srcClipSheet);
        }
        if(m_clearTempRect) {
            m_pDoc->setupRcTempArea();
            m_pDoc->setupRotateRect();
            m_clearTempRect = false;
        }
        if(!modeOption) {
            drawTempDataFrame(p);
        }
    }


    if(m_bDrawSelectFrame) drawSelectFrame(p);
    if(m_bDrawTempLine)    drawTempLine(p);
    if(m_pCursorObj) drawCursorObj(p);
}
