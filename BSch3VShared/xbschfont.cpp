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

#include <qpaintdevice.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qimage.h>
#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
#include <string.h>
#include <stdio.h>
#include "xbsch.h"
#include "xbschfont.h"
////////////////////////////////////////////////////////////////////////////////
// SSmallFont のコンストラクタ
SSmallFont::SSmallFont()
{
    m_updateFont = true;
    m_fontSize = 8;
}

////////////////////////////////////////////////////////////////////////////////
// SSmallFont のデストラクタ
SSmallFont::~SSmallFont()
{
}

//描画に使うフォントを設定する。
void  SSmallFont::SetFont(const QFont& font)
{
    //    QFontInfo fontInfo(font);

    QDesktopWidget *desktop = qApp->desktop();
    int dpi = desktop->logicalDpiX();
    m_fontSize = font.pixelSize() < 0 ? font.pointSize()*dpi/72 : font.pixelSize();
    m_font = font;
    m_updateFont = true;
}


SSize SSmallFont::DrawText(
    QPainter*     p,    	//出力先
    int           x,
    int           y,    	//出力位置
    const char*   str,    	//出力文字列
    const QColor& color,    	//出力色
    unsigned      nMode,        //描画モード
    bool          bVert,    	//垂直フラグ
    int           nPointPos,    //指定した出力位置が 0:文字列の前 1:文字列の後ろ 2:文字列の中央
    //    	    	     0:文字列の下 4:文字列の上   8:文字列の中
    int           nDiv,
    int           nMul,
    bool          bUseUpperLine)
{
    char buff[1026];
    bool upperLine = false;
    const char *pch;
    char ch;
    int nStrLen = strlen(str);
    if(nStrLen > 1024 || nStrLen < 0) {
        nStrLen = 1024;
    }
    int drawStrLen;
    if(bUseUpperLine) { //上線を使う場合は、文字の上位ビットを0にしておく。
        for(pch = str,drawStrLen = 0; *pch !='\0' && drawStrLen <nStrLen; pch++, drawStrLen++) {
            ch = *pch;
            if(ch & 0x80) {
                upperLine = true;
                ch &= 0x7f;
            }
            if(ch < 0x20 || ch > 0x7e) ch = ' ';
            buff[drawStrLen]=ch;
        }
    } else {
        for(pch = str,drawStrLen = 0; *pch !='\0' && drawStrLen < nStrLen; pch++, drawStrLen++) {
            buff[drawStrLen]=*pch;
        }
    }
    buff[drawStrLen]='\0';

    if(drawStrLen == 0) return  SSize(0,0);    //文字列の長さが0なら何もせずに戻る

    if(m_scale != nDiv || m_scaleMul != nMul || m_updateFont) {
        m_scale = nDiv;
        m_scaleMul = nMul;
        m_updateFont = false;
        int pixelsize = m_fontSize* m_scaleMul / m_scale;
        m_font .setPixelSize( pixelsize );
        QFontMetrics fontMetrics(m_font );
        m_upperPos = fontMetrics.overlinePos();
        m_vOffset = fontMetrics.descent()+1;
    }

    QColor col;
    switch(nMode & 0xffff) {
    case DRAW_ON:
        col = color;
        break;
    case DRAW_DARK:
        col = QColor("orange");
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
    }


    int h,w,linewidth;
    x = x* m_scaleMul / m_scale;
    y = y* m_scaleMul / m_scale;

    //- m_vOffset;

    linewidth = 1* m_scaleMul / m_scale;

    QPen pen(col,linewidth,Qt::SolidLine);

    p->setPen(pen);
    p->setFont(m_font );
    QRect rcText = p->boundingRect (x,y,0,0,Qt::AlignLeft|Qt::AlignBottom ,QString::fromLocal8Bit(buff));
    h = rcText.height();
    w = rcText.width();


    p->save();
    p->translate(x,y);
    if(bVert) {
        p->rotate(-90.0);
    }
    int drawX;
    switch(nPointPos&3) {
    case 0:
        drawX = 0;
        break;
    case 1:
        drawX=-w;
        break;
    default:
        drawX=-w/2;
    }

    int drawY;
    switch(nPointPos&12) {
    case 0:
        drawY = 0;
        break;
    case 4:
        drawY=(h-m_vOffset);
        break;
    default:
        drawY=(h-m_vOffset)/2;
    }


    QString outString = QString::fromLocal8Bit(buff);
    p->drawText(drawX,drawY, outString );

    if(bUseUpperLine) { //上線を使う場合は、文字の上位ビットを0にしておく。
        int    i = 0;
        int ly = drawY-m_upperPos;
        int begin, end;
        while(i < drawStrLen) {
            if(str[i]&0x80) {
                rcText = p->boundingRect (x,y,0,0,Qt::AlignLeft|Qt::AlignBottom ,QString::fromLocal8Bit(buff).left(i));
                //size = pDC->GetTextExtent(buff,i);
                begin = rcText.width();
                i++;
                while(i < drawStrLen && str[i]&0x80) {
                    i++;
                }
                rcText = p->boundingRect (x,y,0,0,Qt::AlignLeft|Qt::AlignBottom ,QString::fromLocal8Bit(buff).left(i));
                //size = pDC->GetTextExtent(buff,i);
                end = rcText.width();
                //TRACE("%d->%d\n",begin,end);
                p->drawLine(drawX+begin,ly,drawX+end,ly);
            }
            i++;
        }
    }

    p->restore();

    return SSize(w*m_scale / m_scaleMul, h*m_scale / m_scaleMul);
}


//////////////////////////////////////////////////////////////////////
//グローバル変数
SSmallFont g_SmallFont;    	//BSchフォント描画クラス

SSize DrawString(
    QPainter*     p,    	//出力先
    int           x,int y,    	//出力位置
    const char*   pstr,    	//出力文字列
    const QColor& color,    	//出力色
    QFont&        rFont,
    int           fontSize,
    //unsigned nMode,    	    	    //描画モード
    bool          bVert,    	//垂直フラグ
    int           nPointPos,    //指定した出力位置が 0:文字列の前 1:文字列の後ろ 2:文字列の中央
    //    	    	     0:文字列の下 4:文字列の上   8:文字列の中
    int           nDiv,
    int           nMul)
{
    int pixelsize = fontSize* nMul / nDiv;
    rFont.setPixelSize( pixelsize );
    QFontMetrics fontMetrics(rFont);
    int nOffset = fontMetrics.descent();
    int posX = x*nMul/nDiv;
    int posY = y*nMul/nDiv - nOffset;


    QPen pen(color);
    p->setPen(pen);
    p->setFont(rFont);


    QString outString = QString::fromLocal8Bit(pstr);
    QRect rcText = p->boundingRect (posX,posY,0,0,Qt::AlignLeft|Qt::AlignBottom,outString);
    int h = rcText.height();
    int w = rcText.width();

    p->save();
    p->translate(posX,posY);
    if(bVert) {
        p->rotate(-90.0);
    }
    int drawX,drawY;
    switch(nPointPos&3) {
    case 0:
        drawX = 0;
        break;
    case 1:
        drawX = -w;
        break;
    default:
        drawX = -w/2;
    }
    switch(nPointPos&12) {
    case 0:
        drawY = 0;
        break;
    case 4:
        drawY = h-nOffset;
        break;
    default:
        drawY = (h-nOffset)/2;
    }

    p->drawText(drawX,drawY,outString);
    p->restore();
    return SSize(w*nDiv/nMul,h*nDiv/nMul);    //図面座標でのサイズを返す

}
