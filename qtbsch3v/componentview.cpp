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

#include "componentview.h"

#include <qpainter.h>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QPalette>
#include <QSize>
#include "complib.h"
#include "xbsch.h"
#include "drawcomponentbase.h"

//コンストラクタ
SComponentWidget::SComponentWidget(QWidget* parent):QWidget (parent)
{
    //    m_bLcoMode = lcomode;
    m_block           = 0;
    m_pComponentIndex = NULL;
    m_cursorLTRB      = -1;
    m_cursorOffset    = -1;

    QPalette palette;
    palette.setColor(QPalette::Window,Qt::white);
    setPalette(palette);
    //m_drawframe = false;
    //viewport()->setBackgroundColor(QColor("white"));
    setMouseTracking(true);
}

//デストラクタ
SComponentWidget::~SComponentWidget()
{
}

QSize SComponentWidget::getContentsSize()
{
    int width;
    int height;
    if(!m_pComponentIndex) {
        width  = 0;
        height = 0;
    } else {
        width  = m_pComponentIndex->size().w()*10;
        height = m_pComponentIndex->size().h()*10;
    }
    return QSize(width+80,height+80);
}

void SComponentWidget::setComponentIndex(const SCompIndex* pComponentIndex)
{
    m_cursorLTRB   = -1;
    m_cursorOffset = -1;
    m_pComponentIndex = pComponentIndex;
    QSize size = getContentsSize();
    setMinimumSize(size);
    //resize(size.width(),size.height());
    update();
}

void SComponentWidget::setBlock(int block)
{
    if(!m_pComponentIndex) return;
    if(m_pComponentIndex->block() <= block) return;
    m_block = block;
    update();
}



void  SComponentWidget::paintEvent ( QPaintEvent * event )
{
    QRect rcClip = event->rect();
    QPainter painter(this);

    painter.eraseRect(rcClip);
    SRect srcClip(rcClip.x(),rcClip.y(),rcClip.width(),rcClip.height());
    if(!m_pComponentIndex) return;

    int w = getContentsSize().width();
    int h = getContentsSize().height();
    SSize size(0,0);

    g_drawComponentBase(&painter,
                        m_pComponentIndex,
                        SPoint(w-40,h-40),
                        size,//compIndex がNULLのときだけ使う
                        0,
                        m_block,
                        DRAW_ON,
                        &srcClip,
                        1,
                        true);
}

void SComponentWidget::resizeEvent(QResizeEvent * )
{
    //    qDebug("viewportResizeEvent");
}

void SComponentWidget::mousePressEvent( QMouseEvent * /* event */)
{
    //qDebug("SComponentWidget::mousePressEvent X=%d,Y=%d",event->x(),event->y());
}

