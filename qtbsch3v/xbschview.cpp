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
#include <stdlib.h>
#include <QPainter>
#include <QColor>
#include <QTimer>
//Added by qt3to4:
//#include <QWheelEvent>
#include <QPixmap>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <string>
using namespace std;
#include <QApplication>
#include <QDesktopWidget>
#include <QBitmap>
#include <QCursor>
#include <QMessageBox>
#include <QFont>
#include "cfgdata.h"
#include "coord.h"

#include <QDebug>


#include "xbsch.h"
#include "xbschobj.h"
#include "xbschview.h"
#include "xbschdoc.h"
#include "xbschjunc.h"
#include "xbschline.h"
#include "xbschentry.h"
#include "xbschlabel.h"
#include "xbschtag.h"
#include "xbschcomponent.h"
#include "xbschcomment.h"
#include "drawcomponent.h"

#include "complib.h"

#include "editlabeldialog.h"
#include "edittagdialog.h"
#include "selectcomponentdialog.h"
#include "editcomponentdialog.h"
#include "editcommentdlg.h"
#include "setSheetSize.h"
#include "drawobject.h"
#include "application.h"

#include "xbschglobal.h"

//QWidget * parent = 0, const char * name = 0, WFlags f = 0
//SXBSchView::SXBSchView()
SXBSchView::SXBSchView(QWidget * parent):
    QWidget (parent)
{
    m_fEditHighLight = true;
    QPalette palette;
    palette.setColor(backgroundRole(),Qt::white);
    setPalette(palette);
    //setBackgroundColor(QColor("white"));
    //    viewport()->setAttribute(Qt::WA_PaintOnScreen);    	//Qt4 追加
    //setBackgroundMode(Qt::NoBackground);
    //setAcceptDrops(TRUE);
    //setBackgroundMode(QWidget::PaletteBase);
    //setPalette( QPalette( QColor(0,0,0) ) );

    //QObject::connect(this,SIGNAL(contentsMoving(int,int)),
    //    	    	this,SLOT(whenScroll(int,int)));

    setMouseTracking(true);

    //常にスクロールバーを表示にしていないと、スクロールバーの表示/非表示をともなう
    //表示サイズ変更で、リペイントが2回発生して鬱陶しい。
    //setHScrollBarMode(AlwaysOn);
    //setVScrollBarMode(AlwaysOn);
    setDefaultFont();
    //    setSmallFont(false);

    m_pDoc = new SXBSchDocSch(parent);

    int w,h;
    if(g_cfg.getInt("Sheet","Width",w) && g_cfg.getInt("Sheet","Height",h)) {
        m_pDoc->SetSheetSize(SSize(w,h));
    }
    //m_pDoc->SetView(viewport());


    int width    =m_pDoc->SheetSize().w();
    int height    =m_pDoc->SheetSize().h();
    setMinimumSize(width,height);

    m_toolmode = TOOL_SELECTOR;
    m_bDisplayGrid     	= true;
    m_bTimer     	    = false;
    m_bLButtonDown     	= false;
    m_bDrawSelectFrame     = false;
    m_bDrawTempLine    	= false;
    m_nZoom    = 2;
    setZoomValue();    //  m_viewScale, m_viewScaleMul の設定
    //m_viewScale     	    = 1;    	//表示縮小率の初期値は常に1
    //m_viewScaleMul     	    = 1;    	//表示拡大率の初期値は常に1
    m_pCursorObj     	= NULL;
    m_moveOption    	=0;
    m_bInhibitPlaceObject = false;
    m_bReserveAttribute = false;
    m_bCompleteDrawTemp    = true;

    m_oldContentsX    = -1;
    m_oldContentsY    = -1;
    m_oldOpCount    = -1;
    m_oldViewScale    = -1;
    m_oldViewScaleMul    = -1;
    m_clearTempRect = true;


    m_pCurStd = new QCursor(Qt::ArrowCursor);//new QCursor(bmp_curstd_b,bmp_curstd_m,0,0);
    m_pCurMov = new QCursor(Qt::OpenHandCursor);//new QCursor(bmp_curmov_b,bmp_curmov_m,0,0);

    //m_pClipboard = QApplication::clipboard();
    //QObject::connect(m_pClipboard,SIGNAL(dataChanged()),this,SLOT(clipboardChanged()) );
    //m_bCanPaste = false;

    connect(&m_timerTempDraw,SIGNAL(timeout()),this,SLOT(onTimerDrawTemp()));
    //    connect(this,SIGNAL(contentsMoving (int,int)),this,SLOT(scrollBuffer(int,int)));
}

SXBSchView::~SXBSchView()
{
    delete m_pCurStd;
    delete m_pCurMov;
    delete m_pDoc;
}

//ズームイン・アウトに関連するメソッド
bool SXBSchView::CanZoomIn()
{
    return (m_nZoom < MAX_ZOOM);
    //    if(m_viewScale > 1)    return true;
    //    else    	    	return false;
}

bool SXBSchView::CanZoomOut()
{
    return (m_nZoom > 0);
    //    if(m_viewScale < 5)    return true;
    //    else    	    	return false;
}

//指定図面座標をビューの中心にする
void SXBSchView::JumpTo(QPoint& jumpto)
{
    //qDebug("JumpTo() %d,%d",jumpto.x(),jumpto.y());
    int sheetw = m_pDoc->SheetSize().w();
    int sheeth = m_pDoc->SheetSize().h();

    //ジャンプ先が図面の外なら図面内に修正する
    if(jumpto.x()>sheetw) jumpto.setX(sheetw);
    if(jumpto.y()>sheeth) jumpto.setY(sheeth);


    int viewportw = width();
    int viewporth = height();
    int newContentsX = jumpto.x()*m_viewScaleMul/m_viewScale-viewportw/2;
    int newContentsY = jumpto.y()*m_viewScaleMul/m_viewScale-viewporth/2;
    if(newContentsX < 0) newContentsX = 0;
    if(newContentsY < 0) newContentsY = 0;
    //setContentsPos (newContentsX,newContentsY);
    QPoint pos = SheetToContents(jumpto);
    //pos = contentsToViewport(pos);
    //pos = viewport()->mapToGlobal(pos);
    QCursor::setPos(pos);
}



void SXBSchView::ZoomIn(bool menucommand)
{
    //    qDebug("ZoomIn %d",menucommand);
    //printf("SXBSchView::ZoomIn()\n");
    if(m_bLButtonDown) return;    //左ボタン押下中はズーム禁止

    QPoint pos;
    if(!menucommand) {
        pos = QCursor::pos();
        //pos = viewport()->mapFromGlobal(pos);
        //pos = viewportToContents(pos);
        pos = ContentsToSheet(pos);
    }

    m_nZoom++;
    setZoomValue();    //  m_viewScale, m_viewScaleMul の設定

    //    switch(m_viewScale){
    //    case 1:
    //    	return;
    //    case 2:
    //    	m_viewScale = 1;
    //    	break;
    //    case 5:
    //    default:
    //    	m_viewScale = 2;
    //    	break;
    //    }
    int width    =m_pDoc->SheetSize().w()*m_viewScaleMul/m_viewScale;
    int height    =m_pDoc->SheetSize().h()*m_viewScaleMul/m_viewScale;
    setMinimumSize(width,height);
    if(!menucommand)JumpTo(pos);
    m_clearTempRect = true;
    update();
}

void SXBSchView::ZoomOut(bool menucommand)
{
    //    qDebug("ZoomOut %d",menucommand);
    //printf("SXBSchView::ZoomOut()\n");
    if(m_bLButtonDown) return;    //左ボタン押下中はズーム禁止

    QPoint pos;
    if(!menucommand) {
        pos = QCursor::pos();
        //pos = viewport()->mapFromGlobal(pos);
        //pos = viewportToContents(pos);
        pos = ContentsToSheet(pos);
    }


    m_nZoom--;
    setZoomValue();    //  m_viewScale, m_viewScaleMul の設定

    //    switch(m_viewScale){
    //    case 5:
    //    	return;
    //    case 1:
    //    	m_viewScale = 2;
    //    	break;
    //    case 2:
    //    default:
    //    	m_viewScale = 5;
    //    	break;
    //    }
    int width    =m_pDoc->SheetSize().w()*m_viewScaleMul/m_viewScale;
    int height    =m_pDoc->SheetSize().h()*m_viewScaleMul/m_viewScale;
    setMinimumSize(width,height);
    if(!menucommand)JumpTo(pos);
    m_clearTempRect = true;
    update();    //画面の更新を強制
}


//コピー
void SXBSchView::copy()
{
    m_pDoc->copy();
}

//カット
void SXBSchView::cut()
{
    if(m_pDoc->m_listTemp.size() > 0 ) {
        m_pDoc->cut();
        update();    //画面の更新を強制
    }
}

//ペースト
void SXBSchView::paste()
{
    if(!m_pDoc->canPaste()) return;
    if(m_toolmode != TOOL_SELECTOR) {
        if(!setToolMode(TOOL_SELECTOR)) return;
        emit toolChange();
    }

    //    QPainter painter(viewport());
    //    painter.translate ( -contentsX(),-contentsY() );


    if(m_pDoc->m_listTemp.size() > 0 ) {
        //    	drawTempDataFrame(&painter);
        //    	drawTempXBSchObj(&painter,DRAW_XOR,NULL);
        //    	drawTempXBSchObj(&painter,DRAW_ON,NULL);
        m_pDoc->resetSelect();
        updateViewBuffer(true);
        update();    //画面の更新を強制
    }

    QPoint ptOrigin;
    QPoint ptAtSheet;
    QPoint ptCur = QCursor::pos();
    ptCur = mapFromGlobal(ptCur);
    QSize sizeVp = QSize(width(),height());
    //printf("SXBSchView::paste() C:%d  %d   VP:%d  %d\n",ptCur.x(),ptCur.y(),sizeVp.width(),sizeVp.height());
    if(   (ptCur.x() < 0)
            ||(ptCur.y() < 0)
            ||(ptCur.x() > sizeVp.width() )
            ||(ptCur.y() > sizeVp.height() ) ) {
        ptOrigin = QPoint(10,10);    //カーソルがビューの領域外なら画面左上に配置
    } else {
        //ptAtSheet = viewportToContents(ptCur);     //カーソルの位置をスクロールビュー座標に変換
        //ptAtSheet = ContentsToSheet(ptAtSheet);    //カーソルの位置を図面座標に変換
        ptAtSheet = ptCur;

        if(   (ptAtSheet.x() < 10)
                ||(ptAtSheet.y() < 10)
                ||(ptAtSheet.x() > (m_pDoc->SheetSize().w()-10) )
                ||(ptAtSheet.y() > (m_pDoc->SheetSize().h()-10) ) ) {
            ptOrigin = QPoint(10,10);
        } else {
            ptOrigin = ptCur;
        }
    }
    //ptOrigin = viewportToContents(ptOrigin);     //配置位置をスクロールビュー座標に変換
    //ptOrigin = ContentsToSheet(ptOrigin);    	//配置位置を図面座標に変換

    ptOrigin = snapPoint(ptOrigin);

    //printf("SXBSchView::paste()2 :%d  %d \n",ptOrigin.x(),ptOrigin.y());
    if(m_pDoc->paste( SPoint(ptOrigin.x(),ptOrigin.y()) )) {
        //    	drawTempXBSchObj(&painter,DRAW_XOR,NULL);
        //    	m_pDoc->setupRcTempArea();
        //    	m_pDoc->setupRotateRect();
        //    	drawTempDataFrame(&painter);
        m_clearTempRect = true;    	//2005/05/14
        update();    //画面の更新を強制
        m_bCompleteDrawTemp = true;
    }
    //repaint(TRUE);
    //viewport()->update();    //画面の更新を強制
}

//削除
void SXBSchView::del()
{
    //    QPainter painter(viewport());
    //    painter.translate ( -contentsX(),-contentsY() );

    if(m_pDoc->m_listTemp.size() > 0 ) {
        //    	drawTempDataFrame(&painter);
        //    	drawTempXBSchObj(&painter,DRAW_XOR,NULL);
        m_pDoc->del();
    }
    //repaint(TRUE);
    update();    //画面の更新を強制
}

void SXBSchView:: moveToLayer(int nLayer)
{
    if(!canMoveToLayer(nLayer))return;
    m_pDoc->moveToLayer(nLayer);
    m_pDoc->resetSelect();
    update();    //画面の更新を強制
}

//UNDO
void SXBSchView::undo()
{
    m_pDoc->undo();
    updateViewBuffer(true);
    update();    //画面の更新を強制
}

//表示に関わるもの
void SXBSchView::RefreshView()
{
    updateViewBuffer(true);
    update();
}


//Attribute
void SXBSchView::attribute()
{
    if(!m_pDoc->canAttribute()) return;
    XBSchObjListIterator pObjIte =m_pDoc->m_listTemp.begin();
    switch((*pObjIte)->id()) {
    case SXBSchObj::ID_LABEL:
        execLabelAttributeDlg((SXBSchLabel*)(*pObjIte));
        break;
    case SXBSchObj::ID_TAG:
        execTagAttributeDlg((SXBSchTag*)(*pObjIte));
        break;
    case SXBSchObj::ID_COMPONENT:
        execComponentAttributeDlg((SXBSchComponent*)(*pObjIte));
        break;
    case SXBSchObj::ID_COMMENT:
        execCommentAttributeDlg((SXBSchComment*)(*pObjIte));
        break;
    }
}


void SXBSchView::setDefaultFont()
{
    g_defaultfont = g_cfg.getSettings()->value("Font",QFont("San-serif",14)).value < QFont>();
}

//コメントのアトリビュート変更ダイアログを起動する
void SXBSchView::execCommentAttributeDlg(SXBSchComment* pComment)
{
    editcommentdlg *dlg = new editcommentdlg(this);
    //editcomment *dlg = new editcomment(this,0,true);
    dlg->setText(QString::fromLocal8Bit(pComment->text()));
    if(strlen(pComment->fontName()) != 0) {
        QFont font(QString::fromLocal8Bit(pComment->fontName()));
        font.setPixelSize(pComment->fontSize());
        font.setBold(pComment->fontBold());
        font.setItalic(pComment->fontItalic());
        font.setStrikeOut(pComment->fontStrikeOut());
        font.setUnderline(pComment->fontUnderline());
        dlg->setFont(font);
    }

    if ( dlg->exec() ) {
        if(!dlg->text().isEmpty()) {
            pComment->setText(dlg->text().toLocal8Bit());
            pComment->setFontName(dlg->font().family().toLocal8Bit());
            pComment->setFontSize(dlg->font().pixelSize() < 0 ? dlg->font().pointSize()*qApp->desktop()->logicalDpiX()/72 : dlg->font().pixelSize()); //2003/06/14 Qt3対応
            pComment->setFontBold(dlg->font().bold());
            pComment->setFontItalic(dlg->font().italic());
            pComment->setFontUnderline(dlg->font().underline());
            pComment->setFontStrikeOut(dlg->font().strikeOut());
            pComment->setSelection(SXBSchObj::SELECT_ALL);
            m_clearTempRect = true;
            m_pDoc->setTempDirty();
            m_clearTempRect = true;    	//2005/05/14
        } else {
            m_pDoc->del();
        }
        update();
    }
    //#ifndef _WINDOWS
    //    viewport()->update();
    //#endif
    delete dlg;
}



//ラベルのアトリビュート変更ダイアログを起動する
void SXBSchView::execLabelAttributeDlg(SXBSchLabel* pObj)
{
    EditLabelDialog *dlg = new EditLabelDialog(this);
    dlg->setText(QString::fromLocal8Bit(pObj->text()));
    if ( dlg->exec() ) {
        if(!dlg->text().isEmpty()) {
            pObj->setText(dlg->text().toLatin1());
            m_pDoc->setupRcTempArea();
            m_pDoc->setupRotateRect();
            m_pDoc->setTempDirty();
            m_clearTempRect = true;    	//2005/05/14
        } else {
            m_pDoc->del();
        }
        update();
    }
    delete dlg;
}


//タグのアトリビュート変更ダイアログを起動する
void SXBSchView::execTagAttributeDlg(SXBSchTag* pObj)
{
    //EditTag *dlg = new EditTag(this,Qt::Sheet);
    EditTagDialog *dlg = new EditTagDialog(this);
    dlg->setText(QString::fromLocal8Bit(pObj->text()));
    dlg->setType(pObj->tagType());
    if ( dlg->exec() ) {
        // user clicked OK
        //qDebug((dlg->text()).local8Bit());
        //QPainter painter(viewport());
        //painter.translate ( -contentsX(),-contentsY() );
        //drawTempDataFrame(&painter);
        //drawTempXBSchObj(&painter,DRAW_OFF,NULL);
        pObj->setText(dlg->text().toLatin1());
        pObj->setTagType(dlg->type());
        m_pDoc->setupRcTempArea();
        m_pDoc->setupRotateRect();
        m_pDoc->setTempDirty();
        //drawTempXBSchObj(&painter,DRAW_XOR,NULL);
        //drawTempDataFrame(&painter);
        m_clearTempRect = true;    	//2005/05/14
        update();
    }
    delete dlg;
}

//部品のアトリビュート変更ダイアログを起動する
void SXBSchView::execComponentAttributeDlg(SXBSchComponent* pObj)
{
    //editcomponent *dlg = new editcomponent(this,0,true);
    EditComponentDialog *dlg = new EditComponentDialog(this);
    dlg->setCompIndex(pObj->compInfoIndex());
    dlg->setNote( QString::fromLocal8Bit(pObj->note()));
    dlg->setName( QString::fromLocal8Bit(pObj->name()));
    dlg->setOrgName( QString::fromLocal8Bit(pObj->orgname()));
    dlg->setReference( QString::fromLocal8Bit(pObj->refnum()));
    dlg->setBlock( pObj->block());
    dlg->setNameVertical(!(pObj->nameHorizontal()));
    dlg->setReferenceVertical(!(pObj->refnumHorizontal()));
    if ( dlg->exec() ) {
        //QPainter painter(viewport());
        //painter.translate ( -contentsX(),-contentsY() );
        //drawTempDataFrame(&painter);
        //drawTempXBSchObj(&painter,DRAW_OFF,NULL);
        pObj->setNote(dlg->note().toLocal8Bit());
        pObj->setName(dlg->name().toLocal8Bit());
        pObj->setRefnum(dlg->reference().toLatin1());
        pObj->setBlock(dlg->block());

        pObj->setNameHorizontal(!(dlg->nameVertical()));
        pObj->setRefnumHorizontal(!(dlg->referenceVertical()));

        //drawTempXBSchObj(&painter,DRAW_XOR,NULL);
        //drawTempDataFrame(&painter);
        m_pDoc->setTempDirty();
        m_clearTempRect = true;    	//2005/05/14
        update();
    }
    delete dlg;
}



//Rotate
void SXBSchView::rotate()
{
    if(m_pCursorObj) {
        //QPainter painter(viewport());
        //painter.translate ( -contentsX(),-contentsY() );
        //drawCursorObj(&painter);
        m_pCursorObj->rotate();
        //drawCursorObj(&painter);
        update();
        return;
    }

    if(!m_pDoc->canRotate()) return;
    //    QPainter painter(viewport());
    //    painter.translate ( -contentsX(),-contentsY() );

    //    drawTempDataFrame(&painter);
    //    drawTempXBSchObj(&painter,DRAW_XOR,NULL);
    m_pDoc->rotate();
    //    drawTempXBSchObj(&painter,DRAW_XOR,NULL);
    //    drawTempDataFrame(&painter);
    update();
}

//Mirror
void SXBSchView::mirror()
{
    if(m_pCursorObj) {
        //QPainter painter(viewport());
        //painter.translate ( -contentsX(),-contentsY() );
        //drawCursorObj(&painter);
        m_pCursorObj->rotate();
        //drawCursorObj(&painter);
        update();
        return;
    }


    if(!m_pDoc->canMirror()) return;
    //    QPainter painter(viewport());
    //    painter.translate ( -contentsX(),-contentsY() );

    //    drawTempDataFrame(&painter);
    //    drawTempXBSchObj(&painter,DRAW_XOR,NULL);
    m_pDoc->mirror();
    //    drawTempXBSchObj(&painter,DRAW_XOR,NULL);
    //    drawTempDataFrame(&painter);
    update();    //画面の更新を強制
}

//Invert()
void SXBSchView::invert()
{
}

//ツール設定に関連するメソッド
bool SXBSchView::setToolMode(BSCH_TOOL_MODE mode)
{
    if(m_bLButtonDown) return false;    //左ボタン押下中はツール変更を行わない

    //    QPainter painter(viewport());
    //    painter.translate ( -contentsX(),-contentsY() );

    if(m_pDoc->m_listTemp.size()>0) {
        m_pDoc->resetSelect();
        updateViewBuffer(true);
        update();

    }
    //カーソルに付きまとう図面オブジェクトの描画の後始末
    if(m_pCursorObj) {
        //drawCursorObj(&painter);
        delete m_pCursorObj;
        m_pCursorObj = NULL;
        update();
    }


    //    m_toolmode = mode;

    switch(mode) {
    case TOOL_SELECTOR:
        if(m_toolmode == TOOL_SELECTOR) {
            update();
        }
        break;
    case TOOL_JUNCTION:
        m_pCursorObj = (SXBSchObj*) new SXBSchJunc;
        m_pCursorObj->setP1(SPoint(-100,-100));
        break;
    case TOOL_ENTRY:
        m_pCursorObj = (SXBSchObj*) new SXBSchEntry;
        m_pCursorObj->setP1(SPoint(-100,-100));
        break;
    case TOOL_BENTRY:
        m_pCursorObj = (SXBSchObj*) new SXBSchBusEntry;
        m_pCursorObj->setP1(SPoint(-100,-100));
        break;
    default:
        break;
    }
    m_toolmode = mode;

    return true;
}


//グリッド表示に関連するメソッド
void SXBSchView::setDisplayGrid(bool display)
{
    m_bDisplayGrid = display;
    m_oldOpCount = -1;
    update();
}

//スナップの状態に関連するメソッド
void SXBSchView::setSnapGrid(bool snap)
{
    if(m_bLButtonDown) return;
    m_bSnapGrid = snap;
}

//シートサイズの設定ダイアログの起動
void SXBSchView::resizeSheet()
{
    setSheetSizeDlg dlg(this);
    dlg.m_width  = m_pDoc->SheetSize().w();
    dlg.m_height = m_pDoc->SheetSize().h();
    dlg.setWindowModality(Qt::WindowModal);
    if(dlg.exec()) {
        int w = dlg.m_width;
        int h = dlg.m_height;
        SetSheetSize(QSize(w,h));
        g_cfg.setInt("Sheet","Width",w);
        g_cfg.setInt("Sheet","Height",h);
    }
}

//シートサイズの設定
void SXBSchView::SetSheetSize(const QSize& s)
{
    //ドキュメントへの設定
    m_pDoc->SetSheetSize(SSize(s.width(),s.height()));
    //ビューの再設定
    int width    =m_pDoc->SheetSize().w()*m_viewScaleMul/m_viewScale;
    int height    =m_pDoc->SheetSize().h()*m_viewScaleMul/m_viewScale;
    setMinimumSize(width,height);
    setAutoScrollValue();
    m_oldOpCount = -1;
    update();    //画面の更新を強制
}

//シートサイズの取得
QSize SXBSchView::SheetSize()
{
    QSize size(m_pDoc->SheetSize().w(),m_pDoc->SheetSize().h());
    return size;
}


//ファイルの保存
int SXBSchView::save(const QString& fname)
{
    int nResult = m_pDoc->writeFile(fname.toLocal8Bit());
    updateViewBuffer(true);
    update();    //画面の更新を強制
    return nResult;
}

//ファイルからの読み込み
int SXBSchView::load(const QString& fname)
{
    //qDebug("SXBSchView::load() %s\n",(const char*)fname.local8Bit());
    int nResult = m_pDoc->readFile(fname.toLocal8Bit());
    if(!nResult) { //0で正常終了
        int width    =m_pDoc->SheetSize().w()*m_viewScaleMul/m_viewScale;
        int height    =m_pDoc->SheetSize().h()*m_viewScaleMul/m_viewScale;
        setMinimumSize(width,height);
        setAutoScrollValue();
    }
    m_oldOpCount = -1;
    update();    //画面の更新を強制
    return nResult;
}

//イメージファイルの出力
bool SXBSchView::exportImage(const QString& fname)
{
    m_pDoc->resetSelect();
    updateViewBuffer(true);

    int w = SheetSize().width();
    int h = SheetSize().height();

    QPixmap buffer(w,h);

    QPainter painter;
    painter.begin(&buffer);
    QRect rcClip = QRect(0,0,w,h);
    SRect srcClip =SRect(0,0,w,h);

    //    int currentScale = m_viewScale;
    //    int currentScaleMul = m_viewScaleMul;

    //    m_viewScale = 1;
    //    m_viewScaleMul = 1;
    SSize size = m_pDoc->SheetSize();

    //#ifdef _WINDOWS
    //    painter.setBackgroundColor(Qt::color0);
    //    painter.eraseRect(0,0,w,h);
    //    g_drawFrame(&painter,size,rcClip,Qt::color1,1,1);
    //    drawMainXBSchObj(&painter,DRAW_MONO,&srcClip,1,1);
    //#else
    painter.setBackground(Qt::white);
    painter.eraseRect(0,0,w,h);
    g_drawFrame(&painter,size,rcClip,Qt::black,1,1);
    //    drawMainXBSchObj(&painter,DRAW_ON,&srcClip);

    QSettings *settings = g_cfg.getSettings();
    settings->beginGroup("PrintOption");
    bool color = settings->value("Color",true).toBool();
    settings->endGroup();
    drawMainXBSchObj(&painter,(color ? DRAW_ON : DRAW_MONO),&srcClip,false,1,1);
    //#endif
    //    m_viewScale = currentScale;
    //    m_viewScaleMul = currentScaleMul;
    bool result = buffer.save(fname);


    //qDebug("SXBSchView::exportImage(const QString& fname)");

    update();    //画面の更新を強制
    return result;
}


//マウスが押下されたときに呼ばれる。
void SXBSchView::mousePressEvent(QMouseEvent* me)
{
    //    printf("SXBSchView::contentsMousePressEvent()\n");
    //    printf("  X=%d  Y=%d\n",me->x(),me->y());
    //    printf("  ContentsX=%d  ContentsY=%d\n",contentsX(),contentsY());
    //    printf("  VisibleWidth=%d  VisibleHeight=%d\n", visibleWidth(), visibleHeight());

    m_bInhibitPlaceObject = false;
    m_bReserveAttribute = false;

    QPoint ptAtSheet = ContentsToSheet(QPoint(me->x(),me->y()));

    QPoint ptSnap;
    if(m_bSnapGrid) ptSnap = snapPoint(ptAtSheet);
    else    	    ptSnap = ptAtSheet;


    //    QPainter painter(viewport());
    //    painter.translate ( -contentsX(),-contentsY() );


    //    QPainter *ppainter;

    SXBSchObj* pObj;

    if(me->button() & Qt::LeftButton) {
        m_ptLButtonDownAtSheet = ptAtSheet;
        m_bLButtonDown = true;

        QPoint qpSnapTempSnapPoint(m_pDoc->tempSnapPoint().x(),m_pDoc->tempSnapPoint().y());

        int onFlag;
        //一時データが存在する場合
        if(m_pDoc->m_listTemp.size() > 0 ) {
            m_bInhibitPlaceObject = true;
            onFlag = m_pDoc->isOnTempData(SPoint(m_ptLButtonDownAtSheet.x(),m_ptLButtonDownAtSheet.y()));
            if(onFlag) {
                if(m_bSnapGrid) {
                    QPoint ptSnapTempObject = snapPoint(qpSnapTempSnapPoint);
                    if(ptSnapTempObject != qpSnapTempSnapPoint) {
                        int snapx = ptSnapTempObject.x() - qpSnapTempSnapPoint.x();
                        int snapy = ptSnapTempObject.y() - qpSnapTempSnapPoint.y();
                        //ppainter = new QPainter(viewport());
                        //ppainter->translate ( -contentsX(),-contentsY() );
                        //drawTempDataFrame(ppainter);
                        //drawTempXBSchObj(ppainter,DRAW_XOR,NULL);
                        m_pDoc->moveTempData(snapx,snapy);
                        //drawTempXBSchObj(ppainter,DRAW_XOR,NULL);
                        //drawTempDataFrame(ppainter);
                        //delete ppainter;
                        update();
                    }
                }

                //移動オプションの設定
                if(m_pDoc->m_listTemp.size() == 1) {
                    if(onFlag & ON_NAME) {
                        m_moveOption = MOVE_OPTION_COMPONENT_NAME;
                        updateViewBuffer(true);
                        //qDebug("Move component name");
                    } else if(onFlag & ON_NUM) {
                        m_moveOption = MOVE_OPTION_COMPONENT_NUM;
                        updateViewBuffer(true);
                        //qDebug("Move component num");
                    } else {
                        m_moveOption = 0;
                    }
                } else {
                    m_moveOption = 0;
                }
                //m_ptLastMoveTempAtSheet = ptSnap;
                m_ptLastMoveTempAtSheet = ptAtSheet;
                return;
            } else {
                //ボタン押下ポイントが一時データから外れている場合
                //一時データの選択状態を解除する

                //ppainter = new QPainter(viewport());
                //ppainter->translate ( -contentsX(),-contentsY() );
                //drawTempDataFrame(ppainter);
                //drawTempXBSchObj(ppainter,DRAW_OFF,NULL); //OFFではXOR描画を消せないことがある
                //drawTempXBSchObj(ppainter,DRAW_OFF,NULL);
                //drawTempXBSchObj(ppainter,DRAW_ON,NULL);
                //delete ppainter;
                m_pDoc->resetSelect();
                updateViewBuffer(true);
                update();

                // 2002/04/21 選択解除のあとにそのまま新規配置作業を行わない
                if(    m_toolmode == TOOL_COMMENT
                        || m_toolmode == TOOL_LABEL
                        || m_toolmode == TOOL_TAG
                        || m_toolmode == TOOL_COMPONENT) return;
            }
        }

        switch(m_toolmode) {
        case TOOL_WIRE:
        case TOOL_BUS:
        case TOOL_DASH:
            m_ptLastMoveAtContents = QPoint(me->x(),me->y());
            m_ptLastMoveAtSheet = ptAtSheet;
            m_bDrawTempLine = true;
            m_nTempLineOrder = 0;
            break;
        case TOOL_SELECTOR:
        case TOOL_DRAG:
            m_ptLastMoveAtContents = QPoint(me->x(),me->y());
            m_ptLastMoveAtSheet = ptAtSheet;
            m_bDrawSelectFrame = true;
            break;
        case TOOL_JUNCTION:
        case TOOL_ENTRY:
        case TOOL_BENTRY:
            if(m_pCursorObj != NULL) {
                //ppainter = new QPainter(viewport());
                //ppainter->translate ( -contentsX(),-contentsY() );
                //drawCursorObj(ppainter);
                //    pObj = (SXBSchObj*) new SXBSchJunc;
                //    pObj->setP1(ptSnap);
                //    drawJunction(&painter,pObj,DRAW_ON);
                pObj = m_pCursorObj->duplicate();
                //drawXBSchObj(ppainter,pObj,DRAW_ON,NULL);
                m_pDoc->addSelect(pObj);
                m_pDoc->resetSelect();
                update();    //画面の更新を強制
                //drawCursorObj(ppainter);
                //delete ppainter;
            }
            break;
        default:
            break;
        }
    }
}

void SXBSchView::mouseDoubleClickEvent(QMouseEvent *me)
{
    QPoint ptAtSheet = ContentsToSheet(QPoint(me->x(),me->y()));

    QPoint ptSnap;
    if(m_bSnapGrid) ptSnap = snapPoint(ptAtSheet);
    else    	    ptSnap = ptAtSheet;
    //QPainter *ppainter;

    if(m_toolmode == TOOL_WIRE) {
        //ppainter = new QPainter(viewport());
        //ppainter->translate ( -contentsX(),-contentsY() );
        SXBSchObj* pObj = (SXBSchObj*) new SXBSchJunc;
        pObj->setP1(SPoint(ptSnap.x(),ptSnap.y()));
        //drawJunction(ppainter,pObj,DRAW_ON);
        m_pDoc->addSelect(pObj);
        m_pDoc->resetSelect();
        //delete ppainter;
        update();
    } else {
        m_bReserveAttribute = true;
        //releaseMouse(); //Pantherでこれがうまく動作していないらしい。
        //attribute();
    }
}


//コメントの配置
bool  SXBSchView::placeComment(const QPoint& point)
{
    editcommentdlg *dlg = new editcommentdlg(this);

    QFont font = g_cfg.getSettings()->value("Font",QFont("San-serif",14)).value < QFont>();
    dlg->setFont(font);

    if ( dlg->exec() ) {
        if(!dlg->text().isEmpty()) {
            SXBSchComment *pComment = new SXBSchComment;
            pComment->setP1(SPoint(point.x(),point.y()));
            pComment->setText(dlg->text().toLocal8Bit());
            pComment->setFontName(dlg->font().family().toLocal8Bit());
            pComment->setFontSize(dlg->font().pixelSize() < 0 ? dlg->font().pointSize()*qApp->desktop()->logicalDpiX()/72 : dlg->font().pixelSize()); //Qt3対応
            pComment->setFontBold(dlg->font().bold());
            pComment->setFontItalic(dlg->font().italic());
            pComment->setFontUnderline(dlg->font().underline());
            pComment->setFontStrikeOut(dlg->font().strikeOut());
            pComment->setSelection(SXBSchObj::SELECT_ALL);
            m_pDoc->addSelect(pComment);
            delete dlg;
            return true;
        }
        delete dlg;
    } else {
        delete dlg;
        if(setToolMode(TOOL_SELECTOR)) {
            emit toolChange();
        }
    }
    return false;
}



//ラベルの配置
bool  SXBSchView::placeLabel(const QPoint& point)
{
    EditLabelDialog *dlg = new EditLabelDialog(this);
    if ( dlg->exec() ) {
        // user clicked OK
        //qDebug((dlg->text()).local8Bit());
        if(!dlg->text().isEmpty()) {
            SXBSchLabel *pLabel = new SXBSchLabel;
            pLabel->setP1(SPoint(point.x(),point.y()));
            pLabel->setText(dlg->text().toLatin1());
            pLabel->setSelection(SXBSchObj::SELECT_ALL);
            m_pDoc->addSelect(pLabel);
            delete dlg;
            return true;
        }
        delete dlg;
    } else {
        delete dlg;
        if(setToolMode(TOOL_SELECTOR)) {
            emit toolChange();
        }
    }
    return false;
}


//タグの配置
bool SXBSchView::placeTag(const QPoint& point)
{
    EditTagDialog *dlg = new EditTagDialog(this);
    if ( dlg->exec() ) {
        // user clicked OK
        //if(!dlg->text().isEmpty()){
        SXBSchTag *pTag = new SXBSchTag;
        pTag->setP1(SPoint(point.x(),point.y()));
        const char* pc = dlg->text().toLatin1();
        pTag->setText(pc);
        pTag->setTagType(dlg->type());
        pTag->setSelection(SXBSchObj::SELECT_ALL);
        m_pDoc->addSelect(pTag);
        //}
        delete dlg;
        return true;
    } else {
        // user clicked Cancel
        //qDebug("SXBSchView::placeLabel  Cancel\n");
        delete dlg;
        if(setToolMode(TOOL_SELECTOR)) {
            emit toolChange();
        }
    }
    return false;

}

//部品の配置
bool SXBSchView::placeComponent(const QPoint& point)
{
//    SelComp *dlg = new SelComp(this->parentWidget(),Qt::Sheet);
    g_FreeLibrary();
    g_ReadLibrary();
    SelectComponentDialog *dlg = new SelectComponentDialog(this->parentWidget());
    if ( dlg->exec() ) {
        const SCompIndex* pCompInfoIndex = dlg->selectedCompIndex();
        SXBSchComponent* pComp = new  SXBSchComponent;
        pComp->setP1(SPoint(point.x(),point.y()));
        pComp->setCompInfoIndex(pCompInfoIndex);
        pComp->setName(pCompInfoIndex->name());
        pComp->setRefnum(pCompInfoIndex->number());
        pComp->setSelection(SXBSchObj::SELECT_ALL);
        m_pDoc->addSelect(pComp);
        delete dlg;
        return true;
    } else {
        delete dlg;
        if(setToolMode(TOOL_SELECTOR)) {
            emit toolChange();
        }
    }
    return false;
}



void SXBSchView::paintEvent ( QPaintEvent * /*event*/ )
{
    QPainter painter(this);
    drawContents(&painter,0,0,width(),height());
}


//マウスのボタンが解放されたときに呼ばれる。
void SXBSchView::mouseReleaseEvent(QMouseEvent *me)
{
    bool shiftkeyPressed = (me->modifiers() &  Qt::ShiftModifier);

    //    printf("SXBSchView::contentsMouseReleaseEvent()\n");
    //    printf("  X=%d  Y=%d\n",me->x(),me->y());
    if(m_bReserveAttribute) {
        m_bReserveAttribute = false;
        attribute();
        return;
    }

    QPoint ptAtSheet = ContentsToSheet(QPoint(me->x(),me->y()));

    QPoint ptSnap;
    if(m_bSnapGrid) ptSnap = snapPoint(ptAtSheet);
    else    	    ptSnap = ptAtSheet;


    //20050724    QPainter painter(viewport());
    //20050724    painter.translate ( -contentsX(),-contentsY() );

    //    QPoint ptAtSheet = ContentsToSheet(QPoint(me->x(),me->y()));

    if(me->button() & Qt::LeftButton) {
        m_bLButtonDown = false;
        int oldMoveOption = m_moveOption;
        m_moveOption   =0;
        if(oldMoveOption) {
            updateViewBuffer(true);
            update();    //画面の更新を強制
        }
        if(m_bDrawSelectFrame) {
            //drawSelectFrame(&painter);//,QColor("black"));
            m_bDrawSelectFrame = false;
            int nSelCount = 0;
            QRect rc(m_ptLButtonDownAtSheet,m_ptLastMoveAtSheet);
            rc = rc.normalized();
            if(m_toolmode == TOOL_SELECTOR) {
                int moveWidth  = abs(m_ptLButtonDownAtSheet.x() - m_ptLastMoveAtSheet.x());
                int moveHeight = abs(m_ptLButtonDownAtSheet.y() - m_ptLastMoveAtSheet.y());
                if(moveWidth < 3 && moveHeight < 3) {
                    nSelCount = m_pDoc->select(SPoint(m_ptLastMoveAtSheet.x(),m_ptLastMoveAtSheet.y()));
                } else {
                    nSelCount = m_pDoc->select(SRect(rc.left(),rc.top(),rc.width(),rc.height()));
                }
            } else if(m_toolmode == TOOL_DRAG) {
                nSelCount = m_pDoc->selectForDrag(SRect(rc.left(),rc.top(),rc.width(),rc.height()),shiftkeyPressed);
            }
            if(nSelCount > 0) {
                //    	    	drawTempXBSchObj(&painter,DRAW_OFF,NULL);
                //    	    	drawTempXBSchObj(&painter,DRAW_XOR,NULL);
                //    	    	drawTempDataFrame(&painter);
                m_bCompleteDrawTemp = true;
                m_oldOpCount =-1;
                //viewport()->update();    //画面の更新を強制
            }
            update();    //画面の更新を強制
        } else if(m_bDrawTempLine) {
            //drawTempLine(&painter);
            m_bDrawTempLine = false;
            SXBSchObj* pObj1 = NULL;
            SXBSchObj* pObj2 = NULL;

            switch(m_toolmode) {
            case TOOL_WIRE:
                pObj1 = new SXBSchWire;
                pObj2 = new SXBSchWire;
                break;
            case TOOL_BUS:
                pObj1 = new SXBSchBus;
                pObj2 = new SXBSchBus;
                break;
            case TOOL_DASH:
                pObj1 = new SXBSchDash;
                pObj2 = new SXBSchDash;
                break;
            default:
                break;
            }

            QPoint ptStart = m_ptLButtonDownAtSheet;
            if(m_bSnapGrid) ptStart = snapPoint(ptStart);

            QPoint ptEnd = m_ptLastMoveAtSheet;
            if(m_bSnapGrid) ptEnd = snapPoint(ptEnd);

            QPoint passPoint;
            if(m_nTempLineOrder >= 0) {    //水平線を先に描画する
                passPoint.setX(ptEnd.x());
                passPoint.setY(ptStart.y());
            } else {    	    	    	//垂直線を先に描画する
                passPoint.setX(ptStart.x());
                passPoint.setY(ptEnd.y());
            }
            if(ptStart == passPoint) {
                delete pObj1;
                pObj1 = NULL;
            } else {
                pObj1->setP1(SPoint(ptStart.x(),ptStart.y()));
                pObj1->setP2(SPoint(passPoint.x(),passPoint.y()));
                //    	    	drawXBSchObj(&painter,pObj1,DRAW_ON,NULL);
                m_pDoc->addSelect(pObj1);
                m_pDoc->resetSelect();

            }
            if(passPoint == ptEnd) {
                delete pObj2;
                pObj2 = NULL;
            } else {
                pObj2->setP1(SPoint(passPoint.x(),passPoint.y()));
                pObj2->setP2(SPoint(ptEnd.x(),ptEnd.y()));
                //    	    	drawXBSchObj(&painter,pObj2,DRAW_ON,NULL);
                m_pDoc->addSelect(pObj2);
                m_pDoc->resetSelect();

            }
            update();    //画面の更新を強制

        } else if(!m_bInhibitPlaceObject) {
            //    	    QPainter *ppainter;
            switch(m_toolmode) {
            case TOOL_COMMENT:
                m_bLButtonDown = false;
                if(placeComment(ptSnap)) {
                    //ppainter = new QPainter(viewport());
                    //ppainter->translate ( -contentsX(),-contentsY() );
                    //drawTempXBSchObj(ppainter,DRAW_XOR,NULL);
                    //m_pDoc->setupRcTempArea();
                    //drawTempDataFrame(ppainter);
                    //m_bCompleteDrawTemp = true;
                    //delete ppainter;
                    m_clearTempRect = true;
                    update();
                }
                break;

            case TOOL_LABEL:
                m_bLButtonDown = false;
                if(placeLabel(ptSnap)) {
                    //ppainter = new QPainter(viewport());
                    //ppainter->translate ( -contentsX(),-contentsY() );
                    //drawTempXBSchObj(ppainter,DRAW_XOR,NULL);
                    //drawTempDataFrame(ppainter);
                    //m_bCompleteDrawTemp = true;
                    //delete ppainter;
                    m_clearTempRect = true;
                    update();

                }
                break;

            case TOOL_TAG:
                m_bLButtonDown = false;
                if(placeTag(ptSnap)) {
                    //ppainter = new QPainter(viewport());
                    //ppainter->translate ( -contentsX(),-contentsY() );
                    //drawTempXBSchObj(ppainter,DRAW_XOR,NULL);
                    //drawTempDataFrame(ppainter);
                    //m_bCompleteDrawTemp = true;
                    //delete ppainter;
                    m_clearTempRect = true;
                    update();

                }
                break;

            case TOOL_COMPONENT:
                m_bLButtonDown = false;
                if(placeComponent(ptSnap)) {
                    //ppainter = new QPainter(viewport());
                    //ppainter->translate ( -contentsX(),-contentsY() );
                    //drawTempXBSchObj(ppainter,DRAW_XOR,NULL);
                    //drawTempDataFrame(ppainter);
                    //m_bCompleteDrawTemp = true;
                    //delete ppainter;
                    //    	    	    qDebug("placeComponent\n");
                    m_clearTempRect = true;
                    update();

                }
                break;
            default:
                break;
            }
        }
    }
}

//マウスが動いたときに呼ばれる。
void SXBSchView::mouseMoveEvent(QMouseEvent *me)
{
    //20050724    QPainter painter(viewport());
    //20050724    painter.translate ( -contentsX(),-contentsY() );

    QPoint ptMoveAtContents(me->x(),me->y());
    QPoint ptMoveAtSheet = ContentsToSheet(ptMoveAtContents);
    //    QPoint ptStart = SheetToContents(m_ptLButtonDownAtSheet);

    int dx,dy;
    if(ptMoveAtSheet.x()<0) dx = 0;
    else dx = ptMoveAtSheet.x();
    if(ptMoveAtSheet.y()<0) dy = 0;
    else dy = ptMoveAtSheet.y();
    emit cursorMove(dx,dy);


    //    if(m_pDoc->isOnTempData(ptMoveAtSheet)) viewport()->setCursor(sizeAllCursor);
    //    else    	    	    	    	    viewport()->setCursor(arrowCursor);


    //スナップを考慮した現在のカーソル位置の生成
    QPoint ptSnap;
    if(m_bSnapGrid)    ptSnap = snapPoint(ptMoveAtSheet);
    else    	    ptSnap = ptMoveAtSheet;
    QPoint ptSnapLast;
    if(m_bSnapGrid)    ptSnapLast = snapPoint(m_ptLastMoveTempAtSheet);
    else    	    ptSnapLast = m_ptLastMoveTempAtSheet;


    //    qDebug("MouseMove  %d,%d   %d,%d   %d,%d\n",ptMoveAtSheet.x(),ptMoveAtSheet.y(),ptSnapLast.x(),ptSnapLast.y(),ptSnap.x(),ptSnap.y());



    //左ボタンが押されていて、一時データが存在する場合
    if((me->buttons() & Qt::LeftButton) && (m_pDoc->m_listTemp.size() > 0 ) ) {
        if(m_moveOption == MOVE_OPTION_COMPONENT_NAME || m_moveOption == MOVE_OPTION_COMPONENT_NUM) {
            if(m_ptLastMoveTempAtSheet != ptMoveAtSheet) {
                int offsetx = ptMoveAtSheet.x() - m_ptLastMoveTempAtSheet.x();
                int offsety = ptMoveAtSheet.y() - m_ptLastMoveTempAtSheet.y();
                XBSchObjListIterator pObjIte = m_pDoc->m_listTemp.begin();
                SXBSchComponent* pComp = (SXBSchComponent*)(*pObjIte);
                if(m_moveOption == MOVE_OPTION_COMPONENT_NAME) {
                    //g_drawComponentNameXOR(&painter,pComp,m_viewScale);
                    pComp->moveNamePos(offsetx,offsety);
                    m_pDoc->setTempDirty();
                    //g_drawComponentNameXOR(&painter,pComp,m_viewScale);
                    update();    //画面の更新を強制
                } else if(m_moveOption == MOVE_OPTION_COMPONENT_NUM) {
                    //g_drawComponentNumXOR(&painter,pComp,m_viewScale);
                    pComp->moveRefnumPos(offsetx,offsety);
                    m_pDoc->setTempDirty();
                    //g_drawComponentNumXOR(&painter,pComp,m_viewScale);
                    update();    //画面の更新を強制
                }
            }
        } else if(ptSnapLast != ptSnap) {
            //    	    drawTempDataFrame(&painter);
            //    	    if(m_bComp    	    leteDrawTemp){
            //    	    	drawTempXBSchObj(&painter,DRAW_XOR,NULL);
            //    	    }else{
            //    	    	m_timerTempDraw.stop();
            //    	    }
            int offsetx = ptSnap.x() - ptSnapLast.x();
            int offsety = ptSnap.y() - ptSnapLast.y();
            m_pDoc->moveTempData(offsetx,offsety);
            //    	    m_timerTempDraw.start(200,true);
            //    	    m_bCompleteDrawTemp = false;
            //    	    drawTempDataFrame(&painter);
            m_bCompleteDrawTemp = true;
            update();    //画面の更新を強制

        }
        m_ptLastMoveTempAtSheet = ptMoveAtSheet;
        //移動中のカーソルの形状の設定
        setCursor(*m_pCurMov);

    } else {
        //移動中以外の場合のカーソルの形状の設定
        if(m_pDoc->isOnTempData(SPoint(ptMoveAtSheet.x(),ptMoveAtSheet.y()))) {
            setCursor(*m_pCurMov);
        } else {
            setCursor(*m_pCurStd);
        }
    }


    if(m_pCursorObj) {
        //drawCursorObj(&painter);
        m_pCursorObj->setP1(SPoint(ptSnap.x(),ptSnap.y()));
        //drawCursorObj(&painter);
        if(ptSnapLast != ptSnap) {
            update();
        }
        m_ptLastMoveTempAtSheet = ptMoveAtSheet;
    }


    //Windows において motifスタイルのとき、メニュー表示時に誤ったマウスボタン押下メッセージが
    //発生することの対策パッチ 2001/08/17
    if(!(me->buttons() & Qt::LeftButton)) {
        m_bLButtonDown = false;
        m_bDrawTempLine = false;
        m_bDrawSelectFrame = false;
        //return;
    }

    if(m_bDrawSelectFrame) {
        update();
        //drawSelectFrame(&painter);//,QColor("black"));
    } else if(m_bDrawTempLine) {
        //    	drawTempLine(&painter);
    }

    //「最後に動いたときの座標」の更新
    m_ptLastMoveAtContents = ptMoveAtContents;
    m_ptLastMoveAtSheet = ptMoveAtSheet;


    if(m_bDrawSelectFrame) {    	    //選択ツールで選択中
        //20050724    	drawSelectFrame(&painter);
    } else if(m_bDrawTempLine) {    	//布線ツールで仮線を描画中
        QPoint ptMov = m_ptLButtonDownAtSheet - m_ptLastMoveAtSheet;
        if(m_nTempLineOrder == 0) {    //縦が先か横が先かが未決定
            if(abs(ptMov.x()) > 8 || abs(ptMov.y()) > 8) {
                if(abs(ptMov.x()) > abs(ptMov.y())) {
                    m_nTempLineOrder = 1;
                } else {
                    m_nTempLineOrder = -1;
                }
            }
        } else {
            if(abs(ptMov.x()) <= 8 && abs(ptMov.y()) <= 8) {
                m_nTempLineOrder = 0;
            }
        }
        if(ptSnapLast != ptSnap) {
            //    	    qDebug("UpdateTempLine %d,%d   %d,%d\n",ptSnapLast.x(),ptSnapLast.y(),ptSnap.x(),ptSnap.y());
            update();
        }
        //    	drawTempLine(&painter);
        m_ptLastMoveTempAtSheet = ptMoveAtSheet;
    }

#if 0
    //ボタン押下中に表示エリアの周縁部でマウスが動いた場合にスクロールタイマーを生成する
    if(    	m_bLButtonDown //(me->state()&(LeftButton | RightButton) )
            &&    (    	(10 > ptMoveAtContents.x())
                        ||     (10 > ptMoveAtContents.y())
                        ||    (width()-10 < ptMoveAtContents.x())
                        ||    (height()-10 < ptMoveAtContents.y())
               )
      ) {
        if(!m_bTimer) {
            //printf("Set ScrollTimer\n");
            //QTimer::singleShot( 100, this, SLOT(onScrollTimer()) );
            //    	    QTimer *timer = new QTimer(this);
            //    	    connect(timer,SIGNAL(timeout()),this,SLOT(onScrollTimer()) );
            //    	    timer->start(100,true);
            m_bTimer = true;
        }
    }
#endif
}

//座標変換に関わるもの
//Contentsの座標からsheet座標への変換
QPoint SXBSchView::ContentsToSheet(const QPoint& point)
{
    QPoint p(point.x()*m_viewScale/m_viewScaleMul,point.y()*m_viewScale/m_viewScaleMul);
    return p;
}

//Contentsの座標からsheet座標への変換
QRect SXBSchView::ContentsToSheet(const QRect& rc)
{
    int x1,y1,x2,y2;
    rc.getCoords(&x1,&y1,&x2,&y2);
    x1 = x1*m_viewScale/m_viewScaleMul;
    y1 = y1*m_viewScale/m_viewScaleMul;
    x2 = x2*m_viewScale/m_viewScaleMul;
    y2 = y2*m_viewScale/m_viewScaleMul;
    QRect r;
    r.setCoords(x1,y1,x2,y2);
    //QRect r(    rc.left()    *m_viewScale,
    //    	    rc.top()    *m_viewScale,
    //    	    rc.width()    *m_viewScale,
    //    	    rc.height()    *m_viewScale);
    return r;
}

//sheetの座標からContents座標への変換
QPoint SXBSchView::SheetToContents(const QPoint& point)
{
    QPoint p(point.x()*m_viewScaleMul/m_viewScale,point.y()*m_viewScaleMul/m_viewScale);
    return p;
}

//sheetの座標からContents座標への変換
QRect SXBSchView::SheetToContents(const QRect& rc)
{
    int x1,y1,x2,y2;
    rc.getCoords(&x1,&y1,&x2,&y2);
    x1 = x1*m_viewScaleMul/m_viewScale;
    y1 = y1*m_viewScaleMul/m_viewScale;
    x2 = x2*m_viewScaleMul/m_viewScale;
    y2 = y2*m_viewScaleMul/m_viewScale;
    QRect r;
    r.setCoords(x1,y1,x2,y2);
    //    QRect r(    rc.left()    /m_viewScale,
    //    	    	rc.top()    /m_viewScale,
    //    	    	rc.width()    /m_viewScale,
    //    	    	rc.height()    /m_viewScale);
    return r;
}



//最寄りのグリッドの位置を返す
QPoint SXBSchView::snapPoint(const QPoint& point)
{
    QPoint p( ((point.x()+4)/10)*10,((point.y()+4)/10)*10);
    return p;
}

//自動スクロールのサイズの量の設定
void SXBSchView::setAutoScrollValue()
{
    m_nAutoScrollX = 50;
    m_nAutoScrollY = 50;
}



//SLOT。 自身の SIGNAL contentsMoving(int,int) にconnectされていて
//スクロールが始まる直前に呼ばれる。
//void SXBSchView::whenScroll(int x,int y)
//{
//    //printf("SXBSchView::whenScroll() %d,%d\n",x,y);
//}

//SLOT。スクロールタイマーのタイムアウト処理
//スクロールタイマーが起動されるときは左ボタンが押下されていることを前提としている。
void SXBSchView::onScrollTimer()
{
    m_bTimer = false;
    //printf("SXBSchView::onScrollTimer()\n");
    //自動スクロール必要性のチェック
    if(!m_bLButtonDown) return;
#if 0
    int x = 0;
    int y = 0;
    QSize size(m_pDoc->SheetSize().w(),m_pDoc->SheetSize().h());
    //左チェック
    if(contentsX()+10 > m_ptLastMoveAtContents.x()) {
        if(contentsX() > 0) x = -m_nAutoScrollX;
    } else
        //右チェック
        if(contentsX()+visibleWidth()-10 < m_ptLastMoveAtContents.x()) {
            if(contentsX()+visibleWidth() < size.width()*m_viewScaleMul/m_viewScale)  x = m_nAutoScrollX;
        }
    //上チェック
    if(contentsY()+10 > m_ptLastMoveAtContents.y()) {
        if(contentsY() > 0) y = -m_nAutoScrollY;
    } else
        //下チェック
        if(contentsY()+visibleHeight()-10 < m_ptLastMoveAtContents.y()) {
            if(contentsY()+visibleHeight() < size.height()*m_viewScaleMul/m_viewScale)  y = m_nAutoScrollY;
        }

    if(x || y) {
        scrollBy(x,y);
        QPoint ptCurrent = QCursor::pos();
        ptCurrent = viewport()->mapFromGlobal(ptCurrent);
        QMouseEvent* pme = new QMouseEvent(QEvent::MouseMove,ptCurrent,0,Qt::LeftButton);
        QApplication::postEvent(viewport(),pme);
    }
#endif
}

//一時データをマウスで移動しているときにタイマー駆動で再描画を行う
void SXBSchView::onTimerDrawTemp()
{
    return;

    //    if(m_bCompleteDrawTemp) return;
    //    QPainter painter(viewport());
    //    painter.translate ( -contentsX(),-contentsY() );
    //    drawTempXBSchObj(&painter,DRAW_XOR,NULL);
    //    m_bCompleteDrawTemp = true;
}

void SXBSchView::resizeEvent ( QResizeEvent *e )
{
    QSize size = e->size();
    resizeBuffer(size.width(),size.height());
}

void SXBSchView::resizeBuffer(int w,int h)
{
    //qDebug() << __func__ << w << h;
    m_viewBuffer = QPixmap(w,h);
    //qDebug() << m_viewBuffer.size();
    m_viewBuffer.fill(Qt::white);
    drawBuffer(0,0,0,0,w,h);
    //    qDebug("SXBSchView::resizeBuffer  %d,%d\n",w,h);
}

//m_nZoomをもとにして、setZoomValue()で、m_viewScaleとm_viewScaleMulを設定する。
void  SXBSchView::setZoomValue()
{
    if(m_nZoom < 0) {
        m_nZoom = 0;
    } else if (m_nZoom > MAX_ZOOM) {
        m_nZoom = MAX_ZOOM;
    }
    static int divTable[]= {5,2,1,2,1};
    static int mulTable[]= {1,1,1,3,5};


    m_viewScale = divTable[m_nZoom];    	    //縮小率
    m_viewScaleMul = mulTable[m_nZoom];    	//拡大率
}

unsigned SXBSchView::visibleLayer()
{
    return m_pDoc->visibleLayer();
}
void SXBSchView::setVisibleLayer(unsigned n)
{
    m_pDoc->setVisibleLayer(n);
}

int SXBSchView::activeLayer()
{
    return m_pDoc->editLayer();
}

void SXBSchView::setActiveLayer(int n)
{
    m_pDoc->setEditLayer(n);
}

bool SXBSchView::darkenNonActiveLayer()
{
    return m_fEditHighLight;
}

void SXBSchView::setDarkenNonActiveLayer(bool f)
{
    m_fEditHighLight = f;
}

bool SXBSchView::canMoveToLayer(int nLayer)
{
    nLayer &= 0x07;
    unsigned  layerTable[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

    return (   canCopy()
               && (m_toolmode == TOOL_SELECTOR)
               && (nLayer != m_pDoc->editLayer())
               && (layerTable[nLayer] & m_pDoc->visibleLayer()) );
}


//void SXBSchView::clipboardChanged()
//{
//    //qDebug("SXBSchView::clipboardChanged()\n");
//}

//void SXBSchView::setContentsPos ( int x, int y )
//{
//    qDebug("SXBSchView::setContentsPos  %d,%d\n",x,y);
//    QScrollView::setContentsPos ( x, y );
//}

//void SXBSchView::scrollBuffer(int x,int y){
//    qDebug("SXBSchView::scrollBuffer  %d,%d\n",x,y);
//
//
//    //drawBuffer(x,y,x,y,visibleWidth(),visibleHeight());
//
//    //m_oldContentsX = x;
//    //m_oldContentsY = y;
//}

