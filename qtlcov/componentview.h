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

#ifndef COMPONENTVIEW_H
#define COMPONENTVIEW_H

#include <qwidget.h>
#include <qapplication.h>

//#include <q3scrollview.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>

#include <string>
#include <list>
#include <qpainter.h>
#include <qpixmap.h>
#include <qcolor.h>


using namespace std;


#include "coord.h"
#include "complib.h"
#include "compeditdoc.h"

#define COL_EDITPTN (QColor(128,255,255))
#define COL_XOR (QColor(255,0,0))
#define COL_PART (QColor(0,0,255))

class QCursor;


class SComponentView: public QWidget
{
    Q_OBJECT

    enum {
        PAGE_MARGIN = 50,
        MIN_ZOOMNUM = 0,
        MAX_ZOOMNUM = 3
    };

    enum {
        DRAGOP_NOT    = 0,
        DRAGOP_PTN  = 1,
        DRAGOP_PIN    = 2
    };

public:
    enum {
        TOOL_SELECT = 1,
        TOOL_PIN    = 2,
        TOOL_LINE    = 3,
        TOOL_CIRCLE    = 4,
        TOOL_POLYGON = 5,
        TOOL_ARC    = 6,
        TOOL_TEXT    = 7
    };


public:
    SComponentView(QWidget* parent);    //コンストラクタ
    ~SComponentView();    //デストラクタ

    QSize setContentsSize();    //スクロールビューのコンテンツのサイズを決める

public:
    SCompEditDoc m_doc;
    SCompIndex* m_pCompIndex;

protected:
    int m_zoomNum;
    int m_magMul;            //表示倍率の分子
    int m_magDiv;            //表示倍率の分母
    int m_block;

    QSize m_pageSize;    //現在の表示倍率でのページサイズ
    QSize m_clientSize;    //クライアントエリアのサイズ
    QPoint m_windowOrg; //スクロールの左上座標
    int m_margin;

    int m_tool;            //現在のツール
    int m_placeState;        //配置操作のステート番号
    SPoint m_arrayTempPoint[64];

    int m_dragOpStatus;        //オブジェクトのドラッグ中

    bool m_startSelect;

    QPoint m_pointLButtonDown;    //左ボタンが押下されたときのマウス座標
    QPoint m_pointCurrentPos;        //現在のマウス座標
    QPoint m_pointLButtonUp;        //左ボタンが解放されたときのマウス座標
    QSize m_prevOffset;        //ドラッグ中にすでに移動を確定した量
    int m_pinPosLtrbStart;        //ピンのドラッグをはじめる前の位置 (LTRB)
    int m_pinPosOffsetStart;    //ピンのドラッグをはじめる前の位置 (OFFSET)


    QPixmap m_viewBuffer;        //オフスクリーンバッファ

    QSize    m_offScrnBuffSize;    //オフスクリーンバッファのサイズ
    QPoint     m_offScrnBuffOrg;    //オフスクリーンバッファの左上座標

    SPin* m_pCursorPin;            //カーソルに付き従うオブジェクト

    bool m_editNodeMode;        //ノード編集モードのとき true

    int m_movingGrip;                //グリップ移動属性

    QCursor* m_pCurStd;
    QCursor* m_pCurMov;




    // 操作
public:
    int tool() {
        return m_tool;   // 選択されているツールを返す
    }
    void setTool(int tool);            // ツールを選択する

    bool onKeyEscape();            // ESCAPEキーハンドラ

    bool canDelete();                // 何か DELETE できる状態なら true を返す
    bool doDelete();                // DELETEの実際

    bool canCreateBitPtn();        // ビットパターンの生成が可能なとき true
    bool doCreateBitPtn();            // ビットパターン生成の実際

    bool canCopy();                // COPYできる状態なら true を返す
    bool doCopy();                // COPYの実際

    bool canUndo();                // UNDOできる状態なら true を返す
    bool doUndo();                // UNDOの実際

    bool canMirrorRotate();        // ミラーまたは回転が可能なとき true を返す
    bool doMirror();                // ミラーの実際
    bool doRotate();                // 回転の実際

    void  setEditNodeMode(bool editnode); //ノード編集モード切替え
    bool  editNodeMode() {
        return m_editNodeMode;   //ノード編集モードかどうかを返す
    }




    bool canPaste();                // PASTEが可能なとき true を返す
    bool doPaste();                // PASTEの実際
    bool doPastePin(SReadCE3& rce3);    // doPaste()から呼ばれるピンのペースト処理
    bool doPastePtn(SReadCE3& rce3);    // doPaste()から呼ばれるパターンのペースト処理



    bool canProperty();            // PROPERTYコマンドが実行可能なとき true を返す
    bool editProperty();            // PROPERTYコマンドの実際
    bool editPinProperty(SPin* pPin);         // editProperty()から呼ばれるピンのPROPERTYコマンド
    bool editPtnProperty(SPtnObj* pPtn);    // editProperty()から呼ばれるパターンのPROPERTYコマンド


    bool save();    //m_docに展開した部品情報を m_pCompIndexに戻す;

    bool releaseTemp();    //tempデータに保持したデータを、一般展開データに戻す

    void setDirty() {
        m_doc.m_tempDirtyFlag = true;   //DIRTYフラグをONにする。
    }

    void RefreshView();

    //// オーバーライド
    //    protected:
    //    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


protected:
    void drawToBuff(bool force);    //バッファへの書き込み


    void drawPin(QPainter* p,SPin* pPin,int block,unsigned dwMode);
    void drawPins(QPainter* p,SCompEditElmList& list,unsigned dwMode);
    void drawPtns(QPainter* p,SCompEditElmList& list,unsigned dwMode);
    void drawPtn(QPainter* p,SPtnObj* pPtn,int cx,int cy,const QColor& color ,int magDiv,int magMul);
    void drawCompFrame(QPainter* p);
    void drawGrid(QPainter* p);
    void drawGrip(QPainter* p);
    void drawTempDataFrame(QPainter*p, const QRect& rc);

    int doSelect(SPoint sptEdit,QPoint lpt);

    void setZoomValue(int n);
    //座標変換 論理座標と部品編集座標の相互変換
    void LPtoCompEditP(QPoint& point);
    void LPtoCompEditP(QRect& rc);
    void CompEditPtoLP(QPoint& point);
    void CompEditPtoLP(QRect& rc);

    //ピンの配置。既存ピンがなければm_pCursorPinをTempDataに配置する。
    bool placePin();

    void drawPlaceLine(QPainter* p);
    void drawPlaceCircle(QPainter* p);
    void drawPlaceArc(QPainter* p);

    bool editNodeActive();

    int isOnGrip(const QPoint& logicalPoint);


    // 生成された、メッセージ割り当て関数
protected:
public:
    void setCompIndex(SCompIndex* pCompIndex);
    void saveCompInfo();
    QSize setPageSize();
    void OnViewZoomin();
    void OnViewZoomout();
    //    afx_msg void OnUpdateViewZoomin(CCmdUI *pCmdUI);
    //    afx_msg void OnUpdateViewZoomout(CCmdUI *pCmdUI);
    //    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    //    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    //    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    //    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);


protected:
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *me);
    virtual void mouseDoubleClickEvent ( QMouseEvent * );
    virtual void resizeEvent(QResizeEvent * );
    virtual void resizeBuffer(int w,int h);
    //void resizeEvent ( QResizeEvent * );
    //void viewportPaintEvent (QPaintEvent *);

    //    void contentsWheelEvent ( QWheelEvent *e );

    void setWindowOrg();


    virtual void paintEvent ( QPaintEvent * event );
    //再描画
    void drawContents(QPainter*p,int clipx,int clipy, int clipw, int cliph );
    //drawContentsにともなうバッファの再描画
    //渡される無効エリアの座標はコンテンツ座標系
    //    void updateViewBuffer(int clipx,int clipy, int clipw, int cliph );
    //    void updateViewBuffer(bool bForceUpdate = false);//int clipx,int clipy, int clipw, int cliph );
    //バッファの再描画
    //渡される無効エリアの座標はコンテンツ座標系
    //    void drawBuffer(int x,int y,int clipx,int clipy, int clipw, int cliph );


};

#endif
