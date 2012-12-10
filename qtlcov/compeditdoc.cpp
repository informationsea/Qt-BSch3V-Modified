/****************************************************************************
    Qt-LCoV Library Editor for Qt-BSch3V
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
#include <qapplication.h>
#include <qclipboard.h>
#include <qstring.h>
#include "stdafx.h"
#include "compeditdoc.h"
#include <limits.h>

SCompEditDoc::SCompEditDoc(void)
{
    m_nOpCount    = 0;
    m_dirtyFlag    = false;
    m_tempDirtyFlag = false;
    m_size = SSize(1,1);
    m_block = 1;
}

SCompEditDoc::~SCompEditDoc(void)
{
    init();
}

bool SCompEditDoc::releaseTemp()
{
    if(m_listTemp.size() == 0) return false;
    SCompEditElmListIterator ite;
    if(m_tempDirtyFlag) {    //一時データに変更が発生していた場合
        bool allNewPlace = true;
        ite = m_listTemp.begin();
        while(ite != m_listTemp.end()) {
            bool valid = true;
            if(ite->m_dataType == DATATYPE_PTN) {
                SPtnObj* pObj =(SPtnObj*)ite->m_pData;
                if(!pObj->qValid()) {
                    valid = false;
                    delete pObj;
                }
            }
            if(valid) {
                ite->m_pOrigin = NULL;
                m_listMain.push_back(*ite);
            }
            ite++;
        }
        m_tempDirtyFlag = false;
        m_dirtyFlag = true;
    } else {    	    	    //一時データに変更が発生しなかった場合
        ite = m_listTemp.begin();
        while(ite != m_listTemp.end()) {
            //一時データにコピーしたオブジェクトのdelete
            if(ite->m_dataType == DATATYPE_PIN) {
                SPin* pPin =(SPin*)ite->m_pData;
                delete pPin;
            } else {
                SPtnObj* pObj =(SPtnObj*)ite->m_pData;
                delete pObj;
            }
            //元のデータを復活させる
            if(ite->m_pOrigin) {
                SCompEditElm* pOrgElm = ite->m_pOrigin;
                pOrgElm->m_deleteOpCount = -1;
            }
            ite++;
        }
    }
    m_listTemp.clear();
    return true;
}

bool SCompEditDoc::canUndo()
{
    return (m_nOpCount > 0);
}

bool SCompEditDoc::undo()
{
    releaseTemp();
    if(m_listMain.size() == 0) {
        m_nOpCount = 0;
        return false;
    }
    int undoCount = 0;
    while(undoCount == 0 && m_nOpCount > 0) {
        SCompEditElmListIterator ite;
        ite = m_listMain.begin();
        while(ite != m_listMain.end()) {
            if(ite->m_deleteOpCount == m_nOpCount) {
                ite->m_deleteOpCount= -1;
                undoCount++;
                ite++;
            } else if(ite->m_placeOpCount == m_nOpCount) {
                if(ite->m_dataType == DATATYPE_PIN) {
                    SPin* pPin =(SPin*)ite->m_pData;
                    delete pPin;
                } else {
                    SPtnObj* pObj =(SPtnObj*)ite->m_pData;
                    delete pObj;
                }
                ite->m_pData = NULL;
                ite = m_listMain.erase(ite);
                undoCount++;
            } else {
                ite++;
            }
        }
        m_nOpCount--;
    }
    return (undoCount > 0);
}

bool SCompEditDoc::copy()
{
    if(m_listTemp.size() == 0) return false;

    string str;
    SWriteCE3 wce3;
    wce3.Attach(&str);

    SCompEditElmListIterator ite;
    ite = m_listTemp.begin();
    if(m_listTemp.size() == 1 && ite->m_dataType == DATATYPE_PIN) {
        wce3.WriteRecord("+LCOV_PIN_V.1.0");
        wce3.WriteEOL();
        SPin* pPin = (SPin*)ite->m_pData;
        pPin->writeCe3(wce3);
        wce3.WriteEOL();
        wce3.WriteRecord("-LCOV_PIN_V.1.0");
        wce3.WriteEOL();
    } else {
        SPoint pLT = SPoint(m_tempArea.l(),m_tempArea.t());
        wce3.WriteRecord("+LCOV_PTN_V.1.0");
        wce3.WriteEOL();
        while(ite != m_listTemp.end()) {
            if(ite->m_dataType != DATATYPE_PIN) {
                SPtnObj* pObj =(SPtnObj*)ite->m_pData;
                pObj->writeCe3(wce3,&pLT);
            }
            ite++;
        }
        wce3.WriteRecord("-LCOV_PTN_V.1.0");
        wce3.WriteEOL();
    }

    QClipboard *cb = QApplication::clipboard();
    cb->setText(QString::fromLocal8Bit(str.c_str()));

    //    int nLength = str.size()+1;
    //    HGLOBAL hGMem=::GlobalAlloc(GMEM_MOVEABLE,nLength);//クリップボードに渡すメモリの確保
    //    if(hGMem == NULL){
    //    	return false;
    //    }
    //
    //
    //
    //    BYTE* pgBuff=(BYTE*)::GlobalLock(hGMem);    //割り当てたグローバルメモリをロックする
    //    memcpy(pgBuff,str.c_str(),nLength);    	//割り当てたグローバルメモリにメモリファイルの内容をコピー
    //    ::GlobalUnlock(hGMem);    	    	//グローバルメモリのアンロック
    //    ::OpenClipboard(AfxGetMainWnd()->m_hWnd);//クリップボードのオープン
    //    ::EmptyClipboard();    	    	    //クリップボードを空にする
    //    ::SetClipboardData(CF_TEXT,hGMem);    //クリップボードにデータを渡す
    //    ::CloseClipboard();    	    	    //クリップボードを閉じる

    return true;
}

//範囲を指定して行う選択動作
int SCompEditDoc::doSelect(const SRect& rc)
{
    //TRACE("doSelect(const SRect& rc)  %d\n",m_nOpCount);

    SCompEditElm elm;
    int count = 0;
    SCompEditElmListRevIterator rite;
    rite = m_listMain.rbegin();
    while(rite != m_listMain.rend()) {
        if(rite->m_deleteOpCount < 0 && rite->m_dataType == DATATYPE_PTN) {
            SPtnObj* pPtn =(SPtnObj*)rite->m_pData;
            if(pPtn->testIntersect(rc)) {
                rite->m_deleteOpCount = m_nOpCount;
                SPtnObj* pNewPtn = pPtn->duplicate();
                elm.m_dataType = DATATYPE_PTN;
                elm.m_placeOpCount = m_nOpCount;
                elm.m_deleteOpCount = -1;
                elm.m_pOrigin = &(*rite);
                elm.m_pData = pNewPtn;
                m_tempDirtyFlag = false;
                m_listTemp.push_front(elm);
                count++;
            }
        }
        rite++;
    }
    return count;
}



//点を指定して行う選択動作
int SCompEditDoc::doSelect(const SPoint& pt)
{
    SRect rcArea;
    SCompEditElm elm;

    releaseTemp();
    m_tempDirtyFlag = false;

    SCompEditElmListIterator ite;
    ite = m_listMain.begin();
    while(ite != m_listMain.end()) {
        if(ite->m_deleteOpCount < 0 && ite->m_dataType == DATATYPE_PIN) {
            SPin* pPin =(SPin*)ite->m_pData;
            rcArea = pinArea(pPin->ltrb(),pPin->offset());
            if(rcArea.intersect(pt)) {
                ite->m_deleteOpCount = m_nOpCount;
                SPin* pNewPin = new SPin(*pPin);
                elm.m_dataType = DATATYPE_PIN;
                elm.m_placeOpCount = m_nOpCount;
                elm.m_deleteOpCount = -1;
                elm.m_pOrigin = &(*ite);
                elm.m_pData = pNewPin;
                m_tempDirtyFlag = false;
                m_listTemp.push_back(elm);
                return 1;
            }
        }
        ite++;
    }
    /*
    SCompEditElmListRevIterator rite;
    rite = m_listMain.rbegin();
    while(rite != m_listMain.rend()){
    if(rite->m_deleteOpCount < 0 && rite->m_dataType == DATATYPE_PTN){
    SPtnObj* pPtn =(SPtnObj*)rite->m_pData;
    //rcArea = pPtn->area();
    //rcArea.setL(rcArea.l()-1);
    //rcArea.setT(rcArea.t()-1);
    //rcArea.setR(rcArea.r()+1);
    //rcArea.setB(rcArea.b()+1);
    //if(rcArea.intersect(pt)){
    if(pPtn->testIntersect(pt)){
    rite->m_deleteOpCount = m_nOpCount;
    SPtnObj* pNewPtn = pPtn->duplicate();
    elm.m_dataType = DATATYPE_PTN;
    elm.m_placeOpCount = m_nOpCount;
    elm.m_deleteOpCount = -1;
    elm.m_pOrigin = &(*rite);
    elm.m_pData = pNewPtn;
    m_tempDirtyFlag = false;
    m_listTemp.push_back(elm);
    return 1;
    }
    }
    rite++;
    }
    */
    return 0;
}



bool SCompEditDoc::delTemp()
{
    if(m_listTemp.size() == 0) return false;
    bool allNewPlace = true;
    SCompEditElmListIterator ite;
    ite = m_listTemp.begin();
    while(ite != m_listTemp.end()) {
        //一時データにコピーしたオブジェクトのdelete
        if(ite->m_dataType == DATATYPE_PIN) {
            SPin* pPin =(SPin*)ite->m_pData;
            delete pPin;
        } else {
            SPtnObj* pObj =(SPtnObj*)ite->m_pData;
            delete pObj;
        }
        if(ite->m_pOrigin != NULL) {
            allNewPlace = false;
        }
        ite++;
    }
    m_listTemp.clear();
    m_tempDirtyFlag = false;
    if(!allNewPlace) {
        m_dirtyFlag = true;
    }
    return true;
}

bool SCompEditDoc::mirror()
{
    if(m_listTemp.size() == 0) return false;
    int center = (m_tempArea.l()+m_tempArea.r())/2;
    bool retv = false;
    SCompEditElmListIterator ite;
    ite = m_listTemp.begin();
    while(ite != m_listTemp.end()) {
        if(ite->m_dataType == DATATYPE_PTN) {
            SPtnObj* pObj =(SPtnObj*)ite->m_pData;
            pObj->mirrorH(center);
            m_tempDirtyFlag = true;
            retv = true;
        }
        ite++;
    }
    if(retv) {
        setupTempArea();
    }
    return retv;
}

bool SCompEditDoc::rotate()
{
    if(m_listTemp.size() == 0) return false;
    int centerX = (m_tempArea.l()+m_tempArea.r())/2;
    int centerY = (m_tempArea.t()+m_tempArea.b())/2;
    SPoint pt(centerX,centerY);
    bool retv = false;
    SCompEditElmListIterator ite;
    ite = m_listTemp.begin();
    while(ite != m_listTemp.end()) {
        if(ite->m_dataType == DATATYPE_PTN) {
            SPtnObj* pObj =(SPtnObj*)ite->m_pData;
            pObj->rotate(pt);
            m_tempDirtyFlag = true;
            retv = true;
        }
        ite++;
    }
    if(retv) {
        setupTempArea();
    }
    return retv;
}


bool SCompEditDoc::move(int offsetx,int offsety)
{
    if(m_listTemp.size() == 0) return false;
    bool retv = false;
    SCompEditElmListIterator ite;
    ite = m_listTemp.begin();
    while(ite != m_listTemp.end()) {
        if(ite->m_dataType == DATATYPE_PTN) {
            SPtnObj* pObj =(SPtnObj*)ite->m_pData;
            pObj->move(offsetx, offsety);
            m_tempDirtyFlag = true;
            retv = true;
        }
        ite++;
    }
    return retv;
}


void SCompEditDoc::setupTempArea()
{

    if(m_listTemp.size() == 0) return;
    m_tempArea = SRect(SPoint(INT_MAX,INT_MAX),SPoint(INT_MIN,INT_MIN));

    SCompEditElmListIterator ite;
    ite = m_listTemp.begin();
    while(ite != m_listTemp.end()) {
        if(ite->m_dataType == DATATYPE_PIN) {
            SPin* pPin =(SPin*)ite->m_pData;
            m_tempArea |= pinArea(pPin->ltrb(),pPin->offset());
        } else {
            SPtnObj* pObj =(SPtnObj*)ite->m_pData;
            m_tempArea |= pObj->area();
        }
        ite++;
    }
    m_tempArea.setL(m_tempArea.l()-1);
    m_tempArea.setT(m_tempArea.t()-1);
    m_tempArea.setR(m_tempArea.r()+1);
    m_tempArea.setB(m_tempArea.b()+1);
}
/*
int SCompEditDoc::isOnGrip(const SPoint& pt)
{
 SRect rc;
 rc.setL(pt.x()-1);
 rc.setT(pt.y()-1);
 rc.setR(pt.x()+1);
 rc.setB(pt.y()+1);

 if(m_listTemp.size() != 1) return -1;
 SCompEditElmListIterator ite;
 ite = m_listTemp.begin();
 if(ite->m_dataType != DATATYPE_PTN) return -1;
 SPtnObj* pObj =(SPtnObj*)ite->m_pData;
 int nodes = pObj->gripCount();
 int i;
 for(i = 0;i < nodes;i++){
  SPoint ptGrip = pObj->gripPos(i);
  if(rc.intersect(ptGrip)) return i;
 }
 return -1;
}
*/


bool SCompEditDoc::isOnTmpData(const SPoint& pt)
{
    if(m_listTemp.size() == 0) return false;
    return m_tempArea.intersect(pt);
}

void SCompEditDoc::init()
{
    m_nOpCount    = 0;
    m_dirtyFlag    = false;
    m_tempDirtyFlag = false;
    m_block = 1;

    SCompEditElmListIterator ite;
    ite = m_listMain.begin();
    while(ite != m_listMain.end()) {
        if(ite->m_dataType == DATATYPE_PIN) {
            SPin* pPin =(SPin*)ite->m_pData;
            delete pPin;
        } else {
            SPtnObj* pObj =(SPtnObj*)ite->m_pData;
            delete pObj;
        }
        ite++;
    }

    m_listMain.clear();

    ite = m_listTemp.begin();
    while(ite != m_listTemp.end()) {
        if(ite->m_dataType == DATATYPE_PIN) {
            SPin* pPin =(SPin*)ite->m_pData;
            delete pPin;
        } else {
            SPtnObj* pObj =(SPtnObj*)ite->m_pData;
            delete pObj;
        }
        ite++;
    }
    m_listTemp.clear();
}

void SCompEditDoc::setPin(const PSPin* pPinArray, int count)
{
    if(!pPinArray)return;
    SCompEditElm elm;
    for(int i = 0; i < count; i++) {
        SPin* pNewPin = new SPin(*(pPinArray[i]));
        elm.m_dataType = DATATYPE_PIN;
        elm.m_placeOpCount = m_nOpCount;
        elm.m_deleteOpCount = -1;
        elm.m_pOrigin = NULL;
        elm.m_pData = pNewPin;
        m_listMain.push_back(elm);
    }
}

//一時データへのピンの追加
bool SCompEditDoc::addPinToTemp(SPin* pPin)
{
    if(!pPin) return false;
    SCompEditElm elm;
    SPin* pNewPin = new SPin(*pPin);
    elm.m_dataType = DATATYPE_PIN;
    elm.m_placeOpCount = m_nOpCount;
    elm.m_deleteOpCount = -1;
    elm.m_pOrigin = NULL;
    elm.m_pData = pNewPin;
    m_tempDirtyFlag = true;
    m_listTemp.push_back(elm);
    return true;
}


//一時データへのパターンの追加
bool SCompEditDoc::addPtnToTemp(SPtnObj* pPtn)
{
    if(!pPtn) return false;
    SCompEditElm elm;
    SPtnObj* pNewPtn = pPtn->duplicate();
    elm.m_dataType = DATATYPE_PTN;
    elm.m_placeOpCount = m_nOpCount;
    elm.m_deleteOpCount = -1;
    elm.m_pOrigin = NULL;
    elm.m_pData = pNewPtn;
    m_tempDirtyFlag = true;
    m_listTemp.push_back(elm);
    return true;
}



void SCompEditDoc::setPtnObj(const PSPtnObj* pPtnArray, int count)
{
    if(!pPtnArray) return;
    SCompEditElm elm;
    for(int i = 0; i < count; i++) {
        SPtnObj* pNewObj = pPtnArray[i]->duplicate();
        elm.m_dataType = DATATYPE_PTN;
        elm.m_placeOpCount = m_nOpCount;
        elm.m_deleteOpCount = -1;
        elm.m_pOrigin = NULL;
        elm.m_pData = pNewObj;
        m_listMain.push_back(elm);
    }
}

void SCompEditDoc::loadCompInfo(const SCompInfo* pComp)
{
    init();
    if(!pComp)return;
    m_size = pComp->m_size;
    m_block = pComp->m_nBlock;
    setPin(pComp->m_ppPin,pComp->m_nPinCount);
    if(pComp->m_pPtn) {
        setPtnObj(pComp->m_pPtn->m_ppvPtn,pComp->m_pPtn->m_vPtnCount);
    }
}


void SCompEditDoc::saveCompInfo(SCompInfo* pComp)
{
    releaseTemp();

    incrementOpCount();

    pComp->m_size = m_size;
    pComp->m_nBlock = m_block;
    int i,j;
    int pinCount = 0;
    int ptnCount = 0;

    SRect rcValid(-15,-15,m_size.w()*10+30,m_size.h()*10+30);

    SCompEditElmListIterator ite;
    ite = m_listMain.begin();
    while(ite != m_listMain.end()) {
        if(ite->m_deleteOpCount < 0) {
            if(ite->m_dataType == DATATYPE_PIN) {
                pinCount++;
            } else {
                SPtnObj* pPtn = (SPtnObj*)ite->m_pData;
                if(!pPtn->testIntersect(rcValid)) {
                    ite->m_deleteOpCount = m_nOpCount;
                    m_dirtyFlag = true;
                } else {
                    ptnCount++;
                }
            }
        }
        ite++;
    }


    for(i = 0; i < pComp->m_nPinCount; i++) {
        SPin* pPin =  pComp->m_ppPin[i];
        delete pPin;
    }
    delete[]pComp->m_ppPin;
    pComp->m_ppPin =NULL;
    pComp->m_nPinCount = 0;

    if(pComp->m_pPtn != NULL) {
        for(i = 0; i < pComp->m_pPtn->m_vPtnCount; i++) {
            SPtnObj* pPtn = pComp->m_pPtn->m_ppvPtn[i];
            delete pPtn;
        }
        delete[]pComp->m_pPtn->m_ppvPtn;
        pComp->m_pPtn->m_ppvPtn = NULL;
        pComp->m_pPtn->m_vPtnCount = 0;
    }

    pComp->m_ppPin = new PSPin[pinCount];
    if(ptnCount > 0 && pComp->m_pPtn != NULL) {
        pComp->m_pPtn->m_ppvPtn = new PSPtnObj[ptnCount];
    }

    i = 0;
    j = 0;


    ite = m_listMain.begin();
    while(ite != m_listMain.end()) {
        if(ite->m_deleteOpCount < 0) {
            if(ite->m_dataType == DATATYPE_PIN) {
                if(i < pinCount) {    	// i はpinのカウンタ
                    SPin* pPin =(SPin*)ite->m_pData;
                    pComp->m_ppPin[i] = new SPin(*pPin);
                    pComp->m_nPinCount++;
                }
                i++;
            } else {
                if(pComp->m_pPtn != NULL) {
                    if(j < ptnCount) {    	// j はptnのカウンタ
                        SPtnObj* pPtn =(SPtnObj*)ite->m_pData;
                        pComp->m_pPtn->m_ppvPtn[j] = pPtn->duplicate();
                        pComp->m_pPtn->m_vPtnCount++;
                    }
                    j++;
                }
            }
        }
        ite++;
    }
}


//既存ピンと同じ場所なら、既存ピンのポインタを返す
SPin* SCompEditDoc::pinConflict(const SPin* pPin)
{
    SCompEditElmListIterator ite;
    ite = m_listMain.begin();
    while(ite != m_listMain.end()) {
        if(ite->m_deleteOpCount < 0) {
            if(ite->m_dataType == DATATYPE_PIN) {
                SPin* pPin2 =(SPin*)ite->m_pData;
                if(    (pPin->ltrb() == pPin2->ltrb()) && (pPin->offset() == pPin2->offset()) ) return pPin2;
            }
        }
        ite++;
    }
    /*
    ite = m_listTemp.begin();
    while(ite != m_listTemp.end()){
    if(ite->m_deleteOpCount < 0){
    if(ite->m_dataType == DATATYPE_PIN){
    SPin* pPin2 =(SPin*)ite->m_pData;
    if(    (pPin->ltrb() == pPin2->ltrb()) && (pPin->offset() == pPin2->offset()) ) return pPin2;
    }
    }
    ite++;
    }
    */
    return NULL;
}


//ptに最も近いピン配置座標を得る
void SCompEditDoc::pinPlacePoint(const SPoint& pt,int &ltrb,int &offset)
{
    int right = m_size.w()*10;
    int bottom = m_size.h()*10;
    int x = pt.x();
    int y = pt.y();

    ltrb = -1;

    if(x < 0) {
        if((y > x)&&((bottom - y)>x)) ltrb = 0;
    } else if(x > right) {
        if((y>(right-x))&&((bottom - y)>(right-x))) ltrb = 2;
    } else if(y < 0) {
        ltrb = 1;
    } else if(y > bottom) {
        ltrb = 3;
    }
    if(ltrb == -1) {    // ここまでで未設定
        int ld = x;
        int td = y;
        int rd = right  -x;
        int bd = bottom -y;
        if(ld < rd) {
            if(ld < td && ld < bd)    ltrb = 0;
            else if(td < bd)    	ltrb = 1;
            else    	    	ltrb = 3;
        } else {
            if(rd < td && rd < bd)    ltrb = 2;
            else if(td < bd)    	ltrb = 1;
            else    	    	ltrb = 3;
        }
    }
    if(ltrb == 0 || ltrb == 2) {
        offset = y;
        if(offset < 0) offset = 0;
        else if(offset > bottom) offset = bottom;
    } else {
        offset = x;
        if(offset < 0) offset = 0;
        else if(offset > right) offset = right;
    }
    offset = (offset+5)/10;
}

//ピン座標から有効な占有範囲を求める
SRect SCompEditDoc::pinArea(int ltrb,int offset)
{
    SPoint pt = pinPosition(ltrb,offset);
    int l,t,w,h;
    int x = pt.x();
    int y = pt.y();
    switch(ltrb) {
    case 0:
        l = x-10;
        t = y-4;
        w = 10;
        h = 8;
        break;
    case 1:
        l = x-4;
        t = y-10;
        w = 8;
        h = 10;
        break;
    case 2:
        l = x;
        t = y-4;
        w = 10;
        h = 8;
        break;
    default:
        l = x-4;
        t = y;
        w = 8;
        h = 10;
        break;
    }
    return SRect(l,t,w,h);
}


//ピンの座標から部品編集座標を得る
SPoint SCompEditDoc::pinPosition(int ltrb,int offset)
{
    int x,y;

    offset *= 10;
    int cx = m_size.w()* 10;
    int cy = m_size.h()* 10;

    switch(ltrb) {
    case 0:
        x = 0;
        y = offset;
        break;
    case 1:
        x = offset;
        y = 0;
        break;
    case 2:
        x = cx;
        y = offset;
        break;
    default:
        x = offset;
        y = cy;
        break;
    }
    return SPoint(x,y);
}



