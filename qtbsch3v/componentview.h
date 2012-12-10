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

#ifndef COMPONENTVIEW_H
#define COMPONENTVIEW_H

#include <QWidget>
#include <QScrollArea>
#include <qcursor.h>
#include <QMouseEvent>
#include <QResizeEvent>

#include <string>
#include <list>
using namespace std;

class QPainter;
//class QCursor;

#include "coord.h"
#include "complib.h"


class SComponentWidget: public QWidget
{
    Q_OBJECT

public:
    SComponentWidget(QWidget* parent=0);
    ~SComponentWidget();

    //操作
    void setComponentIndex(const SCompIndex* pComponentIndex);
    void setBlock(int block);

protected:
    const SCompIndex* m_pComponentIndex;

    bool m_bLcoMode;
    int  m_block;
    int  m_cursorLTRB;
    int  m_cursorOffset;

protected:
    QSize getContentsSize();
private slots:

protected:
    void paintEvent ( QPaintEvent * event );
    void resizeEvent(QResizeEvent * );
    void mousePressEvent ( QMouseEvent * event );

};

#if 0

class SComponentView: public QScrollArea
{
    Q_OBJECT

public:
    SComponentView(QWidget* parent,bool lcomode);    //コンストラクタ
    ~SComponentView();                                //デストラクタ

    //表示に関わるもの
    void RefreshView() {
        viewport()->update();
    }

    //操作
    void setComponentIndex(const SCompIndex* pComponentIndex);
    void setBlock(int block);
    int  cursorLTRB() {
        return m_cursorLTRB;
    }
    int  cursorOffset() {
        return m_cursorOffset;
    }

protected:
    const SCompIndex* m_pComponentIndex;

    bool m_bLcoMode;
    int  m_block;
    int  m_cursorLTRB;
    int  m_cursorOffset;

protected:
    QSize getContentsSize();    //スクロールビューのコンテンツのサイズを決める
    SPoint pinEndPos(int ltrb,int offset);    //LTRBとOffsetで示される座標から実際のピン端点座標を求める
    void drawFrame(QPainter*p,const SRect* prcClip);
    void drawCursor(QPainter*p,const SRect* prcClip);

signals:
    void cursorMoved();
    //void toolChange();

private slots:

protected:
    void mousePressEvent(QMouseEvent*);
    //    void contentsMouseReleaseEvent(QMouseEvent *);
    //    void contentsMouseMoveEvent(QMouseEvent *me);
    void paintEvent ( QPaintEvent * event );
    void drawContents(QPainter*p,int clipx,int clipy, int clipw, int cliph );    //再描画
    void resizeEvent(QResizeEvent * );

    int contentsWidth();
    int contentsHeight();
    int contentsX();
    int contentsY();
    void resizeContents(int width,int height);

};
#endif

#endif


/*
  Local Variables:
  mode: c++
  End:
*/

