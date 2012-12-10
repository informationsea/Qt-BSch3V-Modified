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
#include <string.h>
#include <stdio.h>
#include "xbsch.h"
#include "xbschpattern.h"
//#include "xbschview.h"

class SBmpWorkArea
{
private:
    unsigned char* bitreverse_table;
    void init_bitreverse_table();
public:
    SBmpWorkArea();
    ~SBmpWorkArea();
    unsigned char* buff0;
    unsigned char* buff1;
    unsigned char bitreverse(unsigned char c) const;
};

SBmpWorkArea::SBmpWorkArea()
{
    bitreverse_table = (unsigned char*) new char[256];
    init_bitreverse_table();
    buff0 = (unsigned char*) new char[0x10000];
    buff1 = (unsigned char*) new char[0x10000];
}

SBmpWorkArea::~SBmpWorkArea()
{
    delete[]bitreverse_table;
    delete[]buff0;
    delete[]buff1;
}

void SBmpWorkArea::init_bitreverse_table()
{
    for(int n =0; n < 256; n++) {
        unsigned char sbit = 0x01;
        unsigned char dbit = 0x80;
        unsigned char d = 0;
        for(int i = 0; i < 8; i++) {
            if(n & sbit) d |= dbit;
            sbit <<= 1;
            dbit >>= 1;
        }
        bitreverse_table[n]=d;
    }
}

unsigned char SBmpWorkArea::bitreverse(unsigned char c) const
{
    unsigned char rc = bitreverse_table[c];
    return rc;
}


//////////////////////////////////////////////////////////////////////
//グローバル変数
static SBmpWorkArea bmpworkarea;

//  上下反転、左右反転、上下左右反転は高速に実行できる。
//  90度回転はビット操作を伴うので時間がかかる。
//
//    (0) 0      (1) 90     (2) 180    (3) 270
//     →         ←         ←         →
//    ●Ｔ○     ○Ｌ●     ○Ｂ○     ○Ｒ○
//  ↓Ｌ↑Ｒ↓ ↓Ｂ→Ｔ↓ ↑Ｒ↓Ｌ↑ ↑Ｔ←Ｂ↑
//    ○Ｂ○     ○Ｒ○     ○Ｔ●     ●Ｌ○
//     →         ←         ←         →
//
//    (4) M0     (5) M90    (6) M180   (7) M270
//     ←         →         →         ←
//    ○Ｔ●     ●Ｌ○     ○Ｂ○     ○Ｒ○
//  ↓Ｒ↑Ｌ↓ ↓Ｔ←Ｂ↓ ↑Ｌ↓Ｒ↑ ↑Ｂ←Ｔ↑
//    ○Ｂ○     ○Ｒ○     ●Ｔ○     ○Ｌ●
//     ←         →         →         ←
//
//    1 は 0 を90度回転したもの
//    2 は 0 を                    上下左右反転したもの
//    3 は 0 を270度回転したもの
//    4 は 0 を                        左右反転したもの
//    5 は 0 を90度回転したものを      左右反転したもの
//    6 は 0 を                    上下    反転したもの
//    7 は 0 を90度回転したものを  上下    反転したもの


//ビットマップを上下反転コピー
//ソースとデスティネーションが同じでもOK
void g_bmp_vmirror(int sx,int sy,const unsigned char *s,unsigned char *d)
{
    int xbyte=((sx+15)/16)*2;
    const unsigned char* sUpper = s;
    const unsigned char* sLower = s+xbyte*(sy-1);
    unsigned char* dUpper = d;
    unsigned char* dLower = d+xbyte*(sy-1);
    unsigned char c;
    int ymax = (sy+1)/2;
    for(int y = 0; y < ymax; y++) {
        for(int i = 0; i < xbyte; i++) {
            c    	    =sUpper[i];
            dUpper[i]    =sLower[i];
            dLower[i]    =c;
        }
        sUpper += xbyte;
        sLower -= xbyte;
        dUpper += xbyte;
        dLower -= xbyte;
    }
}


//ビットマップを左右反転コピー
//反転した結果、ビットマップはワードアラインメント右詰めになる
//ソースとデスティネーションが同じでもOK
void g_bmp_hmirror(int sx,int sy,const unsigned char *s,unsigned char *d)
{
    int xbyte=((sx+15)/16)*2;
    const unsigned char* sTop = s;
    unsigned char* dTop = d;
    unsigned char c;
    int xmax = xbyte/2;
    for(int y = 0; y < sy; y++) {
        int i,j;
        for(i = 0,j = xbyte-1; i < xmax; i++,j--) {
            c    	    =sTop[i];
            dTop[i]    	=bmpworkarea.bitreverse(sTop[j]);
            dTop[j]    	=bmpworkarea.bitreverse(c);
        }
        sTop += xbyte;
        dTop += xbyte;
    }
}

//ビットマップを上下左右反転コピー
//反転した結果、ビットマップはワードアラインメント右詰めになる
//ソースとデスティネーションが同じでもOK
void g_bmp_vhmirror(int sx,int sy,const unsigned char *s,unsigned char *d)
{
    int xbyte=((sx+15)/16)*2;
    const unsigned char* sUpper = s;
    const unsigned char* sLower = s+xbyte*(sy-1);
    unsigned char* dUpper = d;
    unsigned char* dLower = d+xbyte*(sy-1);
    unsigned char c;
    int ymax = (sy+1)/2;
    for(int y = 0; y < ymax; y++) {
        int i,j;
        for(i = 0,j = xbyte-1; i < xbyte; i++,j--) {
            c    	    =sUpper[i];
            dUpper[i]    =bmpworkarea.bitreverse(sLower[j]);
            dLower[j]    =bmpworkarea.bitreverse(c);
        }
        sUpper += xbyte;
        sLower -= xbyte;
        dUpper += xbyte;
        dLower -= xbyte;
    }
}


//ビットマップを90度回転コピー
//ソースを左下から縦スキャン、デスティネーションを最上ラインから横スキャン
//                 →
//     ●○○    ●●●
//   ↑●○○    ○○○
//     ●○○    ○○○
//
void g_bmp_rot90(int sx,int sy,const unsigned char *s,int& dx,int& dy,unsigned char *d)
{
    dy = sx;
    dx = sy;
    int sxbyte=((sx+15)/16)*2;
    int dxbyte=((dx+15)/16)*2;
    int x,y;
    unsigned char* pch = d;
    for(y = 0; y < dy; y++) {
        for(x = 0; x < dxbyte; x++) {
            *pch++=0;
        }
    }


    unsigned char bitSrc,bitDst;
    const unsigned char* pSrcTop = s + sxbyte*(sy-1);    //左下のバイトを指す
    unsigned char* pDstTop = d;    	    	    	//左上のバイトを指す
    bitSrc = 0x80;
    for(x = 0; x < sx; x++) {
        const unsigned char* pSrc = pSrcTop;
        unsigned char* pDst = pDstTop;
        *pDst = 0;
        bitDst = 0x80;
        for(y = 0; y < sy; y++) {
            if(bitDst == 0x80) *pDst = 0;    	//初期化
            if(*pSrc & bitSrc) *pDst |= bitDst;
            pSrc -= sxbyte;    	//ソースは1列上に移動
            if(bitDst == 0x01) {    //デスティネーションは左のビットに移動
                bitDst = 0x80;    //隣のバイトに移るときは
                pDst++;
                //    	    	*pDst = 0;    	//移った先のバイトを初期化する
            } else {
                bitDst >>= 1;
            }
        }
        //１列が終わったら
        if(bitSrc == 0x01) {    	//ソースを隣のビットに移動
            bitSrc = 0x80;
            pSrcTop++;
        } else {
            bitSrc >>= 1;
        }
        pDstTop += dxbyte;    	//デスティネーションを次の行に移動
    }
}



//ビットマップを270度回転コピー
//ソースを左上から縦スキャン、デスティネーションを左下から横スキャン
//
//     ●○○    ○○○
//   ↓●○○    ○○○
//     ●○○    ●●●
//                 →
void g_bmp_rot270(int sx,int sy,const unsigned char *s,int& dx,int& dy,unsigned char *d)
{
    dy = sx;
    dx = sy;
    int sxbyte=((sx+15)/16)*2;
    int dxbyte=((dx+15)/16)*2;
    int x,y;
    unsigned char* pch = d;
    for(y = 0; y < dy; y++) {
        for(x = 0; x < dxbyte; x++) {
            *pch++=0;
        }
    }


    unsigned char bitSrc,bitDst;
    const unsigned char* pSrcTop = s;    //左上のバイトを指す
    unsigned char* pDstTop = d+dxbyte*(dy-1);//左下のバイトを指す
    bitSrc = 0x80;
    for(x = 0; x < sx; x++) {
        const unsigned char* pSrc = pSrcTop;
        unsigned char* pDst = pDstTop;
        *pDst = 0;
        bitDst = 0x80;
        for(y = 0; y < sy; y++) {
            if(bitDst == 0x80) *pDst = 0;    	//初期化
            if(*pSrc & bitSrc) *pDst |= bitDst;
            pSrc += sxbyte;    	//ソースは1列下に移動
            if(bitDst == 0x01) {    //デスティネーションは右のビットに移動
                bitDst = 0x80;    //隣のバイトに移るときは
                pDst++;
            } else {
                bitDst >>= 1;
            }
        }
        //１列が終わったら
        if(bitSrc == 0x01) {    	//ソースを隣のビットに移動
            bitSrc = 0x80;
            pSrcTop++;
        } else {
            bitSrc >>= 1;
        }
        pDstTop -= dxbyte;    	//デスティネーションを次の行に移動
    }
}

void g_put_bmp(
    QPainter* pPainter,    	    //出力先
    int x,
    int y,    	    	    	//出力位置
    const unsigned char* bitmap,    	    //出力するビットマップ(ワードアラインメント モノクロ)
    int w,    	    	    	//ピクセル幅
    int h,    	    	    	//ピクセル高
    int dir,    	    	    //向き
    int scale,    	    	    //スケール 1,2,5のどれか
    int scaleMul,    	    	    //スケール 1,2,5のどれか
    const QColor& color,    	//出力色
    unsigned  nMode    	    	//描画モード
)
{
    const unsigned char* srcbmp;
    QColor col;
    srcbmp = bitmap;

    /*
    switch(scale){
    case 1:
    srcbmp = bitmap;
    break;
    case 2:
    g_bmp_50percent(w,h,bitmap,w,h,bmpworkarea.buff1);
    srcbmp = bmpworkarea.buff1;
    break;
    case 5:
    g_bmp_20percent(w,h,bitmap,w,h,bmpworkarea.buff1);
    srcbmp = bmpworkarea.buff1;
    break;
    default:
    return;
    }
    */
    switch(dir) {
    case 0:
        break;
    case 1:    // 1 は 0 を90度回転したもの
        g_bmp_rot90(w,h,srcbmp,w,h,bmpworkarea.buff0);
        srcbmp = bmpworkarea.buff0;
        break;
    case 2: // 2 は 0 を上下左右反転したもの
        g_bmp_vhmirror(w,h,srcbmp,bmpworkarea.buff0);
        srcbmp = bmpworkarea.buff0;
        break;
    case 3: // 3 は 0 を270度回転したもの
        //    	g_bmp_rot90(w,h,srcbmp,w,h,bmpworkarea.buff0);
        //    	g_bmp_vhmirror(w,h,bmpworkarea.buff0,bmpworkarea.buff0);
        g_bmp_rot270(w,h,srcbmp,w,h,bmpworkarea.buff0);
        srcbmp = bmpworkarea.buff0;
        break;
    case 4: // 4 は 0 を左右反転したもの
        g_bmp_hmirror(w,h,srcbmp,bmpworkarea.buff0);
        srcbmp = bmpworkarea.buff0;
        break;
    case 5: // 5 は 0 を90度回転したものを左右反転したもの
        g_bmp_rot90(w,h,srcbmp,w,h,bmpworkarea.buff0);
        g_bmp_hmirror(w,h,bmpworkarea.buff0,bmpworkarea.buff0);
        srcbmp = bmpworkarea.buff0;
        break;
    case 6: // 6 は 0 を上下反転したもの
        g_bmp_vmirror(w,h,srcbmp,bmpworkarea.buff0);
        srcbmp = bmpworkarea.buff0;
        break;
    case 7: // 7 は 0 を90度回転したものを上下反転したもの
        g_bmp_rot90(w,h,srcbmp,w,h,bmpworkarea.buff0);
        g_bmp_vmirror(w,h,bmpworkarea.buff0,bmpworkarea.buff0);
        srcbmp = bmpworkarea.buff0;
        break;
    default:
        return;
    }

    int buffPixelWidth = ((w+15)/16)*16;
    if(dir == 2 || dir == 4 ||dir == 5) {
        x += (w-buffPixelWidth);
    }



    switch(nMode & 0xffff) {
    case DRAW_ON:
        col = color;
        break;

    case DRAW_OFF:
        col =QColor("white");
        break;

    case DRAW_MONO:
        col = QColor("black");
        break;

    case DRAW_TEMP:
        col = color;
        break;

    }

    x = x*scaleMul / scale - scaleMul / (scale*2);
    y = y*scaleMul / scale - scaleMul / (scale*2);
    double mag = 1.0*scaleMul/scale;
#ifdef Q_WS_MACX
    //QBitmap bmp(buffPixelWidth,h,srcbmp,false);
    QBitmap bmp = QBitmap::fromData(QSize(buffPixelWidth,h),srcbmp);
    QRegion region(bmp);
    pPainter->save();
    pPainter->translate(x,y);
    pPainter->scale(mag,mag);
    pPainter->setClipRegion(region);
    pPainter->fillRect(0,0,buffPixelWidth,h,QBrush (col));
    pPainter->restore();
#else
    //QBitmap bmp(buffPixelWidth,h,srcbmp,false);
    QBitmap bmp = QBitmap::fromData(QSize(buffPixelWidth,h),srcbmp);
    QPixmap Pmp(buffPixelWidth,h);
    Pmp.fill(col);
    Pmp.setMask(bmp);
    pPainter->save();
    pPainter->translate(x,y);
    pPainter->scale(mag,mag);
    pPainter->drawPixmap(0,0,Pmp);
    pPainter->restore();
#endif


    return;
}
