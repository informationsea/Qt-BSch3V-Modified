/****************************************************************************
    Qt-BSch3V schematic capture
    Copyright (C) 2002-2005 H.Okada
    Copyright (C) 2009-2010 Stork Lab

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

#ifndef XBSCHVIEW_H
#define XBSCHVIEW_H

#include <QWidget>
#include <QScrollArea>
#include <QClipboard>
#include <QBitmap>
#include <QTimer>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
//#include <QWheelEvent>
#include <QResizeEvent>
#include <QPrinter>


#include "xbsch.h"
#include "xbschdocsch.h"
#include "xbschfont.h"
#include "xbschpattern.h"
#include "application.h"

//class SXBSchDoc;
class SXBSchObj;
class QPainter;
class QCursor;

class SXBSchLabel;
class SXBSchComment;
class SXBSchComponent;
class SXBSchTag;



//class QSize;
//class QRect;
//class QPoint;

#define MAX_SHEET_WIDTH 3000
#define MAX_SHEET_HEIGHT 2000

class SXBSchView: public QWidget
{
    Q_OBJECT
public:

    enum MOVE_OPTION {
        MOVE_OPTION_COMPONENT_NAME = 1,
        MOVE_OPTION_COMPONENT_NUM  = 2
    };


    SXBSchView(QWidget* parent = 0);
    ~SXBSchView();
    SXBSchDocSch* Document() {
        return m_pDoc;
    }

    void print(QPrinter*  pPrinter);

public slots:
    //表示に関わるもの
    void RefreshView();

    //編集に関わるもの
    //コピー
    void copy();
    //カット
    void cut();
    //ペースト
    void paste();
    //削除
    void del();
    //UNDO
    void undo();
    //Attribute
    void attribute();
    //Rotate
    void rotate();
    //Mirror
    void mirror();
    //Invert()
    void invert();


    //シートサイズの設定ダイアログの起動
    void resizeSheet();

public:
    void moveToLayer(int nLayer);

    //コピー可能か？
    bool canCopy() {
        return m_pDoc->canCopy() ;
    }
    //貼り付け可能か？
    bool canPaste() {
        return m_pDoc->canPaste() ;
    }
    //削除可能か？
    bool canDel() {
        return m_pDoc->canDel() ;
    }
    //切り取り可能か？
    bool canCut() {
        return m_pDoc->canCut() ;
    }
    //Undo可能か？
    bool canUndo() {
        return m_pDoc->canUndo() ;
    }
    //Attribute変更が可能か?
    bool canAttribute() {
        return m_pDoc->canAttribute() ;
    }
    //Rotate可能か？
    bool canRotate() {
        return m_pDoc->canRotate() ;
    }
    //Mirror可能か？
    bool canMirror() {
        return m_pDoc->canMirror() ;
    }

    //レイヤーの移動が可能か
    bool canMoveToLayer(int nLayer);


    unsigned visibleLayer();
    void setVisibleLayer(unsigned n);
    int activeLayer();
    void setActiveLayer(int n);
    bool darkenNonActiveLayer();
    void setDarkenNonActiveLayer(bool f);




    //ツール設定に関連するメソッド
    bool setToolMode(BSCH_TOOL_MODE mode);
    BSCH_TOOL_MODE toolMode() {
        return m_toolmode;
    }

    //グリッド表示に関連するメソッド
    void setDisplayGrid(bool display);
    bool displayGrid() {
        return m_bDisplayGrid;
    }
    //スナップの状態に関連するメソッド
    void setSnapGrid(bool snap);
    bool snapGrid() {
        return m_bSnapGrid;
    }


    //シートサイズの設定
    void SetSheetSize(const QSize& size);
    //シートサイズの取得
    QSize SheetSize();

    //ファイルの保存
    int save(const QString& fname);
    //ファイルからの読み込み
    int load(const QString& fname);
    //イメージファイルの出力
    bool exportImage(const QString& fname);

    //最後の保存のあとに変更が入ったかどうかを返す
    bool isDirty() {
        return m_pDoc->isDirty();
    }
    //図面オブジェクトが一つでもあればfalseを返す
    bool isEmpty() {
        return m_pDoc->isEmpty();
    }

    //コンフィギュレーションデータをもとにデフォルトフォントを設定
    void setDefaultFont();
    //    //コンフィギュレーションデータをもとにスモールフォントを設定
    //    void setSmallFont(bool updateview=true);

    void execLabelAttributeDlg(SXBSchLabel* pobj);            //ラベルのアトリビュート変更ダイアログを起動する
    void execTagAttributeDlg(SXBSchTag* pobj);                //タグのアトリビュート変更ダイアログを起動する
    void execComponentAttributeDlg(SXBSchComponent* pobj);    //部品のアトリビュート変更ダイアログを起動する
    void execCommentAttributeDlg(SXBSchComment* pObj);        //コメントのアトリビュート変更ダイアログを起動する


    void setApplicationWindow(ApplicationWindow *window) {
        m_applicationWindow = window;
    }

    //    SXBSchView();

    //public slots:
    //    void setContentsPos ( int x, int y );

    //protected slots:
    //    void scrollBuffer(int x,int y);

    //ズームイン・アウト
public:
    bool CanZoomIn();
    bool CanZoomOut();
    void ZoomIn(bool menucommand);
    void ZoomOut(bool menucommand);
private:
    enum {
        MAX_ZOOM = 4
    };
    void setZoomValue();
    int m_nZoom;            //m_nZoomをもとにして、setZoomValue()で、m_viewScaleとm_viewScaleMulを設定する。
    int  m_viewScale;            //縮小率
    int  m_viewScaleMul;        //拡大率

private:
    ApplicationWindow *m_applicationWindow;

    QPixmap m_viewBuffer;            //描画用バッファ

    bool m_fEditHighLight;


    SXBSchDocSch* m_pDoc;            //ドキュメントへのポインタ
    BSCH_TOOL_MODE m_toolmode;    //現在の操作ツールのモード
    bool m_bDisplayGrid;            //グリッドの表示
    bool m_bSnapGrid;                //グリッドへのスナップの状態

    bool m_bTimer;                    //タイマー稼働中
    bool m_bLButtonDown;            //左ボタンの押下情報
    bool m_bDrawSelectFrame;        //選択枠の描画フラグ
    bool m_bDrawTempLine;            //ワイヤー、バス、破線の布線において描画する仮線描画フラグ
    int  m_nTempLineOrder;            //ワイヤー、バス、破線の布線において水平線が先なら1、垂直線が先なら-1、未決定なら0

    bool m_bInhibitPlaceObject;        //2003/10/26 これがtrueのとき、マウスリリース時のオブジェクトの配置を禁止する。
    bool m_bReserveAttribute;        //2003/10/26 これがtrueのとき、マウスリリース時にアトリビュートを実行する。

    QPoint m_ptLastMoveAtContents;    //カーソルが最後に動いたときのviewport座標
    QPoint m_ptLastMoveAtSheet;        //カーソルが最後に動いたときのsheet座標
    QPoint m_ptLButtonDownAtSheet;    //左ボタンが押下されたときのsheet座標
    QPoint m_ptLastMoveTempAtSheet;    //一時データを最後に動かした位置

    int  m_nAutoScrollX;            //自動スクロールのサイズ
    int  m_nAutoScrollY;            //自動スクロールのサイズ
    SXBSchObj* m_pCursorObj;        //カーソルに付きまとう図面オブジェクト
    int     m_moveOption;                //移動しているものを指定するオプション

    QCursor* m_pCurStd;
    QCursor* m_pCurMov;

    bool m_bCompleteDrawTemp;
    QTimer m_timerTempDraw;

    int m_oldContentsX;
    int m_oldContentsY;
    int m_oldOpCount;
    int m_oldViewScale;
    int m_oldViewScaleMul;
    bool m_clearTempRect;

protected:
    //座標変換に関わるもの
    QPoint ContentsToSheet(const QPoint& point);    //Contentsの座標からsheet座標への変換
    QRect  ContentsToSheet(const QRect& rc);        //Contentsの座標からsheet座標への変換
    QPoint SheetToContents(const QPoint& point);    //sheetの座標からContents座標への変換
    QRect  SheetToContents(const QRect& rc);        //sheetの座標からContents座標への変換
    QPoint snapPoint(const QPoint& point);            //最寄りのグリッドの位置を返す
    //自動スクロールのサイズの量の設定
    void setAutoScrollValue();

    void JumpTo(QPoint& jumpto);//指定図面座標をビューの中心にする

    void drawGrid(QPainter*p,const QRect& rcClip,const QColor& color);

    //リスト上のデータの描画 xbschdrawobject.cpp
    void drawMainXBSchObj(QPainter*p,unsigned  nMode,SRect* prcClip,bool darkenNonActive, int nScale, int nScaleMul);
    void drawTempXBSchObj(QPainter*p,unsigned  nMode,SRect* prcClip);



    //選択フレームの描画                                  xbschdrawobject.cpp
    void drawSelectFrame(QPainter*p);
    //一時データのフレームの描画                          xbschdrawobject.cpp
    void drawTempDataFrame(QPainter*p);
    //カーソルにつきまとう図面オブジェクトの描画         xbschdrawobject.cpp
    void drawCursorObj(QPainter*p);
    //一時線の描画                                          xbschdrawobject.cpp
    void drawTempLine(QPainter*p);

    void resizeBuffer(int w,int h);

    /////////////////////////////////////////
    //  配置に関連するもの
    //ラベルの配置
    bool placeLabel(const QPoint&);        //ラベルの配置
    bool placeTag(const QPoint&);        //タグの配置
    bool placeComponent(const QPoint&);    //部品の配置
    bool placeComment(const QPoint& point);    //コメントの配置

signals:
    void cursorMove(int x,int y);
    void toolChange();
private slots:
    //void whenScroll(int x,int y);
    void onScrollTimer();
    void onTimerDrawTemp();
    //void clipboardChanged();

protected:
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *me);
    void mouseDoubleClickEvent ( QMouseEvent * );
    void resizeEvent ( QResizeEvent * );
    //void viewportPaintEvent (QPaintEvent *);

    //void contentsWheelEvent ( QWheelEvent *e );

    //再描画                                                    xbschdrawobject.cpp
    void drawContents(QPainter*p,int clipx,int clipy, int clipw, int cliph );
    //drawContentsにともなうバッファの再描画
    //渡される無効エリアの座標はコンテンツ座標系
    //    void updateViewBuffer(int clipx,int clipy, int clipw, int cliph );
    void updateViewBuffer(bool bForceUpdate = false);//int clipx,int clipy, int clipw, int cliph );
    //バッファの再描画
    //渡される無効エリアの座標はコンテンツ座標系
    void drawBuffer(int x,int y,int clipx,int clipy, int clipw, int cliph );

    virtual void paintEvent ( QPaintEvent * event );
};


#endif


/*
  Local Variables:
  mode: c++
  End:
 */
