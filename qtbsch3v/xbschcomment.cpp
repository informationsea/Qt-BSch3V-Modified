/****************************************************************************
    BSch3V schematic capture
    Copyright (C) 1997-2005 H.Okada

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
 ** XBSch図面要素ラベルクラスの実装
 ****************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include <string>
using namespace std;
#include "ce3io.h"
#include "xbschobj.h"
#include "xbschcomment.h"
#include <stdlib.h>


//コンストラクタ
SXBSchComment::SXBSchComment()
{
    m_drawSizeIsValid = false;
    m_fontSize          = 12;
    m_fontBold          = false;
    m_fontItalic      = false;
    m_fontUnderline   = false;
    m_fontStrikeOut   = false;
    m_strText          =    "";
    m_fontName          =    "";

}

bool SXBSchComment::readCe3(SReadCE3& rce3,const SPoint* pptOrigin,SXBSchDoc*)
{
    string str("");
    bool bInitX1 = false;
    bool bInitY1 = false;
    bool bInitText = false;

    m_drawSizeIsValid = false;
    m_fontSize          = 12;
    m_fontBold          = false;
    m_fontItalic      = false;
    m_fontUnderline   = false;
    m_fontStrikeOut   = false;

    while(1) {
        if(rce3.ReadRecord(str) == EOF) return false;
        if(str[0] == '-') {
            if( (str != "-COMMENT") || !bInitX1 || !bInitY1 || !bInitText) {
                return false;
            } else {
                break;    //break while(1){ }
            }
        } else if(str[0] == '+') {
            str[0]='-';
            if(rce3.SkipTo(str) == EOF) return false;
        } else {
            int n;
            int l = str.length();
            for(n = 0; n < l; n++) {
                if(str[n] == ':') break;
            }
            if(0 < n && n<(l-1)) {    // : で分けられたレコードである
                string var = str.substr(0,n);    	    	//先頭から:の手前まで
                string val = str.substr(n+1);
                int nParam;

                //#ifdef _MFCBSCH    //Qt-BSch doesn't support layer.
                if(var == "L") {
                    nParam = atoi(val.c_str());
                    if(isValidLayer(nParam)) {
                        m_Layer    = nParam;
                    }
                } else if(var == "X") {    //X座標
                    nParam = atoi(val.c_str());
                    if(pptOrigin) nParam += pptOrigin->x();
                    m_p1.setX(nParam);
                    bInitX1 = true;
                } else if(var == "Y") {    //Y座標
                    nParam = atoi(val.c_str());
                    if(pptOrigin) nParam += pptOrigin->y();
                    m_p1.setY(nParam);
                    bInitY1 = true;
                } else if(var == "S") {    //文字列
                    m_strText = val;
                    //if(m_strText.length()>0){
                    bInitText = true;
                    //}
                } else if(var == "FN") {    //フォント名
                    m_fontName = val;
                } else if(var == "FS") {    //フォントサイズ
                    nParam = atoi(val.c_str());
                    setFontSize(nParam);
                } else if(var == "FF") {    //フォントフラグ
                    int n = val.length();
                    for(int i = 0; i < n; i++) {
                        switch(val[i]) {
                        case 'B':
                            m_fontBold    	= true;
                            break;
                        case 'I':
                            m_fontItalic    	= true;
                            break;
                        case 'U':
                            m_fontUnderline    = true;
                            break;
                        case 'S':
                            m_fontStrikeOut    = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool SXBSchComment::writeCe3(SWriteCE3& wce3,const SPoint* pptOrigin)
{
    //    char sz[32];

    int x = m_p1.x();
    int y = m_p1.y();
    string str("");
    if(pptOrigin) {
        x -= pptOrigin->x();
        y -= pptOrigin->y();
    }
    wce3.WriteRecord("+COMMENT");
    wce3.WriteRecordInt("L",m_Layer);
    wce3.WriteRecordInt("X",x);
    wce3.WriteRecordInt("Y",y);
    //    sprintf(sz,"X:%d",x);
    //    wce3.WriteRecord(sz);
    //    sprintf(sz,"Y:%d",y);
    //    wce3.WriteRecord(sz);
    wce3.WriteRecordString("S",m_strText);
    //    str = "S:"; str = str + m_strText;
    //    wce3.WriteRecord(str);
    wce3.WriteRecordString("FN",m_fontName);
    //    str = "FN:"; str = str + m_fontName;
    //    wce3.WriteRecord(str);
    wce3.WriteRecordInt("FS",m_fontSize);
    //    sprintf(sz,"FS:%d",m_fontSize);
    //    wce3.WriteRecord(sz);
    str = "FF:";
    if(m_fontBold)      str = str + 'B';
    if(m_fontItalic)    str = str + 'I';
    if(m_fontUnderline)    str = str + 'U';
    if(m_fontStrikeOut) str = str + 'S';
    wce3.WriteRecord(str);
    wce3.WriteRecord("-COMMENT");
    wce3.WriteEOL();
    return true;
}


//複製を行う
SXBSchObj* SXBSchComment::duplicate()
{
    SXBSchObj* newObj =new SXBSchComment(*this);
    return newObj;
}

//描画が必要かどうかを返す
//描画サイズが無効のときは常にtrueを返す
bool SXBSchComment::qRedraw(const SRect& rc)
{
    if(!m_drawSizeIsValid) {
        return true;
    } else {
        return rc.intersect(area());
    }
}

//占有範囲を返す
//描画サイズが無効のときは描画サイズを20×10ピクセルと仮定して判定する。
SRect SXBSchComment::area()
{
    if(!m_drawSizeIsValid) {
        m_drawSize = SSize(20,10);
        //    	m_drawAreaVOffset = 10;
    }
    //    return SRect(m_p1.x(),m_p1.y()-m_drawAreaVOffset,m_drawSize.w(),m_drawSize.h());
    return SRect(m_p1.x(),m_p1.y()-m_drawSize.h()-COMMENT_Y_OFFSET,m_drawSize.w(),m_drawSize.h());
}


//点を指定して選択重要度を返す
int SXBSchComment::qSelectWeight(const SPoint& p)
{
    int nResult = PointAndRectMag(p,area());
    if(nResult) return (nResult | ON_OBJ);
    else    	return 0;
}

//範囲を指定して選択を行う
unsigned SXBSchComment::testSelection(const SRect& rc)
{
    if( rc.intersect(area()) ) {
        return SELECT_ALL;
    } else {
        return 0;
    }
}

//範囲を選択してドラッグ選択を行う
unsigned SXBSchComment::testSelectionForDrag(const SRect& rc)
{
    if( rc.intersect(area()) ) {
        return SELECT_ALL;
    } else {
        return 0;
    }
}


SRect SXBSchComment::rotateRect()
{
    return SRect(m_p1,m_p1);
}


//テキストの設定
void SXBSchComment::setText(const char* psz)
{
    if(psz == NULL) {
        m_strText = "";
    } else {
        m_strText = psz;
    }
    m_drawSizeIsValid = false;
}

//描画サイズの設定
//
//void SXBSchComment::setDrawSize(int vOffset,const SSize& size)
//{
//    m_drawAreaVOffset = vOffset;
//    m_drawSize = size;
//    m_drawSizeIsValid = true;
//}

void SXBSchComment::setDrawSize(const SSize& size)
{
    //    m_drawAreaVOffset = vOffset;
    m_drawSize = size;
    m_drawSizeIsValid = true;
}


//フォント名の設定
void SXBSchComment::setFontName(const char* psz)
{
    if(psz == NULL) {
        m_fontName = "";
    } else {
        m_fontName = psz;
    }
    m_drawSizeIsValid = false;
}

//フォントサイズの設定
void SXBSchComment::setFontSize(int fontSize)
{
    if(fontSize < 4) fontSize = 4;
    else if(fontSize > 80) fontSize = 80;
    m_fontSize = fontSize;
    m_drawSizeIsValid = false;
}

//ボールド設定
void SXBSchComment::setFontBold(bool istrue)
{
    m_fontBold = istrue;
    m_drawSizeIsValid = false;
}

//イタリック設定
void SXBSchComment::setFontItalic(bool istrue)
{
    m_fontItalic = istrue;
    m_drawSizeIsValid = false;
}

//アンダーライン設定
void SXBSchComment::setFontUnderline(bool istrue)
{
    m_fontUnderline = istrue;
    m_drawSizeIsValid = false;
}

//取り消し線設定
void SXBSchComment::setFontStrikeOut(bool istrue)
{
    m_fontStrikeOut = istrue;
    m_drawSizeIsValid = false;
}
