/****************************************************************************
    Qt-LCoV Library Editor for Qt-BSch3V
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


#include "componentview.h"
#include <qbitmap.h>
#include <qimage.h>
#include <qpainter.h>
#include <qclipboard.h>
#include <qcursor.h>
#include <qcheckbox.h>
#include <QComboBox>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qradiobutton.h>
//Added by qt3to4:
#include <QPixmap>
#include <QResizeEvent>
#include <QMouseEvent>


#include "complib.h"
#include "xbsch.h"
#include "drawcomponentbase.h"
#include "xbschpattern.h"
#include "propptn.h"
//#include "proppinimpl.h"
#include "editptntextdlg.h"
#include "proppindlg.h"

//コンストラクタ
SComponentView::SComponentView(QWidget* parent):QWidget (parent)
{
    m_tool = TOOL_SELECT;
    m_placeState = 0;
    //m_pBmpBuff = NULL;
    m_offScrnBuffSize = QSize(0,0);
    m_offScrnBuffOrg = QPoint(INT_MIN,INT_MIN);
    m_zoomNum = 0;
    m_magMul = 1;    	    	//表示倍率の分子
    m_magDiv = 1;    	    //表示倍率の分母
    m_block = 0;
    m_pCompIndex = NULL;
    m_pageSize = QSize(10,10);    //適当な値
    m_windowOrg = QPoint(0,0);    //適当な値
    m_pCursorPin = NULL;
    m_dragOpStatus = DRAGOP_NOT;
    m_startSelect = false;
    m_editNodeMode = false;
    m_movingGrip = -1;
    setZoomValue(0);
    //viewport()->setBackgroundMode(Qt::NoBackground);
    //    viewport()->setBackgroundColor(QColor("white"));
    //viewport()->setMouseTracking(true);
    //常にスクロールバーを表示
    //setHScrollBarMode(AlwaysOn);
    //setVScrollBarMode(AlwaysOn);
    //QSize size = getContentsSize();

#if 0
#ifdef Q_WS_MACX
    QBitmap bmp_curstd_b;
    bmp_curstd_b.fromData(QSize(16,16),curstd_b_bits,QImage::Format_MonoLSB);
    QBitmap bmp_curstd_m;
    bmp_curstd_m.fromData(QSize(16,16),curstd_m_bits,QImage::Format_MonoLSB);
    QBitmap bmp_curmov_b;
    bmp_curmov_b.fromData(QSize(16,16),curmov_b_bits,QImage::Format_MonoLSB);
    QBitmap bmp_curmov_m;
    bmp_curmov_m.fromData(QSize(16,16),curmov_m_bits,QImage::Format_MonoLSB);
#else
    QBitmap bmp_curstd_b(32,32,(const uchar *)curstd_b_bits,true);
    QBitmap bmp_curstd_m(32,32,(const uchar *)curstd_m_bits,true);
    QBitmap bmp_curmov_b(32,32,(const uchar *)curmov_b_bits,true);
    QBitmap bmp_curmov_m(32,32,(const uchar *)curmov_m_bits,true);
#endif

    m_pCurStd = new QCursor(bmp_curstd_b,bmp_curstd_m,0,0);
    m_pCurMov = new QCursor(bmp_curmov_b,bmp_curmov_m,0,0);
#endif
    // setBackgroundColor(Qt::white);
    //setPalette();

    //resizeContents(0,0);
    //resize(0,0);
    setMinimumSize(0,0);

    setMouseTracking(true);

}

//デストラクタ
SComponentView::~SComponentView()
{
}

//スクロールビューのコンテンツのサイズを決める
QSize SComponentView::setPageSize()
{
    int cx,cy;
    if(!m_pCompIndex) {
        cx = 10;
        cy = 10;
    } else {
        cx = m_pCompIndex->size().w()*10;
        cy = m_pCompIndex->size().h()*10;
    }
    cx = (cx+PAGE_MARGIN*2)*m_magMul/m_magDiv;
    cy = (cy+PAGE_MARGIN*2)*m_magMul/m_magDiv;
    m_pageSize = QSize(cx,cy);

    //resize(cx,cy);
    setMinimumSize(cx,cy);

    drawToBuff(true);

    return m_pageSize;
}

//ツールの設定
void SComponentView::setTool(int tool)
{
    if(m_tool == tool) return;

    delete     m_pCursorPin;
    m_pCursorPin = NULL;

    if(m_doc.releaseTemp()) {
        drawToBuff(true);
    }
    m_placeState = 0;
    m_tool = tool;
    switch(m_tool) {
    case TOOL_PIN:
        m_pCursorPin = new SPin;
        m_pCursorPin->setBlock(1);
        m_pCursorPin->setPinName("");
        m_pCursorPin->setPinNum(0,"");
        break;
    }
    m_movingGrip = -1;

    //viewport()->update();    //画面の更新を強制
    update();
}

// 何か DELETE できる状態なら true を返す
bool SComponentView::canDelete()
{
    if(m_doc.m_listTemp.size()>0) return true;
    else return false;
}
// DELETEの実際
bool SComponentView::doDelete()
{
    if(!m_doc.delTemp()) {
        return false;
    }
    update();    //画面の更新を強制
    return true;
}

// ビットパターンの生成が可能なとき true
bool SComponentView::canCreateBitPtn()
{
    if(m_pCompIndex == NULL) return false;
    if(m_pCompIndex->m_pCompInfo->m_pPtn) {
        SPtn *pPtn = m_pCompIndex->m_pCompInfo->m_pPtn;
        if(pPtn->m_pBuff) return true;
    }
    return false;
}

// ビットパターン生成の実際


// COPYできる状態なら true を返す
bool SComponentView::canCopy()
{
    if(m_doc.m_listTemp.size()>0) return true;
    else return false;
}

// COPYの実際
bool SComponentView::doCopy()
{
    if(!m_doc.copy()) return false;
    update();    //画面の更新を強制
    return true;
}

// UNDOできる状態なら true を返す
bool SComponentView::canUndo()
{
    return m_doc.canUndo();
}
// UNDOの実際
bool SComponentView::doUndo()
{
    m_doc.undo();
    drawToBuff(true);
    update();    //画面の更新を強制
    return true;
}

// ミラーまたは回転が可能なとき true を返す
bool SComponentView::canMirrorRotate()
{
    if(m_doc.m_listTemp.size() == 0) return false;
    SCompEditElmListIterator ite;
    ite = m_doc.m_listTemp.begin();
    if(ite->m_dataType == SCompEditDoc::DATATYPE_PIN) return false;
    return true;
}

// ミラーの実際
bool SComponentView::doMirror()
{
    if(!m_doc.mirror()) return false;
    update();    //画面の更新を強制
    return true;
}

// 回転の実際
bool SComponentView::doRotate()
{
    if(!m_doc.rotate()) return false;
    update();    //画面の更新を強制
    return true;
}


void SComponentView::RefreshView()
{
    drawToBuff(true);
    update();    //画面の更新を強制
}

//ノード編集モード切替え
void  SComponentView::setEditNodeMode(bool editnode)
{
    if(editnode != m_editNodeMode) {
        m_editNodeMode = editnode;
        if(m_doc.m_listTemp.size()>0) {
            update();    //画面の更新を強制
        }
    }
}


// PASTEが可能なとき true を返す
bool SComponentView::canPaste()
{
    QString pcszIDPIN("+LCOV_PIN_V.1.0");
    QString pcszIDPTN("+LCOV_PTN_V.1.0");

    //bool retVal = false;

    QClipboard *cb = QApplication::clipboard();
    QString str;

    str= cb->text().left(pcszIDPIN.length());
    if(str == pcszIDPIN) return true;

    str= cb->text().left(pcszIDPTN.length());
    if(str == pcszIDPTN) return true;

    return false;
}


// PASTEの実際
bool SComponentView::doPaste()
{
    if(!canPaste())return false;
    //setTool(TOOL_SELECT);
    if(m_doc.releaseTemp()) {
        drawToBuff(true);
    }

    QClipboard *cb = QApplication::clipboard();
    string cbbuff = cb->text().toStdString();

    SReadCE3 rce3;
    rce3.Attach(cbbuff.c_str());

    string str;

    rce3.ReadRecord(str);
    if(str == "+LCOV_PIN_V.1.0") {
        return doPastePin(rce3);
    } else if(str == "+LCOV_PTN_V.1.0") {
        return doPastePtn(rce3);
    }
    return false;
}


// doPaste()から呼ばれるピンのペースト処理
bool SComponentView::doPastePin(SReadCE3& rce3)
{
    delete m_pCursorPin;
    m_pCursorPin = NULL;
    bool retv = false;
    string str;
    while(rce3.ReadRecord(str) != EOF) {
        if(str[0] == '-') {
            break;
        } else if(str[0] == '+') {
            if(str == "+PIN") {
                m_pCursorPin = new SPin;
                if(m_pCursorPin->readCe3(rce3,m_pCompIndex->block())) {
                    retv = true;
                } else {
                    delete m_pCursorPin;
                    m_pCursorPin = NULL;
                }
                break;
            } else {
                str[0]='-';
                if(rce3.SkipTo(str) == EOF) {
                    break;
                }
            }
        }
    }
    update();    //画面の更新を強制
    return retv;
}

// doPaste()から呼ばれるパターンのペースト処理
bool SComponentView::doPastePtn(SReadCE3& rce3)
{
    //QPoint ptOffset(contentsX(),contentsY()) ;//= m_windowOrg;
    QPoint ptOffset(-m_margin,-m_margin);
    LPtoCompEditP(ptOffset);
    if(ptOffset.x()<0) ptOffset.setX(0);
    if(ptOffset.y()<0) ptOffset.setY(0);


    bool retv = false;
    string str;
    m_doc.incrementOpCount();
    while(rce3.ReadRecord(str) != EOF) {
        if(str[0] == '-') {
            break;
        } else if(str[0] == '+') {
            SPtnObj* pPtnObj = NULL;
            if(str == "+L") {
                pPtnObj = new SPtnObjLine;
            } else if(str == "+C") {
                pPtnObj = new SPtnObjCircle;
            } else if(str == "+PG") {
                pPtnObj = new SPtnObjPolygon;
            } else if(str == "+AR") {
                pPtnObj = new SPtnObjArc;
            } else {
                str[0]='-';
                if(rce3.SkipTo(str) == EOF) {
                    retv= false;
                    break;
                }
            }
            if(pPtnObj != NULL) {
                if(pPtnObj->readCe3(rce3)) {
                    pPtnObj->move(ptOffset.x(),ptOffset.y());
                    m_doc.addPtnToTemp(pPtnObj);
                }
                delete pPtnObj;
            }
        }
    }
    m_doc.setupTempArea();
    update();    //画面の更新を強制
    return false;
}




// PROPERTYコマンドが実行可能なとき true を返す
bool SComponentView::canProperty()
{
    if(m_doc.m_listTemp.size() == 1) return true;
    else return false;
}


// PROPERTYコマンドの実際
bool SComponentView::editProperty()
{
    if(!canProperty()) return false;
    bool result = false;
    SCompEditElmListIterator ite;
    ite = m_doc.m_listTemp.begin();
    if(ite->m_dataType == SCompEditDoc::DATATYPE_PIN) {
        SPin* pPin =(SPin*)ite->m_pData;
        result = editPinProperty(pPin);
    } else {
        SPtnObj* pPtn =(SPtnObj*)ite->m_pData;
        result = editPtnProperty(pPtn);
    }

    if(result) {
        m_doc.m_tempDirtyFlag = true;
        update();    //画面の更新を強制
    }
    return result;
}

// editProperty()から呼ばれるピンのPROPERTYコマンド
bool SComponentView::editPinProperty(SPin* pPin)
{
    bool retVal = false;
    int blockCount = pPin->block();
    if(blockCount <= 0) return false;

    prop_pindlg* dlg= new prop_pindlg(this);
    //    SPropPin* dlg= new SPropPin(this,0,true);

    const char* name = pPin->pinName();
    string strName="";
    while(*name) {
        char cc = *name;
        cc &= 0x7f;
        if(*name & 0x80) {
            strName += '\\';
        }
        strName += cc;
        name++;
    }

    dlg->setName(QString::fromLocal8Bit(strName.c_str()));

    QString * arrayNumber = new QString[blockCount];
    int i;
    for(i = 0; i < blockCount; i++) {
        if(pPin->pinNum(i)) {
            arrayNumber[i]=QString::fromLocal8Bit(pPin->pinNum(i));
        } else {
            arrayNumber[i]="";
        }
    }

    dlg->setNumber(blockCount,arrayNumber);
    dlg->setHideNumber(pPin->type() & PIN_TYPE_NUMHIDE);
    dlg->setPinType(pPin->type() & 0xf);

    //    dlg->editName->setText(QString::fromLocal8Bit(strName.c_str()));
    //    //dlg.setBlock(pPin->block());
    //        dlg->spinBlock->setMinValue(1);
    //       dlg->spinBlock->setMaxValue(pPin->block());
    //    dlg->m_currentBlock = 1;
    //    dlg->m_pNumber = new QString[pPin->block()];
    //
    //    int i;
    //    for(i = 0;i < pPin->block();i++){
    //    	if(pPin->pinNum(i)){
    //    	    dlg->m_pNumber[i]=QString::fromLocal8Bit(pPin->pinNum(i));
    //    	}else{
    //    	    dlg->m_pNumber[i]="";
    //    	}
    //    }
    //    dlg->editNumber->setText(dlg->m_pNumber[0]);
    //
    //
    //    //dlg.m_hideNum = (pPin->type() & PIN_TYPE_NUMHIDE);
    //    dlg->checkHide->setChecked(    pPin->type() & PIN_TYPE_NUMHIDE );
    //
    //    int pintype = pPin->type() & 0xf;
    //    if(pintype == PIN_TYPE_NOMAL){
    //    	dlg->radioNorm->setChecked(true);
    //    }else if(pintype == PIN_TYPE_SMALL){
    //    	dlg->radioS->setChecked(true);
    //    }else if(pintype == PIN_TYPE_CLOCK){
    //    	dlg->radioCk->setChecked(true);
    //    }else if(pintype == PIN_TYPE_NEGATIVE){
    //    	dlg->radioNeg->setChecked(true);
    //    }else{
    //    	dlg->radioNegCk->setChecked(true);
    //    }

    if(dlg->exec()) {
        pPin->setPinName(dlg->name().toLocal8Bit());
        for(i = 0; i < blockCount; i++) {
            pPin->setPinNum(i,arrayNumber[i].toLocal8Bit());
        }
        //    	for(i = 0;i < pPin->block();i++){
        //    	    pPin->setPinNum(i,dlg->m_pNumber[i]);
        //    	}
        //    	dlg->m_pNumber[dlg->m_currentBlock-1]=dlg->editNumber->text();
        int pintype = dlg->pinType();
        if(dlg->hideNumber())pintype |=     PIN_TYPE_NUMHIDE;


        //    	if(dlg->radioNorm->isChecked()) pintype = PIN_TYPE_NOMAL;
        //    	else if(dlg->radioS->isChecked()) pintype = PIN_TYPE_SMALL;
        //    	else if(dlg->radioCk->isChecked()) pintype = PIN_TYPE_CLOCK;
        //    	else if(dlg->radioNeg->isChecked()) pintype = PIN_TYPE_NEGATIVE;
        //    	else pintype=(PIN_TYPE_NEGATIVE | PIN_TYPE_CLOCK);
        //
        //    	if(dlg->checkHide->isChecked())pintype |=     PIN_TYPE_NUMHIDE;
        pPin->setType(pintype);
        retVal = true;
    }
    //    delete[]dlg->m_pNumber;
    delete dlg;
    delete[]arrayNumber;
    return retVal;
}

// editProperty()から呼ばれるパターンのPROPERTYコマンド
bool SComponentView::editPtnProperty(SPtnObj* pPtn)
{
    if( pPtn->id() == VECTPTN_TXT) {
        SPtnObjText* pText = (SPtnObjText*)pPtn;
        editptntextdlg *dlg = new editptntextdlg(this);
        dlg->setText(QString::fromLocal8Bit(pText->m_strText.c_str()));
        if(pText->m_fontName.size()>0) {
            dlg->setUseLabelFont(false);
            QFont font(QString::fromLocal8Bit(pText->m_fontName.c_str()));
            font.setPixelSize(pText->m_fontSize);
            font.setBold(pText->m_fontBold);
            font.setItalic(pText->m_fontItalic);
            dlg->setFont(font);
        } else {
            dlg->setUseLabelFont(true);
        }
        dlg->setAlign(pText->m_align);
        if(dlg->exec()) {
            pText->setText(dlg->text().toLocal8Bit());
            if(dlg->useLabelFont()) {
                pText->setFontName("");
            } else {
                pText->setFontSize(dlg->font().pixelSize());
                pText->setFontBold(dlg->font().bold());
                pText->setFontItalic(dlg->font().italic());
                pText->setFontName(dlg->font().family().toLocal8Bit());
            }
            pText->m_align = dlg->Align();
            delete dlg;
            return true;
        }
        delete dlg;
        return false;
    } else {
        prop_ptn *dlg = new prop_ptn(this,0,true);
        int i;
        QString str;
        for(i = 0; i <= 5; i++) {
            str.setNum(i);
            dlg->comboWidth->addItem(str);
        }
        int width = pPtn->m_width;
        if(width < 0) width = 0;
        else if(width > 5) width = 5;
        dlg->comboWidth->setCurrentIndex(width);

        dlg->checkDot->setChecked(pPtn->m_style);
        if( pPtn->id() == VECTPTN_PG || pPtn->id() == VECTPTN_C) {
            dlg->checkFill->setChecked(pPtn->m_nFill != -1);
        } else {
            dlg->checkFill->setEnabled(false);
        }

        if(dlg->exec()) {
            pPtn->m_style = ( dlg->checkDot->isChecked() ? 1 : 0);
            pPtn->m_nFill = ( dlg->checkFill->isChecked() ? 1 : -1);
            pPtn->m_width = dlg->comboWidth->currentIndex();
            delete dlg;
            return true;
        }
        delete dlg;
        return false;
    }
}


//m_docに展開した部品情報を m_pCompIndexに戻す
bool SComponentView::save()
{
    if(!m_pCompIndex) return false;
    bool updateFlag = m_doc.releaseTemp();
    if(m_doc.m_dirtyFlag) {
        saveCompInfo();
    }
    if(updateFlag) {
        drawToBuff(true);
        update();    //画面の更新を強制
    }
    return true;
}


//tempデータに保持したデータを、一般展開データに戻す
bool SComponentView::releaseTemp()
{
    if(m_doc.releaseTemp()) {
        drawToBuff(true);
        update();    //画面の更新を強制
        return true;
    } else {
        return     false;
    }
}




//部品情報の設定
void SComponentView::setCompIndex(SCompIndex* pCompIndex)
{
    m_pCompIndex = pCompIndex;
    if(m_pCompIndex) {
        m_doc.loadCompInfo(m_pCompIndex->m_pCompInfo);
    } else {
        m_doc.init();
    }
    setPageSize();
    m_block =0;
    delete m_pCursorPin;
    m_pCursorPin = NULL;
    //setTool(TOOL_SELECT);
    drawToBuff(true);
    update();    //画面の更新を強制
}

//部品情報の保存
void SComponentView::saveCompInfo()
{
    if(m_pCompIndex) {
        m_doc.saveCompInfo(m_pCompIndex->m_pCompInfo);
    }
}




//ピンの描画
void SComponentView::drawPin(QPainter* p,SPin* pPin,int block,unsigned dwMode)
{
    if(!pPin)return;
    SPoint pinLoc;
    SPoint pinEnd;
    if(pPin->block() <= block) block = pPin->block()-1;
    SSize sz = m_pCompIndex->size();
    int offset = pPin->offset()*10;
    int ltrb = pPin->ltrb();
    switch(ltrb&3) {
    case 0:
        pinLoc.setX(0);
        pinLoc.setY(offset);
        break;
    case 1:
        pinLoc.setX(offset);
        pinLoc.setY(0);
        break;
    case 2:
        pinLoc.setX(sz.w()*10);
        pinLoc.setY(offset);
        break;
    case 3:
        pinLoc.setX(offset);
        pinLoc.setY(sz.h()*10);
        break;
    }

    g_drawComponentPin(
        p,    	    	//描画先
        pinLoc,    	    //ピン端座標
        ltrb&3,    	    //LTRBプロパティ
        pPin->type(),    	//ピンの種類
        dwMode,    	//描画モード
        m_magDiv,    	//縮小率
        m_magMul);    	//倍率


    pinEnd = pinLoc;
    switch(ltrb&3) {
    case 0:
        pinEnd.setX(-10);
        break;
    case 1:
        pinEnd.setY(-10);
        break;
    case 2:
        pinEnd.setX(sz.w()*10+10);
        break;
    case 3:
        pinEnd.setY(sz.h()*10+10);
        break;
    }


    QColor colText;
    QColor colHidePin;
    switch(dwMode & 0xffff) {
    case DRAW_ON:
        colText = QColor("blue");
        colHidePin =    QColor("red");
        break;
    case DRAW_MONO:
        colText = Qt::color1;
        colHidePin =    Qt::color1;
        break;
    case DRAW_OFF:
        colText = QColor("white");
        colHidePin =QColor("white");
        break;
    case DRAW_TEMP:
    default:
        colText = COL_XOR;
        colHidePin = COL_XOR;
        break;
        //    case DRAW_XOR:    colText = QColor("red");    break;
    }


    SRect rc;
    rc = g_drawComponentPinNameArea(pinEnd,ltrb,pPin->pinName());
    g_drawComponentPinName(p,rc,ltrb,dwMode,colText,pPin->pinName(),m_magDiv,m_magMul);
    rc = g_drawComponentPinNumArea(pinEnd,ltrb,pPin->pinNum(0),pPin->type());
    if((pPin->type() & PIN_TYPE_NUMHIDE) == 0) {
        g_drawComponentPinNum(p,rc,ltrb,dwMode,colText,pPin->pinNum(0),m_magDiv,m_magMul);
    } else {
        g_drawComponentPinNum(p,rc,ltrb,dwMode,colHidePin,pPin->pinNum(0),m_magDiv,m_magMul);
    }
}

//リスト構造上のピンをまとめて描画
void SComponentView::drawPins(QPainter* p,SCompEditElmList& list,unsigned dwMode)
{
    SCompEditElmListIterator ite;
    ite = list.begin();
    while(ite != list.end()) {
        if(ite->m_deleteOpCount < 0) {
            if(ite->m_dataType == SCompEditDoc::DATATYPE_PIN) {
                SPin* pPin =(SPin*)ite->m_pData;
                drawPin(p,pPin,m_block,dwMode);
            }
        }
        ite++;
    }
}

void SComponentView::drawPtns(QPainter* p,SCompEditElmList& list,unsigned dwMode)
{
    if(list.size() <= 0)return;
    if(!m_pCompIndex)return;

    QColor col;

    if(dwMode & DRAW_ON)    	col = COL_PART;
    else    	    	    	    	col = COL_XOR;


    SSize sz = m_pCompIndex->size();
    int cx = sz.w()*10;
    int cy = sz.h()*10;

    SCompEditElmListIterator ite;
    ite = list.begin();
    while(ite != list.end()) {
        if(ite->m_deleteOpCount < 0) {
            if(ite->m_dataType == SCompEditDoc::DATATYPE_PTN) {
                SPtnObj* pPtn =(SPtnObj*)ite->m_pData;
                drawPtn(p,pPtn,cx,cy,col,m_magDiv,m_magMul);
            }
        }
        ite++;
    }
}


void SComponentView::drawPtn(QPainter* p,SPtnObj* pPtn,int cx,int cy,const QColor& col ,int magDiv,int magMul)
{

    //qDebug("drawPtn %d %d %d",pPtn->id(),cx,cy);
    switch(pPtn->id()) {
    case VECTPTN_L:
        g_DrawPtnLine(
            p,    	    	    //描画先
            (SPtnObjLine*)pPtn,    //描画オブジェクト
            col,    	    	    //描画色
            0,    	    	    //int     dir,
            cx,    	    	    //int     cx,    部品の幅
            cy,    	    	    //int  cy,    部品の高さ
            0,    	    	    //int     x0,    部品の位置
            0,    	    	    //int  y0,    部品の位置
            magDiv,    	    	//表示縮小率
            magMul);    	    //表示倍率
        break;

    case VECTPTN_C:
        g_DrawPtnCircle(
            p,    	    	    //描画先
            (SPtnObjCircle*)pPtn,    //描画オブジェクト
            col,    	    	    //描画色
            0,
            cx,
            cy,
            0,
            0,
            magDiv,    	    	//表示縮小率
            magMul);    	    //表示倍率
        break;

    case VECTPTN_PG:
        g_DrawPtnPolygon(
            p,    	    	    	//描画先
            (SPtnObjPolygon*)pPtn,    //描画オブジェクト
            col,    	    	    	//描画色
            0,    	    	    	//int     dir,
            cx,    	    	    	//int     cx,
            cy,    	    	    	//int  cy,
            0,    	    	    	//int     x0,
            0,    	    	    	//int  y0,
            magDiv,    	    	    //表示縮小率
            magMul);    	    	//表示倍率
        break;

    case VECTPTN_AR:
        g_DrawPtnArc(
            p,    	    	    	//描画先
            (SPtnObjArc*)pPtn,    	// pObj,    	//描画オブジェクト
            col,    	    	    	//描画色
            0,    	    	    	//int     dir,
            cx,    	    	    	//int     cx,
            cy,    	    	    	//int  cy,
            0,    	    	    	//int     x0,
            0,    	    	    	//int  y0,
            magDiv,    	    	    //表示縮小率
            magMul);    	    	//表示倍率
        break;

    case VECTPTN_TXT:
        g_DrawPtnText(
            p,    	    	    	//描画先
            (SPtnObjText*)pPtn,    	// pObj,    	//描画オブジェクト
            col,    	    	    	//描画色
            0,    	    	    	//int     dir,
            cx,    	    	    	//int     cx,
            cy,    	    	    	//int  cy,
            0,    	    	    	//int     x0,
            0,    	    	    	//int  y0,
            magDiv,    	    	    //表示縮小率
            magMul);    	    	//表示倍率
        break;

    }
}


void SComponentView::drawCompFrame(QPainter* p)
{
    if(!m_pCompIndex) return;

    bool usePtn = false;
    if(m_pCompIndex->m_pCompInfo->m_pPtn) {
        SPtn *pPtn = m_pCompIndex->m_pCompInfo->m_pPtn;
        usePtn = true;
        if(pPtn->m_pBuff) {
            g_put_bmp(
                p,    	    	    //出力先
                0,
                0,    	    	    //出力位置
                pPtn->m_pBuff,    	//出力するビットマップ(ワードアラインメント モノクロ)
                pPtn->m_size.w(),    //ピクセル幅
                pPtn->m_size.h(),    	//ピクセル高
                0,    	    	    //向き
                m_magDiv,    	    //
                m_magMul,    	    //
                QColor(128,255,255),    	//出力色
                DRAW_ON    	    	//描画モード
            );
        }
    }


    SSize sz = m_pCompIndex->size();
    QPoint ptCompRightBottom = QPoint(sz.w()*10,sz.h()*10);
    CompEditPtoLP(ptCompRightBottom);

    QColor colLine;
    int width;

    if(usePtn) {
        width = 0;
        colLine = QColor(192,192,192);
    } else {
        width = 1*m_magMul / m_magDiv;
        colLine = COL_PART;
    }


    QPen pen(colLine,width,Qt::SolidLine);
    p->setPen(pen);

    p->drawLine(0,0,ptCompRightBottom.x(),0);
    p->drawLine(ptCompRightBottom.x(),0,ptCompRightBottom.x(),ptCompRightBottom.y());
    p->drawLine(ptCompRightBottom.x(),ptCompRightBottom.y(),0,ptCompRightBottom.y());
    p->drawLine(0,ptCompRightBottom.y(),0,0);

}

void SComponentView::drawGrid(QPainter* p)
{
    //QPoint ptLeftTop(contentsX()-m_windowOrg.x(),contentsY()-m_windowOrg.y());
    QPoint ptLeftTop(0-m_margin,0-m_margin);
    QPoint ptRightBottom = ptLeftTop + QPoint(m_clientSize.width(),m_clientSize.height());
    LPtoCompEditP(ptLeftTop);
    LPtoCompEditP(ptRightBottom);
    int x,y;
#ifdef Q_WS_MACX
    bool grid10only = (m_magMul/m_magDiv < 10);
#else
    bool grid10only = (m_magMul/m_magDiv < 5);
#endif

#ifdef QT_LCOV_LINEGRID
    bool grid10;
    QPen penM(QColor(255,192,192),1);
    QPen penS(QColor(192,192,192),1);
    int nBegin,nEnd;
    nBegin = ptLeftTop.x()-1;
    nEnd = ptRightBottom.x()+1;
    for(y = ptLeftTop.y(); y <= ptRightBottom.y(); y++) {
        grid10 =  (y%10 == 0);
        if(!grid10 && grid10only) continue;
        QPoint ptBegin(nBegin,y);
        CompEditPtoLP(ptBegin);
        QPoint ptEnd(nEnd,y);
        CompEditPtoLP(ptEnd);
        if(grid10) {
            p->setPen(penM);
        } else {
            p->setPen(penS);
        }
        p->drawLine(ptBegin,ptEnd);
    }

    nBegin = ptLeftTop.y()-1;
    nEnd = ptRightBottom.y()+1;
    for(x = ptLeftTop.x(); x <= ptRightBottom.x(); x++) {
        grid10 =  (x%10 == 0);
        if(!grid10 && grid10only) continue;
        QPoint ptBegin(x,nBegin);
        CompEditPtoLP(ptBegin);
        QPoint ptEnd(x,nEnd);
        CompEditPtoLP(ptEnd);
        if(grid10) {
            p->setPen(penM);
        } else {
            p->setPen(penS);
        }
        p->drawLine(ptBegin,ptEnd);
    }


#else
    bool yGrid10;
    for(y = ptLeftTop.y(); y <= ptRightBottom.y(); y++) {
        yGrid10 =  (y%10 == 0);
        if(!yGrid10 && grid10only) continue;
        for(x = ptLeftTop.x(); x <= ptRightBottom.x(); x++) {
            if((yGrid10 && (x%10 == 0))||(!grid10only &&(yGrid10 || (x%10 == 0)))) {
                QPoint pt = QPoint(x,y);
                CompEditPtoLP(pt);
                p->setPen(QColor(0,0,255));
                p->drawPoint(pt);
            } else if(!grid10only) {
                QPoint pt = QPoint(x,y);
                CompEditPtoLP(pt);
                p->setPen(QColor(192,192,192));
                p->drawPoint(pt);
            }
        }
    }
#endif
}

void SComponentView::drawGrip(QPainter* p)
{
    SCompEditElmListIterator ite;
    if(m_doc.m_listTemp.size() != 1 || !m_editNodeMode) return;
    ite = m_doc.m_listTemp.begin();
    if(ite->m_dataType != SCompEditDoc::DATATYPE_PTN) return;
    SPtnObj* pObj =(SPtnObj*)ite->m_pData;
    int nodes = pObj->gripCount();
    int i;

    p->setBrush( QColor(0,0,0) );
    p->setPen( QColor(0,0,0) );

    for(i = 0; i < nodes; i++) {
        SPoint spt = pObj->gripPos(i);
        QPoint pt(spt.x(),spt.y());
        CompEditPtoLP(pt);
        p->drawRect(pt.x()-3,pt.y()-3,6,6);
        //    	p->drawRect(pt.x()-3,pt.y()-3,7,7);
        if(i == 0) {
            p->setBrush( QColor(255,255,255) );
        }
    }
}



int SComponentView::doSelect(SPoint sptEdit,QPoint lpt)
{
    if(m_doc.m_listMain.size() <= 0)return 0;
    if(!m_pCompIndex)return 0;
    int selCount = m_doc.doSelect(sptEdit);
    if(selCount > 0) return selCount;


    QPoint offScrnBuffOrg = m_windowOrg;
    QPixmap bmp(32,32);
    QImage img;

    bmp.fill(QColor(0,0,0));
    QPainter painter;
    painter.begin(&bmp);

    painter.translate(16-lpt.x(),16-lpt.y());
    //    p->setPen(QColor(0,0,0));
    //    p->drawPoint(lpt);



    QColor col = COL_EDITPTN;

    SSize sz = m_pCompIndex->size();
    int cx = sz.w()*10;
    int cy = sz.h()*10;

    SCompEditElm elm;
    SCompEditElmListRevIterator rite;
    rite = m_doc.m_listMain.rbegin();
    while(rite != m_doc.m_listMain.rend()) {
        if(rite->m_deleteOpCount < 0 && rite->m_dataType == SCompEditDoc::DATATYPE_PTN) {
            SPtnObj* pPtn =(SPtnObj*)rite->m_pData;
            drawPtn(&painter,pPtn,cx,cy,col,m_magDiv,m_magMul);
            //COLORREF colpt = RGB(0,0,0);
            img = bmp.toImage();
            bool bHit = false;
            int nx,ny;
            int tx,ty;
            ty = 15;
            for(ny = 0; (ny < 3) && !bHit ; ny++) {
                tx = 15;
                for(nx = 0; (nx < 3) && !bHit; nx++) {
                    if(img.pixel(tx,ty) != qRgb(0,0,0)) {
                        bHit = true;
                        break;
                    }
                    tx++;
                }
                ty++;
            }
            if(bHit) {
                int opCount = m_doc.opCount();
                rite->m_deleteOpCount = opCount;    //m_nOpCount;
                SPtnObj* pNewPtn = pPtn->duplicate();
                elm.m_dataType = SCompEditDoc::DATATYPE_PTN;
                elm.m_placeOpCount = opCount;    //m_nOpCount;
                elm.m_deleteOpCount = -1;
                elm.m_pOrigin = &(*rite);
                elm.m_pData = pNewPtn;
                m_doc.m_tempDirtyFlag = false;
                m_doc.m_listTemp.push_back(elm);
                selCount = 1;
                break;
            }
        }
        rite++;
    }
    painter.end();
    return selCount;
}

void SComponentView::setZoomValue(int n)
{
    if(n < MIN_ZOOMNUM) n = MIN_ZOOMNUM;
    if(n > MAX_ZOOMNUM) n = MAX_ZOOMNUM;
    static int aMagV[MAX_ZOOMNUM+1] = {1,2,5,10};
    m_magMul = aMagV[n];
    m_magDiv = 1;
    m_zoomNum = n;
    m_margin=(PAGE_MARGIN*m_magMul)/m_magDiv;
    setPageSize();
}

//座標変換 論理座標と部品編集座標の相互変換
void SComponentView::LPtoCompEditP(QPoint& point)
{
    point.setX( point.x()*m_magDiv/m_magMul);
    point.setY( point.y()*m_magDiv/m_magMul);
}

void SComponentView::LPtoCompEditP(QRect& rc)
{
    int left       	= rc.left()*m_magDiv/m_magMul;
    int top          = rc.top()*m_magDiv/m_magMul;
    int width      = rc.width()*m_magDiv/m_magMul;
    int height     = rc.height()*m_magDiv/m_magMul;
    rc = QRect(left,top,width,height);
}

void SComponentView::CompEditPtoLP(QPoint& point)
{
    point.setX( point.x()*m_magMul/m_magDiv);
    point.setY( point.y()*m_magMul/m_magDiv);
}

void SComponentView::CompEditPtoLP(QRect& rc)
{
    int left       	= rc.left()*m_magMul/m_magDiv;
    int top          = rc.top()*m_magMul/m_magDiv;
    int width      = rc.width()*m_magMul/m_magDiv;
    int height     = rc.height()*m_magMul/m_magDiv;
    rc = QRect(left,top,width,height);
}

//ピンの配置。既存ピンがなければm_pCursorPinをTempDataに配置する。
bool SComponentView::placePin()
{
    if(m_pCompIndex == NULL) return false;
    if(m_pCursorPin == NULL) return false;
    if(m_doc.pinConflict(m_pCursorPin)) return false;

    setWindowOrg();
    SPoint sptPin =m_doc.pinPosition(m_pCursorPin->ltrb(),m_pCursorPin->offset());
    QPoint ptPin(sptPin.x(),sptPin.y());
    QPoint ptLT(0-m_margin,0-m_margin);
    QRect rcClient = QRect(ptLT,m_clientSize);
    LPtoCompEditP(rcClient);
    if(!rcClient.contains(ptPin)) return false;
    //操作カウントをインクリメントして、一時データに配置
    m_doc.incrementOpCount();
    m_pCursorPin->setBlock(m_pCompIndex->block());
    m_doc.addPinToTemp(m_pCursorPin);
    m_doc.setupTempArea();
    m_doc.m_tempDirtyFlag = true;
    if(m_tool == TOOL_SELECT) {
        delete m_pCursorPin;
        m_pCursorPin = NULL;
    }
    return true;

}

void SComponentView::drawPlaceLine(QPainter* p)
{
    int xs,ys,x0,y0,x1,y1;

    int width = 0;

    QPen pen(QColor(255,0,0),width,Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen);

    x0 = m_arrayTempPoint[0].x();
    y0 = m_arrayTempPoint[0].y();

    xs = x0 = (x0*m_magMul)/m_magDiv;
    ys = y0 = (y0*m_magMul)/m_magDiv;

    for(int n = 1; n <= m_placeState; n++) {
        x1 = m_arrayTempPoint[n].x();
        y1 = m_arrayTempPoint[n].y();

        x1 = (x1*m_magMul)/m_magDiv;
        y1 = (y1*m_magMul)/m_magDiv;
        p->drawLine(x0,y0,x1,y1);
        x0 = x1;
        y0 = y1;
    }
    if(m_placeState > 1) {
        p->drawLine(x0,y0,xs,ys);
    }
}

void SComponentView::drawPlaceCircle(QPainter* p)
{
    int x0,y0,x1,y1;

    int width = 0;
    QPen pen(QColor(255,0,0),width,Qt::SolidLine);
    p->setPen(pen);
    p->setBrush( Qt::NoBrush );

    x0 = m_arrayTempPoint[0].x();
    y0 = m_arrayTempPoint[0].y();

    x0 = (x0*m_magMul)/m_magDiv;
    y0 = (y0*m_magMul)/m_magDiv;

    QPoint p0(x0,y0);

    x1 = m_arrayTempPoint[1].x();
    y1 = m_arrayTempPoint[1].y();

    x1 = (x1*m_magMul)/m_magDiv;
    y1 = (y1*m_magMul)/m_magDiv;

    QPoint p1(x1,y1);

    QRect rc(p0,p1);
    rc.normalized();


    p->drawEllipse(rc.left(),rc.top(),rc.width(),rc.height());
    //    p->drawEllipse(rc.left(),rc.top(),rc.width()+1,rc.height()+1);
}

void SComponentView::drawPlaceArc(QPainter* p)
{
    //    	CPen newPen;
    //    CPen* pPenOld;

    int xc,yc,r,x0,y0,x1,y1,x2,y2,x3,y3;

    int width = 0;


    xc = m_arrayTempPoint[0].x();
    yc = m_arrayTempPoint[0].y();
    r =abs(m_arrayTempPoint[0].x()-m_arrayTempPoint[1].x());

    x0 = xc-r;
    y0 = yc-r;
    x1 = xc+r;
    y1 = yc+r;

    x0 = (x0*m_magMul)/m_magDiv;
    y0 = (y0*m_magMul)/m_magDiv;
    x1 = (x1*m_magMul)/m_magDiv;
    y1 = (y1*m_magMul)/m_magDiv;

    QPoint p0(x0,y0);
    QPoint p1(x1,y1);
    QRect rc(p0,p1);
    rc.normalized();


    //    CRect rc(x0,y0,x1,y1);
    //    rc.NormalizeRect();
    //    rc.right++;
    //    rc.bottom++;

    QPen penC(QColor(220,220,220),width,Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin);
    p->setPen(penC);
    p->setBrush( Qt::NoBrush );
    p->drawEllipse(rc.left(),rc.top(),rc.width(),rc.height());
    //    p->drawEllipse(rc.left(),rc.top(),rc.width()+1,rc.height()+1);


    //newPen.CreatePen(PS_SOLID,width,RGB(220,220,220));
    //pPenOld = pDC->SelectObject(&newPen);
    //CBrush* pBrushOld=(CBrush*)pDC->SelectStockObject(NULL_BRUSH);
    //    pDC->Ellipse(&rc);
    //    pDC->SelectObject(pBrushOld);
    //    pDC->SelectObject(pPenOld);
    //    newPen.DeleteObject();

    if(m_placeState >= 2) {
        x2 = m_arrayTempPoint[2].x();
        y2 = m_arrayTempPoint[2].y();
        if(m_placeState == 2) {
            m_arrayTempPoint[3].setX(xc-(x2-xc));
            m_arrayTempPoint[3].setY(yc-(y2-yc));
        }
        x3 = m_arrayTempPoint[3].x();
        y3 = m_arrayTempPoint[3].y();

        x2 = (x2*m_magMul)/m_magDiv;
        y2 = (y2*m_magMul)/m_magDiv;
        x3 = (x3*m_magMul)/m_magDiv;
        y3 = (y3*m_magMul)/m_magDiv;

        QPoint ptBegin(x2,y2);
        QPoint ptEnd(x3,y3);

        QPen pen(QColor(255,0,0),width,Qt::SolidLine,Qt::RoundCap, Qt::RoundJoin);
        p->setPen(pen);

        //開始角終了角の計算をSPtnObjArcで行なう
        SPtnObjArc ptnArc;
        ptnArc.m_ptC = m_arrayTempPoint[0];
        ptnArc.m_nR = abs(m_arrayTempPoint[0].x()-m_arrayTempPoint[1].x());
        ptnArc.setBeginPoint(m_arrayTempPoint[2].x(),m_arrayTempPoint[2].y());
        ptnArc.setEndPoint(m_arrayTempPoint[3].x(),m_arrayTempPoint[3].y());

        int a = ptnArc.m_nBegin;
        int ae = ptnArc.m_nEnd;
        if(ae < a) ae += 360*16;
        int alen = ae-a;

        p->drawArc(rc.left(),rc.top(),rc.width(),rc.height(),a,alen);
        //    	p->drawArc(rc.left(),rc.top(),rc.width()+1,rc.height()+1,a,alen);

        //    	newPen.CreatePen(PS_SOLID,width,RGB(255,0,0));
        //    	pPenOld = pDC->SelectObject(&newPen);
        //    	pDC->Arc(&rc,ptBegin,ptEnd);
        //    	pDC->SelectObject(pPenOld);
        //    	newPen.DeleteObject();
    }

}

bool editNodeActive()
{
    return false;
}

int SComponentView::isOnGrip(const QPoint& logicalPoint)
{
    int x = logicalPoint.x();
    int y = logicalPoint.y();

    if(m_doc.m_listTemp.size() != 1) return -1;
    SCompEditElmListIterator ite;
    ite = m_doc.m_listTemp.begin();
    if(ite->m_dataType != SCompEditDoc::DATATYPE_PTN) return -1;
    SPtnObj* pObj =(SPtnObj*)ite->m_pData;
    int nodes = pObj->gripCount();
    int i;
    for(i = 0; i < nodes; i++) {
        SPoint sptGrip = pObj->gripPos(i);
        QPoint ptGrip(sptGrip.x(),sptGrip.y());
        CompEditPtoLP(ptGrip);
        if(abs(x-ptGrip.x()) <= 3 && abs(y-ptGrip.y()) <= 3) return i;
    }
    return -1;
}

void SComponentView::OnViewZoomin()
{
    setZoomValue(m_zoomNum+1);
    update();
}

void SComponentView::OnViewZoomout()
{
    setZoomValue(m_zoomNum-1);
    update();
}


void  SComponentView::mousePressEvent(QMouseEvent*me)
{

    QPoint logicalPoint(me->x()-m_margin,me->y()-m_margin); //表示している部品の左上を(0,0)とする座標
    QPoint ptCompEdit = logicalPoint;
    LPtoCompEditP(ptCompEdit);    	//部品編集座標に変換
    //qDebug("contentsMousePressEvent %d,%d--%d,%d",me->x(),me->y(),logicalPoint.x(),logicalPoint.y());

    bool bUpdateView = false;

    m_pointLButtonDown = ptCompEdit;
    SPoint sptCompEdit = SPoint(ptCompEdit.x(),ptCompEdit.y());

    m_movingGrip = -1;

    if(m_pCompIndex != NULL && m_pCompIndex->pattern() != NULL && m_doc.m_listTemp.size() == 0) {
        SPtnObj* pPtnObj = NULL;
        if(m_tool == TOOL_TEXT) {
            editptntextdlg *dlg = new editptntextdlg(this);
            if(dlg->exec()) {
                if(dlg->text().count()>0) {
                    pPtnObj = new SPtnObjText;
                    ((SPtnObjText*)pPtnObj)->m_p0 = sptCompEdit;
                    ((SPtnObjText*)pPtnObj)->m_align = dlg->Align();
                    ((SPtnObjText*)pPtnObj)->setText(dlg->text().toLocal8Bit());
                    if(dlg->useLabelFont()) {
                        ((SPtnObjText*)pPtnObj)->setFontName("");
                    } else {
                        ((SPtnObjText*)pPtnObj)->setFontSize(dlg->font().pixelSize());
                        ((SPtnObjText*)pPtnObj)->setFontBold(dlg->font().bold());
                        ((SPtnObjText*)pPtnObj)->setFontItalic(dlg->font().italic());
                        ((SPtnObjText*)pPtnObj)->setFontName(dlg->font().family().toLocal8Bit());
                    }
                    bUpdateView = true;
                }
                m_placeState = 0;
            }
            delete dlg;
        } else if(m_placeState == 0) {
            if(m_tool == TOOL_LINE || m_tool == TOOL_POLYGON || m_tool == TOOL_CIRCLE || m_tool == TOOL_ARC) {
                m_arrayTempPoint[0] = sptCompEdit;
                m_arrayTempPoint[1] = sptCompEdit;
                m_placeState = 1;
                bUpdateView = true;
            }
        } else {
            if(m_tool == TOOL_LINE) {
                m_arrayTempPoint[1] = sptCompEdit;
                pPtnObj = new SPtnObjLine;
                ((SPtnObjLine*)pPtnObj)->m_p0 = m_arrayTempPoint[0];
                ((SPtnObjLine*)pPtnObj)->m_p1 = m_arrayTempPoint[1];
                m_placeState = 0;
                bUpdateView = true;
            } else if(m_tool == TOOL_POLYGON) {
                if(m_placeState < VECTPLOYGON_MAXNODE-2) {
                    m_arrayTempPoint[m_placeState++] = sptCompEdit;
                    m_arrayTempPoint[m_placeState] = sptCompEdit;
                    bUpdateView = true;
                }
            } else if(m_tool == TOOL_CIRCLE) {
                m_arrayTempPoint[1] = sptCompEdit;
                pPtnObj = new SPtnObjCircle;
                ((SPtnObjCircle*)pPtnObj)->m_p0 = m_arrayTempPoint[0];
                ((SPtnObjCircle*)pPtnObj)->m_p1 = m_arrayTempPoint[1];
                m_placeState = 0;
                bUpdateView = true;
            } else if(m_tool == TOOL_ARC) {
                if(m_placeState < 3) {
                    m_arrayTempPoint[m_placeState++] = sptCompEdit;
                    m_arrayTempPoint[m_placeState] = sptCompEdit;
                    bUpdateView = true;
                } else {
                    m_arrayTempPoint[3] = sptCompEdit;
                    pPtnObj = new SPtnObjArc;
                    ((SPtnObjArc*)pPtnObj)->m_ptC = m_arrayTempPoint[0];
                    ((SPtnObjArc*)pPtnObj)->m_nR = abs(m_arrayTempPoint[0].x()-m_arrayTempPoint[1].x());
                    ((SPtnObjArc*)pPtnObj)->setBeginPoint(m_arrayTempPoint[2].x(),m_arrayTempPoint[2].y());
                    ((SPtnObjArc*)pPtnObj)->setEndPoint(m_arrayTempPoint[3].x(),m_arrayTempPoint[3].y());
                    m_placeState = 0;
                    bUpdateView = true;
                }
            }
        }
        if(pPtnObj) {
            m_doc.incrementOpCount();
            m_doc.addPtnToTemp(pPtnObj);
            m_doc.setupTempArea();
            delete pPtnObj;
            pPtnObj = NULL;
        }
    }
    if(bUpdateView) {
        update();
        return;
    }
    //↑ここまでで再描画をともなう処理を行った場合はあとの処理に進まず戻る。




    if(m_pCursorPin != NULL && m_doc.m_listTemp.size() == 0) {
        int pinLtrb;
        int pinOffset;
        m_doc.pinPlacePoint(sptCompEdit,pinLtrb,pinOffset);
        m_pCursorPin->setLtrb(pinLtrb);
        m_pCursorPin->setOffset(pinOffset);
        if(m_tool == TOOL_PIN) {
            m_pCursorPin->setPinName("NAME");
            m_pCursorPin->setPinNum(0,"NUM");
        }
        if(placePin()) {
            bUpdateView = true;
        }
        if(m_tool == TOOL_PIN) {
            m_pCursorPin->setPinName("");
            m_pCursorPin->setPinNum(0,"");
            m_pCursorPin->setBlock(1);
        }
    } else if(editNodeActive() && (m_movingGrip = isOnGrip( logicalPoint)) >= 0) {

    } else if(!editNodeActive() && m_doc.isOnTmpData(sptCompEdit)) {
        m_prevOffset = QSize(0,0);    	//ドラッグ中にすでに移動を確定した量
        if(m_doc.m_listTemp.size() == 1) {    //単独選択
            SCompEditElmListIterator ite;
            ite = m_doc.m_listTemp.begin();
            if(ite->m_dataType == SCompEditDoc::DATATYPE_PIN) {
                SPin* pPin =(SPin*)ite->m_pData;
                //ピンのドラッグをはじめる前の位置
                m_pinPosLtrbStart    = pPin->ltrb();
                m_pinPosOffsetStart    = pPin->offset();
                m_dragOpStatus = DRAGOP_PIN;
            } else {
                m_dragOpStatus = DRAGOP_PTN;
            }
        } else { //複数同時選択
            m_dragOpStatus = DRAGOP_PTN;
        }
    } else {
        if(m_doc.releaseTemp()) {
            bUpdateView = true;
            drawToBuff(true);
        } else if(m_tool == TOOL_SELECT) {
            m_startSelect = true;
        }
    }

    if(bUpdateView) {
        update();
    }
}

void  SComponentView::mouseReleaseEvent(QMouseEvent *me)
{
    QPoint point(me->x()-m_margin,me->y()-m_margin); //表示している部品の左上を(0,0)とする座標
    QPoint ptCompEdit = point;
    LPtoCompEditP(ptCompEdit);    	//部品編集座標に変換

    bool bUpdateView = false;

    m_pointLButtonUp = ptCompEdit;
    SPoint sptCompEdit = SPoint(ptCompEdit.x(),ptCompEdit.y());

    int movX = m_pointLButtonUp.x() - m_pointLButtonDown.x();
    int movY = m_pointLButtonUp.y() - m_pointLButtonDown.y();


    //if(m_dragOpStatus == DRAGOP_PIN){
    if(m_doc.m_listTemp.size() == 1) {    //単独選択
        SCompEditElmListIterator ite;
        ite = m_doc.m_listTemp.begin();
        if(m_dragOpStatus == DRAGOP_PIN) {
            SPin* pPin =(SPin*)ite->m_pData;
            if(m_doc.pinConflict(pPin)) {
                pPin->setLtrb(m_pinPosLtrbStart);
                pPin->setOffset(m_pinPosOffsetStart);
                m_doc.setupTempArea();
                bUpdateView = true;
            }
        }
    } else if(m_tool == TOOL_SELECT && m_startSelect) {
        if(!m_dragOpStatus) {
            m_doc.incrementOpCount();
            int selCount = 0;
            if(abs(movX)<3 && abs(movY)<3) {
                //selCount = m_doc.doSelect(sptCompEdit);
                selCount = doSelect(sptCompEdit,point);
            } else {
                SPoint ptStart(m_pointLButtonDown.x(),m_pointLButtonDown.y());
                SRect rcSelArea(ptStart,sptCompEdit);
                rcSelArea.normalize();//Version 0.21
                selCount = m_doc.doSelect(rcSelArea);
            }
            if(selCount) {
                m_doc.setupTempArea();
                drawToBuff(true);

            }
        }
        bUpdateView = true;
    }

    m_startSelect = false;
    m_movingGrip = -1;

    m_dragOpStatus = DRAGOP_NOT;
    if(bUpdateView) {
        update();
    }
}

void  SComponentView::mouseMoveEvent(QMouseEvent *me)
{
    QPoint point(me->x()-m_margin,me->y()-m_margin); //表示している部品の左上を(0,0)とする座標
    QPoint ptCompEdit = point;
    LPtoCompEditP(ptCompEdit);    	//部品編集座標に変換

    bool bUpdateView = false;

    SPoint sptCompEdit = SPoint(ptCompEdit.x(),ptCompEdit.y());

    m_pointCurrentPos = ptCompEdit;

    int pinLtrb;
    int pinOffset;
    //マウスカーソルの形状の決定
    if(editNodeActive()) {
        if(isOnGrip(point) >= 0 || m_movingGrip >= 0) {
            //setCursor(*m_pCurMov);
            setCursor(Qt::ClosedHandCursor);
        } else {
            //setCursor(*m_pCurStd);
            setCursor(Qt::ArrowCursor);
        }
    } else if(m_doc.isOnTmpData(sptCompEdit)) {    //一時データの上か移動中
        //setCursor(*m_pCurMov);
        if(me->buttons())
            setCursor(Qt::ClosedHandCursor);
        else
            setCursor(Qt::OpenHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
        //setCursor(*m_pCurStd);
    }

    if(m_movingGrip >=0) {
        SCompEditElmListIterator ite;
        ite = m_doc.m_listTemp.begin();
        if(ite->m_dataType != SCompEditDoc::DATATYPE_PTN) {
            m_movingGrip = -1;
        } else {
            SPtnObj* pObj =(SPtnObj*)ite->m_pData;
            pObj->setGripPos(m_movingGrip,sptCompEdit.x(),sptCompEdit.y());
            bUpdateView = true;
            m_doc.m_tempDirtyFlag = true;
        }
    }

    if(m_dragOpStatus == DRAGOP_PIN) {
        //    if(m_doc.m_listTemp.size() == 1){    //単独選択
        SCompEditElmListIterator ite;
        ite = m_doc.m_listTemp.begin();
        if(m_doc.m_listTemp.size() == 1) {    //単独選択
            //    	if(m_dragOpStatus == DRAGOP_PIN){
            SPin* pPin =(SPin*)ite->m_pData;
            m_doc.pinPlacePoint(sptCompEdit,pinLtrb,pinOffset);
            if(pinLtrb != pPin->ltrb()) {
                bUpdateView = true;
                m_doc.m_tempDirtyFlag = true;
                pPin->setLtrb(pinLtrb);
            }
            if(pinOffset != pPin->offset()) {
                bUpdateView = true;
                m_doc.m_tempDirtyFlag = true;
                pPin->setOffset(pinOffset);
            }
        }
    } else if(m_dragOpStatus == DRAGOP_PTN) {
        QPoint totalMoveSize = m_pointCurrentPos - m_pointLButtonDown;
        QPoint moveSize = totalMoveSize - QPoint(m_prevOffset.width(),m_prevOffset.height());;
        m_prevOffset = QSize(totalMoveSize.x(),totalMoveSize.y());
        m_doc.move(moveSize.x(),moveSize.y());
        bUpdateView = true;

    } else if(m_pCursorPin != NULL) {    	//if(m_tool == TOOL_PIN){
        m_doc.pinPlacePoint(sptCompEdit,pinLtrb,pinOffset);
        if(pinLtrb != m_pCursorPin->ltrb()) {
            bUpdateView = true;
            m_pCursorPin->setLtrb(pinLtrb);
        }
        if(pinOffset != m_pCursorPin->offset()) {
            bUpdateView = true;
            m_pCursorPin->setOffset(pinOffset);
        }
    }

    if(m_tool == TOOL_SELECT && m_startSelect) {
        bUpdateView = true;
    }

    if(m_tool == TOOL_LINE || m_tool == TOOL_POLYGON || m_tool == TOOL_CIRCLE || m_tool == TOOL_ARC) {
        if(m_placeState > 0) {
            m_arrayTempPoint[m_placeState] = sptCompEdit;
            bUpdateView = true;
        }
    }

    if(bUpdateView) {
        m_doc.setupTempArea();
        update();
    }
}

void  SComponentView::mouseDoubleClickEvent ( QMouseEvent *me)
{
    QPoint point(me->x()-m_margin,me->y()-m_margin); //表示している部品の左上を(0,0)とする座標
    QPoint ptCompEdit = point;
    LPtoCompEditP(ptCompEdit);    	//部品編集座標に変換

    //CWnd::OnLButtonDblClk(nFlags, point);
    bool bUpdateView = false;
    //    CClientDC dc(this);    //クライアントエリアのデバイスコンテキストを得て
    //    PrepareDC(&dc);    	//デバイスコンテキストにスクロール値の反映
    //    dc.DPtoLP(&point);    //マウスの座標を論理座標に変換
    //    CPoint ptCompEdit = point;
    //    LPtoCompEditP(ptCompEdit);//マウスの座標を編集座標に変換
    SPoint sptCompEdit = SPoint(ptCompEdit.x(),ptCompEdit.y());

    if(canProperty()) {
        if(editProperty()) {
            bUpdateView = true;
        }
    }

    if(m_tool == TOOL_POLYGON && m_placeState >= 2) {
        int count = m_placeState;
        if(count > VECTPLOYGON_MAXNODE) count = VECTPLOYGON_MAXNODE;
        SPtnObj* pPtnObj = new SPtnObjPolygon;
        ((SPtnObjPolygon*)pPtnObj)->m_nCount = count;
        ((SPtnObjPolygon*)pPtnObj)->m_pPt = new SPoint[count];
        int n;
        for(n = 0; n < count; n++) {
            ((SPtnObjPolygon*)pPtnObj)->m_pPt[n] = m_arrayTempPoint[n];
        }
        if(pPtnObj) {
            m_doc.incrementOpCount();
            m_doc.addPtnToTemp(pPtnObj);
            m_doc.setupTempArea();
            delete pPtnObj;
        }
        m_placeState =0;
        bUpdateView = true;
    }

    if(bUpdateView) {
        update();
    }
}

bool SComponentView::editNodeActive()
{
    SCompEditElmListIterator ite;
    if(m_doc.m_listTemp.size() == 1 && m_editNodeMode) {
        ite = m_doc.m_listTemp.begin();
        if(ite->m_dataType == SCompEditDoc::DATATYPE_PTN) {
            return true;
        }
    }
    return false;
}


void  SComponentView::resizeEvent(QResizeEvent *e )
{
    QSize size = e->size();
    //qDebug("%d,%d\n",size.width(),size.height());
    resizeBuffer(size.width(),size.height());
    m_clientSize = size ;
    //Q3ScrollView::resizeEvent(e);
    QWidget::resizeEvent(e);
    drawToBuff(true);
}

void  SComponentView::resizeBuffer(int w,int h)
{
    //m_viewBuffer.resize(w+1, h+1);
    m_viewBuffer = QPixmap(w+1,h+1);
}

void SComponentView::drawTempDataFrame(QPainter*p, const QRect& rc)
{
    //    p->setRasterOp(Qt::XorROP);
    //    QPen pen(QColor("black"),1,DotLine);
    //    p->setPen(pen);

    QPen pen(QColor("gray"));
    p->setPen(pen);
    p->setBrush( Qt::NoBrush );
    p->drawRect(rc);
}


void SComponentView::setWindowOrg()
{
    //    m_margin=(PAGE_MARGIN*m_magMul)/m_magDiv;
    int x = m_margin-0;
    int y = m_margin-0;
    m_windowOrg = QPoint(x,y);
}

//バッファへの書き込み
void  SComponentView::drawToBuff(bool force)
{
    setWindowOrg();

    if(force || m_offScrnBuffOrg !=m_windowOrg) {
        m_offScrnBuffOrg = m_windowOrg;
        m_viewBuffer.fill(Qt::white);
        QPainter painter;
        painter.begin(&m_viewBuffer);

        painter.translate(m_windowOrg.x(),m_windowOrg.y());
        drawGrid(&painter);
        drawCompFrame(&painter);
        drawPtns(&painter,m_doc.m_listMain,DRAW_ON);
        drawPins(&painter,m_doc.m_listMain,DRAW_ON);

        painter.end();
    }
}




// ESCAPEキーハンドラ
bool SComponentView::onKeyEscape()
{
    bool bUpdateView = false;

    if(m_doc.releaseTemp()) {
        bUpdateView = true;
        drawToBuff(true);
    }
    if(m_tool == TOOL_SELECT) {
        if(m_pCursorPin != NULL) {
            delete m_pCursorPin;
            m_pCursorPin = NULL;
            bUpdateView = true;
        }
    }
    if(m_placeState > 0) {
        m_arrayTempPoint[m_placeState-1]=m_arrayTempPoint[m_placeState];
        m_placeState --;
        bUpdateView = true;
    }

    m_movingGrip = -1;

    if(bUpdateView) {
        update();
    }
    return bUpdateView;
}


void SComponentView::paintEvent ( QPaintEvent * event )
{
    //qDebug("paintEvent");
    QPainter painter(this);
    painter.fillRect(QRectF(QPoint(0,0),size()),Qt::white);
    drawContents(&painter,0,0,0,0);
}

//再描画
void  SComponentView::drawContents(QPainter*p,int clipx,int clipy, int clipw, int cliph )
{
    //setWindowOrg();
    //    p->translate(m_margin,m_margin);
    //    drawCompFrame(p);
    //    drawGrid(p);
    //    drawPtns(p,m_doc.m_listMain,DRAW_ON);
    //    drawPins(p,m_doc.m_listMain,DRAW_ON);

    //contentsToViewport ( clipx,clipy,clipx,clipy);
    drawToBuff(false);
    p->setClipping(false);
    p->drawPixmap(0,0,m_viewBuffer);

    //Qt4    bitBlt(viewport(),clipx,clipy,&m_viewBuffer,clipx,clipy,clipw,cliph,Qt::CopyROP,false);
    /*
    drawToBuff(true);
    //PrepareDC(&dc);

    QRect rcClip = QRect(clipx,clipy,clipw,cliph);
    QRect rcClipSheet = ContentsToSheet(rcClip);
    SRect srcClipSheet =SRect(rcClipSheet.left(),rcClipSheet.top(),rcClipSheet.width(),rcClipSheet.height());


    p->setClipping(true);
    contentsToViewport ( clipx,clipy,clipx,clipy);
    p->setClipRect(clipx,clipy,clipw,cliph);

    bitBlt(viewport(),clipx,clipy,&m_viewBuffer,clipx,clipy,clipw,cliph,Qt::CopyROP,false);
    */
    p->translate(m_margin,m_margin);

    drawPins(p,m_doc.m_listTemp,DRAW_TEMP);
    drawPtns(p,m_doc.m_listTemp,DRAW_TEMP);

    if(m_doc.m_listTemp.size()>0) {
        m_doc.setupTempArea();
        QRect rcTemp = QRect(
                           m_doc.m_tempArea.l(),
                           m_doc.m_tempArea.t(),
                           m_doc.m_tempArea.w(),
                           m_doc.m_tempArea.h()
                       );
        CompEditPtoLP(rcTemp);

        if(editNodeActive()) {
            drawGrip(p);
        } else {
            drawTempDataFrame(p, rcTemp);
        }
    }

    if(m_tool == TOOL_SELECT && m_startSelect) {
        QRect rcTemp = QRect(m_pointLButtonDown,m_pointCurrentPos);
        rcTemp.normalized();
        CompEditPtoLP(rcTemp);
        //TRACE("%d,%d,%d,%d\n",rcTemp.left,rcTemp.top,rcTemp.right,rcTemp.bottom);
        QPen pen(QColor("gray"));
        p->setPen(pen);
        p->setBrush( Qt::NoBrush );
        p->drawRect(rcTemp);
        //dc.DrawFocusRect(&rcTemp);    //点線枠の描画
    }


    if(m_pCompIndex != NULL) {
        if(m_pCursorPin != NULL && m_doc.m_listTemp.size() == 0) {
            drawPin(p,m_pCursorPin,0,DRAW_TEMP);
        }
        if(m_tool == TOOL_LINE || m_tool == TOOL_POLYGON ) {
            if(m_placeState > 0) {
                drawPlaceLine(p);
            }
        } else if(m_tool == TOOL_CIRCLE) {
            if(m_placeState > 0) {
                drawPlaceCircle(p);
            }
        } else if(m_tool == TOOL_ARC) {
            if(m_placeState > 0) {
                drawPlaceArc(p);
            }
        }
    }
}


bool SComponentView::doCreateBitPtn()
{
    //return false;

    if(!canCreateBitPtn()) return false;



    //if(m_pCompIndex == NULL) return false;
    //if(m_pCompIndex->m_pCompInfo->m_pPtn){
    //    SPtn *pPtn = m_pCompIndex->m_pCompInfo->m_pPtn;
    //    if(pPtn->m_pBuff) return true;
    //}
    //return false;


    m_doc.releaseTemp();

    if(m_doc.m_listMain.size() <= 0)return false;


    SPtn *pPtn = m_pCompIndex->m_pCompInfo->m_pPtn;
    int width = pPtn->m_size.w();
    int height = pPtn->m_size.h();

    int nBmpXbyte;    //ビットマップの水平方向バイト数
    nBmpXbyte=((width+15)/16)*2;
    int nBmpSize = nBmpXbyte*height;


    QPixmap bmp(width,height);
    QImage img;

    bmp.fill(QColor(0,0,0));
    QPainter painter;
    painter.begin(&bmp);

    QColor col = COL_EDITPTN;

    SSize sz = m_pCompIndex->size();
    int cx = sz.w()*10;
    int cy = sz.h()*10;

    SCompEditElm elm;
    SCompEditElmListRevIterator rite;
    rite = m_doc.m_listMain.rbegin();
    while(rite != m_doc.m_listMain.rend()) {
        if(rite->m_deleteOpCount < 0 && rite->m_dataType == SCompEditDoc::DATATYPE_PTN) {
            SPtnObj* pPtn =(SPtnObj*)rite->m_pData;
            drawPtn(&painter,pPtn,cx,cy,col,1,1);
        }
        rite++;
    }
    painter.end();

    img = bmp.toImage();

    if(!pPtn->m_pBuff) {
        pPtn->m_pBuff=(unsigned char*)new char[nBmpSize];
    }
    unsigned char* pBuff = pPtn->m_pBuff;
    int tx,ty;
    int n;
    for(n = 0; n < nBmpSize; n++) *pBuff++=0;

    for(ty = 0; ty < height; ty++ ) {
        pBuff = pPtn->m_pBuff + nBmpXbyte*ty;
        unsigned char mask = 0x80;
        for(tx = 0; tx < width; tx++) {
            if(img.pixel(tx,ty) != qRgb(0,0,0)) {
                *pBuff |= mask;
            }
            mask = mask>>1;
            if(mask == 0) {
                pBuff++;
                mask = 0x80;
            }
        }
    }

    m_doc.m_dirtyFlag = true;
    drawToBuff(true);
    update();
    return true;
}
