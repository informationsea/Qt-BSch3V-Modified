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
** XBSch図面要素部品クラス
****************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <assert.h>
#include <list>
#include <string>
#include <stdlib.h>
using namespace std;
#include "ce3io.h"
#include "xbschglobal.h"
#include "xbschobj.h"
#include "xbschcomponent.h"

//コンストラクタ
SXBSchComponent::SXBSchComponent()
{
    m_name    	= "";    	    	//名前
    m_refnum    = "";    	    	//参照番号
    m_name_horiz = true;    	    //2005/04/08
    m_refnum_horiz = true;    	    //2005/04/08
    m_block    	= 0;    	    	//ブロック番号
    m_dir    	= COMPONENT_DIR_0;    //向き
    m_compInfoIndex = NULL;    	    //部品の情報へのポインタ
    m_useEmbeddedLib = true;
    m_pinltrb    =0;    	    	    //LTRBに10ピクセル長ピンが存在しているかどうかのフラグ
    m_orgname    = "";    	    	//冗長だけど、ライブラリから読み損ねた部品を残すためのもの
    m_note    	= "";
    resetRefnumPos();
    m_refnum_drawSizeIsValid = false;
    resetNamePos();
    m_name_drawSizeIsValid = false;
}

//デストラクタ
SXBSchComponent::~SXBSchComponent()
{
    if(m_useEmbeddedLib && m_compInfoIndex != NULL) {
        delete m_compInfoIndex->m_pCompInfo;
        delete m_compInfoIndex;
    }
}

//代入演算子
SXBSchComponent& SXBSchComponent:: operator=(const SXBSchComponent& comp)
{
    if(this != &comp) {
        if(m_useEmbeddedLib && m_compInfoIndex != NULL) {
            delete m_compInfoIndex->m_pCompInfo;
            delete m_compInfoIndex;
            m_compInfoIndex = NULL;
        }
        *((SXBSchObj*)this) = comp;
        m_name    	    = comp.m_name;    	    //名前
        m_name_pos    	= comp.m_name_pos;    	//名前の原点からのオフセット
        m_name_horiz    = comp.m_name_horiz;    //2005/04/08
        m_refnum    	= comp.m_refnum;    	//参照番号
        m_refnum_pos    = comp.m_refnum_pos;    //参照番号の原点からのオフセット
        m_refnum_horiz    = comp.m_refnum_horiz;    //2005/04/08
        m_block    	    = comp.m_block;    	    //ブロック番号
        m_dir    	    = comp.m_dir;    	    //向き
        m_pinltrb    	= comp.m_pinltrb;    	//LTRBに10ピクセル長ピンが存在しているかどうかのフラグ
        m_note    	    = comp.m_note;
        m_useEmbeddedLib = comp.m_useEmbeddedLib;
        m_compInfoIndex = NULL;
        m_refnum_drawSizeIsValid = comp.m_refnum_drawSizeIsValid;
        m_name_drawSizeIsValid = comp.m_name_drawSizeIsValid;
        m_name_drawSize = comp.m_name_drawSize;
        m_refnum_drawSize = comp.m_refnum_drawSize;

        if(!m_useEmbeddedLib) {
            m_compInfoIndex    = comp.m_compInfoIndex;    //部品の情報へのポインタ
        } else if(comp.m_compInfoIndex != NULL && comp.m_compInfoIndex->m_pCompInfo != NULL) {    //埋め込んだ部品の情報へのポインタ
            m_compInfoIndex = new SCompIndex;
            m_compInfoIndex->m_pCompInfo = new SCompInfo(*(comp.m_compInfoIndex->m_pCompInfo));
        }
        m_orgname    	= comp.m_orgname;    	//ライブラリからの呼び出し名
    }
    return *this;
}


SXBSchComponent::SXBSchComponent(const SXBSchComponent& comp)    	    	//コピーコンストラクタ
{
    m_compInfoIndex = NULL;    	    //部品の情報へのポインタ

    *this = comp;
}


//複製を行う
SXBSchObj* SXBSchComponent::duplicate()
{
    SXBSchComponent* pComp = new SXBSchComponent(*this);
    SXBSchObj* newObj = pComp;
    return newObj;
}

//m_dir変数を左右反転する
void SXBSchComponent::mirrorDir()
{
    assert(m_dir >= 0 && m_dir <= 7);
    m_dir &=0x07;    //特に意味ナシ
    m_dir ^=0x04;    //反転フラグの反転
    int pinL = (m_pinltrb & COMPONENT_HASLPIN);
    int pinR = (m_pinltrb & COMPONENT_HASRPIN);
    m_pinltrb &= (COMPONENT_HASTPIN | COMPONENT_HASBPIN);
    if(pinL)  m_pinltrb |= COMPONENT_HASRPIN;
    if(pinR)  m_pinltrb |= COMPONENT_HASLPIN;
}

//m_dir変数を右回転する
void SXBSchComponent::rotateDir()
{
    assert(m_dir >= 0 && m_dir <= 7);
    m_dir &=0x07;    //特に意味ナシ
    if(m_dir & 0x04) {
        m_dir -=1;
        m_dir |= 0x04;
    } else {
        m_dir +=1;
        m_dir &= 0x03;
    }
    m_pinltrb <<= 1;
    if(m_pinltrb & 0x10) {
        m_pinltrb |= 1;
    }
    m_pinltrb &= 0xf;
}


//参照番号の位置をデフォルトの位置に変更
void SXBSchComponent::resetRefnumPos()
{
    m_refnum_pos = SPoint(COMPONENT_DEFAULT_REFNUM_X,COMPONENT_DEFAULT_REFNUM_Y);
    m_refnum_horiz    = true;    //2005/04/08
}

//部品名の位置をデフォルトの位置に変更
void SXBSchComponent::resetNamePos()
{
    m_name_pos = SPoint(COMPONENT_DEFAULT_NAME_X,COMPONENT_DEFAULT_NAME_Y);
    m_name_horiz    = true;    //2005/04/08
}

//位置を動かす
void SXBSchComponent::move(int offsetx,int offsety)
{
    SPoint p(offsetx,offsety);
    m_p1 += p;
}




//X座標を指定して左右反転を行う
void SXBSchComponent::mirrorH(int x)
{
    int componentcenter;
    int componentwidth;
    if(m_compInfoIndex != NULL) {
        componentwidth = size().w();
        //    	componentwidth = (m_compInfoIndex->size().w()*10);
    } else {
        componentwidth = 20;
    }

    componentcenter = -componentwidth/2;


    int textcenter;    //文字列水平中央の位置の部品中央からの位置。
    int textX;

    int name_draw_width;
    int refnum_draw_width;

    if(m_name_drawSizeIsValid) {
        name_draw_width = m_name_drawSize.w();
    } else {
        name_draw_width = m_name.length()*8;
    }

    textcenter = m_name_pos.x() + name_draw_width/2 - componentcenter;    //現在位置
    textcenter = -textcenter;    //左右反転した位置
    textX = textcenter - name_draw_width/2 + componentcenter;
    m_name_pos.setX(textX);


    if(m_refnum_drawSizeIsValid) {
        refnum_draw_width = m_refnum_drawSize.w();
    } else {
        refnum_draw_width = m_refnum.length()*8;
    }


    textcenter = m_refnum_pos.x() + refnum_draw_width/2 - componentcenter;    //現在位置
    textcenter = -textcenter;    //左右反転した位置
    textX = textcenter - refnum_draw_width/2 + componentcenter;
    m_refnum_pos.setX(textX);

    componentcenter = m_p1.x() + componentcenter;
    componentcenter = x-(componentcenter-x);
    m_p1.setX(componentcenter + componentwidth/2);

    mirrorDir();
}

//左右反転を行う
void  SXBSchComponent::mirrorH()
{
    int componentHWidth;
    if(m_compInfoIndex != NULL) {
        componentHWidth = size().w();
        //    	componentHWidth = m_compInfoIndex->size().w()*10;
    } else {
        componentHWidth = 20;
    }
    int x = m_p1.x() - componentHWidth / 2;
    mirrorH(x);
}


SSize SXBSchComponent::size() const
{
    int w;
    int h;
    if(m_compInfoIndex == NULL) {
        w = 20;
        h = 20;
    } else {
        w = m_compInfoIndex->size().w()*10;
        h = m_compInfoIndex->size().h()*10;
        if(m_dir & 1) {    //横サイズと縦サイズが入れ替わる
            int n = w;
            w = h;
            h = n;
        }
    }
    return SSize(w,h);
}

//占有範囲を返す
SRect SXBSchComponent::area()
{
    SSize sizeComp = size();
    int r = m_p1.x();
    int b = m_p1.y();
    int l = r - sizeComp.w();
    int t = b - sizeComp.h();
    if(m_pinltrb & COMPONENT_HASLPIN) l -= PIN_LENGTH;
    if(m_pinltrb & COMPONENT_HASTPIN) t -= PIN_LENGTH;
    if(m_pinltrb & COMPONENT_HASRPIN) r += PIN_LENGTH;
    if(m_pinltrb & COMPONENT_HASBPIN) b += PIN_LENGTH;

    return SRect(SPoint(l,t),SPoint(r,b));
}

//占有範囲を返す
SRect SXBSchComponent::bodyArea()
{
    SSize sizeComp = size();
    int w = sizeComp.w();
    int h = sizeComp.h();
    return SRect(m_p1.x()-w,m_p1.y()-h,w,h);
}



//部品名の占有範囲を返す
SRect SXBSchComponent::nameArea() const
{
    int w,h;
    if(!m_name_drawSizeIsValid) {
        w = m_name.length()*8;
        h = 10;
    } else {
        w = m_name_drawSize.w();
        h = m_name_drawSize.h();
    }
    if(this->m_name_horiz) {
        return SRect(m_p1.x()+m_name_pos.x(),m_p1.y()+m_name_pos.y()-h,w,h);
    } else {
        return SRect(m_p1.x()+m_name_pos.x()-h,m_p1.y()+m_name_pos.y()-w,h,w);
    }
    //    return SRect(m_p1.x()+m_name_pos.x(),m_p1.y()+m_name_pos.y()-h,w,h);
}

//参照番号の占有範囲を返す
SRect SXBSchComponent::refnumArea() const
{
    int w,h;
    if(!m_refnum_drawSizeIsValid) {
        w = m_refnum.length()*8;
        h = 10;
    } else {
        w = m_refnum_drawSize.w();
        h = m_refnum_drawSize.h();
    }
    if(this->m_refnum_horiz) {
        return SRect(m_p1.x()+m_refnum_pos.x(),m_p1.y()+m_refnum_pos.y()-h,w,h);
    } else {
        return SRect(m_p1.x()+m_refnum_pos.x()-h,m_p1.y()+m_refnum_pos.y()-w,h,w);
    }
}

//部品名描画サイズの設定
void SXBSchComponent::setNameDrawSize(const SSize& size)
{
    m_name_drawSize = size;
    m_name_drawSizeIsValid = true;
}

//参照番号描画サイズの設定
void SXBSchComponent::setRefnumDrawSize(const SSize& size)
{
    m_refnum_drawSize = size;
    m_refnum_drawSizeIsValid = true;
}




//XY中心座標を指定して、回転を行う
void SXBSchComponent::rotate(const SPoint& p)
{
    resetRefnumPos();
    resetNamePos();
    SPoint center = bodyArea().center();
    int centerY = p.y() + (center.x() - p.x()) ;
    int centerX = p.x() - (center.y() - p.y()) ;
    rotateDir();
    SSize sizeComp = size();
    int x = centerX + sizeComp.w()/2;
    int y = centerY + sizeComp.h()/2;
    m_p1.setX(x);
    m_p1.setY(y);
}

//回転を行う
void  SXBSchComponent::rotate()
{
    resetRefnumPos();
    resetNamePos();
    rotateDir();
    //    rotate(bodyArea().center());
}

SRect SXBSchComponent::rotateRect()
{
    return bodyArea();
}


//範囲を指定して選択を行う
unsigned SXBSchComponent::testSelection(const SRect& rc)
{
    if( rc.intersect(bodyArea()) ) {
        return SELECT_ALL;
    } else {
        return 0;
    }
}


//範囲を選択してドラッグ選択を行う
unsigned SXBSchComponent::testSelectionForDrag(const SRect& rc)
{
    if( rc.intersect(bodyArea()) ) {
        return SELECT_ALL;
    } else {
        return 0;
    }
}

//点を指定して選択重要度を返す
int SXBSchComponent::qSelectWeight(const SPoint& p)
{
    int n;
    int nMax = 0;
    int nOnFlag = 0;
    n = PointAndRectMag(p,bodyArea());
    if(n) {
        nOnFlag |= ON_OBJ;
        if(n > nMax) nMax = n;
    }
    n = PointAndRectMag(p,nameArea());
    if(n) {
        nOnFlag |= ON_NAME;
        if(n > nMax) nMax = n;
    }
    n = PointAndRectMag(p,refnumArea());
    if(n) {
        nOnFlag |= ON_NUM;
        if(n > nMax) nMax = n;
    }
    return nMax|nOnFlag;
}

void SXBSchComponent::setName(const char* psz)
{
    m_name = psz;
    m_name_drawSizeIsValid = false;
}

void SXBSchComponent::setNameHorizontal(bool bHoriz)    //2005/04/09
{
    m_name_horiz = bHoriz;
    m_name_drawSizeIsValid = false;
}


void SXBSchComponent::setNote(const char* psz)
{
    m_note = psz;
}

void SXBSchComponent::setOrgName(const char* psz)
{
    m_orgname = psz;
}


void SXBSchComponent::moveNamePos(int offsetx,int offsety)
{
    m_name_pos.setX(m_name_pos.x()+offsetx);
    m_name_pos.setY(m_name_pos.y()+offsety);
}

void SXBSchComponent::setRefnum(const char* psz)
{
    m_refnum = psz;
    m_refnum_drawSizeIsValid = false;
}

void SXBSchComponent::setRefnumHorizontal(bool bHoriz)    //2005/04/09
{
    m_refnum_horiz = bHoriz;
    m_refnum_drawSizeIsValid = false;
}

void SXBSchComponent::moveRefnumPos(int offsetx,int offsety)
{
    m_refnum_pos.setX(m_refnum_pos.x()+offsetx);
    m_refnum_pos.setY(m_refnum_pos.y()+offsety);
}

void SXBSchComponent::setBlock(int n)
{
    int blockCount;
    if(m_compInfoIndex != NULL) {
        blockCount = m_compInfoIndex->block();
    } else {
        blockCount = 1;
    }
    if(n >= blockCount || n < 0) return;
    else m_block = n;
}


void SXBSchComponent::setDir(int n)
{
    n &= 7;
    m_dir = n;
}

bool SXBSchComponent::readEmbLibCe3(SReadCE3& rce3, SCompInfo* compInfo, string& name)
{
    SPtn* pPtn = NULL;
    bool bComp = false;
    string str("");
    //string name;
    string ptnName("");
    if(!compInfo) return false;
    while(1) {
        if(rce3.ReadRecord(str) == EOF) return false;
        if(str[0] == '-') {
            if( (str != "-BSCH3_LIB_V.1.0") || !bComp) {
                return false;
            } else {
                break;    //break while(1){ }
            }
        } else if(str == "+PTN") {
            if(!pPtn) {
                pPtn = new SPtn;
                if(pPtn->readCe3(rce3)) {
                    //パターン読み込み正常終了
                } else {
                    delete pPtn;
                    pPtn = NULL;
                }
            } else {
                if(rce3.SkipTo("-PTN") == EOF) return false;
            }
        } else if(str == "+COMP") {
            if(!bComp) {
                if(compInfo->readCe3(rce3,name,ptnName)) {
                    bComp = true;
                    //部品定義読み込み正常終了
                } else {
                    return false;
                }
            } else {
                if(rce3.SkipTo("-COMP") == EOF) return false;
            }
        }
    }
    if(pPtn) {
        compInfo->m_pPtn = pPtn;
    }

    return true;
}

bool SXBSchComponent::writeEmbLibCe3(SWriteCE3& wce3)
{
    // >>> 2005/01/30 部品情報が見つからなかった部品を保存する際に落ちる問題の対策
    if(!m_compInfoIndex) return false;
    if(!m_compInfoIndex->m_pCompInfo) return false;
    // <<< 2005/01/30
    static const char* defName = "EMB";
    const char* name = m_orgname.c_str();
    if(name == NULL || name[0] == '\0') {
        name = m_name.c_str();
    }
    if(name == NULL || name[0] == '\0') {
        name = defName;
    }
    wce3.WriteEOL();
    wce3.WriteRecord("+BSCH3_LIB_V.1.0");
    wce3.WriteEOL();
    m_compInfoIndex->m_pCompInfo->writeCe3(wce3,name);
    wce3.WriteRecord("-BSCH3_LIB_V.1.0");
    wce3.WriteEOL();
    return true;

}

bool SXBSchComponent::readCe3(SReadCE3& rce3,const SPoint* pptOrigin,SXBSchDoc*)
{
    string str("");
    string name("");
    bool bInitX1 = false;
    bool bInitY1 = false;
    bool bInitLib = false;
    bool bEmbLib = false;

    while(1) {
        if(rce3.ReadRecord(str) == EOF) return false;
        if(str[0] == '-') {
            if( (str != "-COMPONENT") || !bInitX1 || !bInitY1 || !bInitLib) {
                return false;
            } else {
                break;    //break while(1){ }
            }
        } else if(str == "+BSCH3_LIB_V.1.0") {
            if(!bEmbLib) {
                SCompInfo* pCompInfo = new SCompInfo;
                if(!readEmbLibCe3(rce3,pCompInfo,name)) {
                    delete pCompInfo;
                    return false;
                }
                if(!m_compInfoIndex) m_compInfoIndex = new SCompIndex;
                if(m_compInfoIndex->m_pCompInfo) delete m_compInfoIndex->m_pCompInfo;
                m_compInfoIndex->m_pCompInfo = pCompInfo;
                m_orgname = name;
                bEmbLib = true;
            } else {
                if(rce3.SkipTo("-BSCH3_LIB_V.1.0") == EOF) return false;
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
            if(0 < n && n < l) {    // : で分けられたレコードである
                string var = str.substr(0,n);    	    	//先頭から:の手前まで
                string val = str.substr(n+1);
                int nVal;
                // #ifdef _MFCBSCH    //Qt-BSch doesn't support layer.
                if(var == "L") {
                    nVal = atoi(val.c_str());
                    if(isValidLayer(nVal)) {
                        m_Layer    = nVal;
                    }
                } else
                    // #endif
                    if(var == "X") {
                        nVal = atoi(val.c_str());
                        if(pptOrigin) nVal += pptOrigin->x();
                        m_p1.setX(nVal);
                        bInitX1 = true;
                    } else if(var == "Y") {
                        nVal = atoi(val.c_str());
                        if(pptOrigin) nVal += pptOrigin->y();
                        m_p1.setY(nVal);
                        bInitY1 = true;
                    } else if(var == "LIB") {
                        m_orgname = val;
                        bInitLib = true;
                        //m_compInfoIndex = g_SearchComponentIndex(val.c_str(),NULL,NULL,NULL);
                        if(!bEmbLib) {
                            const SCompIndex* compInfoIndex = g_SearchComponentIndex(val.c_str(),NULL,NULL,NULL);
                            if(compInfoIndex) {
                                m_compInfoIndex = new SCompIndex;
                                m_compInfoIndex->m_pCompInfo = new SCompInfo(*(compInfoIndex->m_pCompInfo));
                            } else {
                                m_compInfoIndex = NULL;
                            }
                        }
                    } else if(var == "DIR") {
                        nVal = atoi(val.c_str());
                        if(nVal >= 0 && nVal <8) {
                            m_dir = nVal;
                        }
                    } else if(var == "BLK") {
                        if(m_compInfoIndex == NULL) {
                            m_block = 0;
                        } else {
                            nVal = atoi(val.c_str());
                            if(nVal >= 0 && nVal < m_compInfoIndex->block()) {
                                m_block = nVal;
                            } else {
                                m_block = 0;
                            }
                        }
                    } else if(var == "N") {
                        m_name = val;
                    } else if(var == "ND") {
                        nVal = atoi(val.c_str());
                        m_name_horiz=(nVal != 0);
                    } else if(var == "NX") {
                        nVal = atoi(val.c_str());
                        m_name_pos.setX(nVal);
                    } else if(var == "NY") {
                        nVal = atoi(val.c_str());
                        m_name_pos.setY(nVal);
                    } else if(var == "R") {
                        m_refnum = val;
                    } else if(var == "RD") {
                        nVal = atoi(val.c_str());
                        m_refnum_horiz=(nVal != 0);
                    } else if(var == "RX") {
                        nVal = atoi(val.c_str());
                        m_refnum_pos.setX(nVal);
                    } else if(var == "RY") {
                        nVal = atoi(val.c_str());
                        m_refnum_pos.setY(nVal);
                    } else if(var == "NOTE") {
                        m_note = val;
                    }
            }
        }
    }
    setPinLtrb();
    return true;
}


bool SXBSchComponent::writeCe3(SWriteCE3& wce3,const SPoint* pptOrigin)
{
    //    char sz[32];

    int x = m_p1.x();
    int y = m_p1.y();

    string str("");

    if(pptOrigin) {
        x -= pptOrigin->x();
        y -= pptOrigin->y();
    }

    wce3.WriteRecord("+COMPONENT");
    writeEmbLibCe3(wce3);
    wce3.WriteRecordInt("L",m_Layer);
    wce3.WriteRecordInt("X",x);
    wce3.WriteRecordInt("Y",y);


    wce3.WriteRecordString("LIB",m_orgname);

    wce3.WriteRecordInt("DIR",m_dir);

    wce3.WriteRecordInt("BLK",m_block);

    wce3.WriteRecordString("N",m_name);

    wce3.WriteRecordInt("ND",(m_name_horiz ? 1:0));

    wce3.WriteRecordInt("NX",m_name_pos.x());

    wce3.WriteRecordInt("NY",m_name_pos.y());

    wce3.WriteRecordString("R",m_refnum);

    wce3.WriteRecordInt("RD",(m_refnum_horiz ? 1:0));

    wce3.WriteRecordInt("RX",m_refnum_pos.x());

    wce3.WriteRecordInt("RY",m_refnum_pos.y());

    wce3.WriteRecordString("NOTE",m_note);


    wce3.WriteRecord("-COMPONENT");
    wce3.WriteEOL();
    return true;
}


void SXBSchComponent::setCompInfoIndex(const SCompIndex* pIndex)
{
    if(pIndex == NULL)return;

    if(m_compInfoIndex != NULL) {
        delete m_compInfoIndex->m_pCompInfo;
        delete m_compInfoIndex;
    }

    //    m_compInfoIndex = pIndex;
    //    if(m_block >= m_compInfoIndex->block()){
    //    	m_block = 0;
    //    }
    //    m_orgname = m_compInfoIndex->name();

    m_orgname = pIndex->name();
    m_compInfoIndex = new SCompIndex;
    m_compInfoIndex->m_pCompInfo = new SCompInfo(*(pIndex->m_pCompInfo));
    if(m_block >= m_compInfoIndex->block()) {
        m_block = 0;
    }

    setPinLtrb();
}

//LTRBに10ピクセル長ピンが存在しているかどうかのフラグの設定
void SXBSchComponent::setPinLtrb()
{
    SPoint pt;    //ダミー
    int ltrb;

    m_pinltrb = 0;

    if(m_compInfoIndex == NULL) {
        return;
    }
    int nMax = m_compInfoIndex->pinCount();
    for(int nIndex = 0; nIndex < nMax; nIndex++) {
        const SPin* pPin = pinEnd(nIndex,ltrb,pt);
        if(pPin) {
            if((pPin->type() & PIN_TYPE_ZLENG) == 0) {
                m_pinltrb |= ((1<<ltrb)&0xf);
            }
        }
    }
}

//n番目のピンの位置を得る。
SPoint SXBSchComponent::pinPosition(int nIndex)const
{
    SPoint pt;
    int dummy;
    pinEnd(nIndex,dummy,pt);
    return pt;
}

int SXBSchComponent::pinCount() const
{
    if(m_compInfoIndex == NULL) {
        return 0;
    } else {
        return m_compInfoIndex->pinCount();
    }
}


//ピンの根っこの位置情報を取得する
const SPin*  SXBSchComponent::pinLoc(int nIndex,int& nLtrb,SPoint& ptLoc) const
{
    if(m_compInfoIndex == NULL) {
        return NULL;
    }
    const SPin* pininfo =  m_compInfoIndex->pin(nIndex);
    if(pininfo == NULL) {
        return NULL;
    }
    int ltrb = pininfo->ltrb(); // L:0 T:1 R:2 B:3
    int offset = pininfo->offset()*10;

    nLtrb = (ltrb + (m_dir & 3)) &3;
    if(m_dir & 4) {
        if(nLtrb == 0) {
            nLtrb = 2;
        } else if(nLtrb == 2) {
            nLtrb = 0;
        }
    }
    SSize wh = size();
    int w = wh.w();
    int h = wh.h();
    int x = m_p1.x();
    int y = m_p1.y();
    switch(nLtrb) {
    case 0:    	// L
        ptLoc.setX(x-w);
        if(m_dir == 2 || m_dir == 3 || m_dir == 6 || m_dir == 7) {
            ptLoc.setY(y-offset);
        } else {
            ptLoc.setY(y-h+offset);
        }
        break;
    case 1:    	// T
        ptLoc.setY(y-h);
        if(m_dir == 1 || m_dir == 2 || m_dir == 4 || m_dir == 7) {
            ptLoc.setX(x-offset);
        } else {
            ptLoc.setX(x-w+offset);
        }
        break;
    case 2:    	// R
        ptLoc.setX(x);
        if(m_dir == 2 || m_dir == 3 || m_dir == 6 || m_dir == 7) {
            ptLoc.setY(y-offset);
        } else {
            ptLoc.setY(y-h+offset);
        }
        break;
    case 3:    	// B
        ptLoc.setY(y);
        if(m_dir == 1 || m_dir == 2 || m_dir == 4 || m_dir == 7) {
            ptLoc.setX(x-offset);
        } else {
            ptLoc.setX(x-w+offset);
        }
        break;
    }
    return pininfo;
}

//ピンの先の位置情報を取得する
const SPin* SXBSchComponent::pinEnd(int nIndex,int& nLtrb,SPoint& ptEnd) const
{
    const SPin*  pPin = pinLoc(nIndex,nLtrb,ptEnd);
    if(!pPin)return NULL;
    if(pPin->type() & PIN_TYPE_ZLENG) {
        return pPin;
    } else {
        switch(nLtrb) {
        case 0:    //L
            ptEnd.setX(ptEnd.x()-PIN_LENGTH);
            break;
        case 1:    //T
            ptEnd.setY(ptEnd.y()-PIN_LENGTH);
            break;
        case 2:    //R
            ptEnd.setX(ptEnd.x()+PIN_LENGTH);
            break;
        default://    	    case 3:    //B
            ptEnd.setY(ptEnd.y()+PIN_LENGTH);
        }
        return pPin;
    }


    //if(m_compInfoIndex == NULL){
    //    return NULL;
    //}
    //const SPin* pininfo =  m_compInfoIndex->pin(nIndex);
    //if(pininfo == NULL){
    //    return NULL;
    //}
    //int ltrb = pininfo->ltrb(); // L:0 T:1 R:2 B:3
    //int offset = pininfo->offset()*10;

    //nLtrb = (ltrb + (m_dir & 3)) &3;
    //if(m_dir & 4){
    //    if(nLtrb == 0){
    //    	nLtrb = 2;
    //    }else if(nLtrb == 2){
    //    	nLtrb = 0;
    //    }
    //}
    //SSize wh = size();
    //int w = wh.w();
    //int h = wh.h();
    //int x = m_p1.x();
    //int y = m_p1.y();
    //switch(nLtrb){
    //case 0:    	// L
    //    ptEnd.setX(x-w-PIN_LENGTH);
    //    if(m_dir == 2 || m_dir == 3 || m_dir == 6 || m_dir == 7){
    //    	ptEnd.setY(y-offset);
    //    }else{
    //    	ptEnd.setY(y-h+offset);
    //    }
    //    break;
    //case 1:    	// T
    //    ptEnd.setY(y-h-PIN_LENGTH);
    //    if(m_dir == 1 || m_dir == 2 || m_dir == 4 || m_dir == 7){
    //    	ptEnd.setX(x-offset);
    //    }else{
    //    	ptEnd.setX(x-w+offset);
    //    }
    //    break;
    //case 2:    	// R
    //    ptEnd.setX(x+PIN_LENGTH);
    //    if(m_dir == 2 || m_dir == 3 || m_dir == 6 || m_dir == 7){
    //    	ptEnd.setY(y-offset);
    //    }else{
    //    	ptEnd.setY(y-h+offset);
    //    }
    //    break;
    //case 3:    	// B
    //    ptEnd.setY(y+PIN_LENGTH);
    //    if(m_dir == 1 || m_dir == 2 || m_dir == 4 || m_dir == 7){
    //    	ptEnd.setX(x-offset);
    //    }else{
    //    	ptEnd.setX(x-w+offset);
    //    }
    //    break;
    //}
    //return pininfo;
}
