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
#include <QtGui>
#include <qpainter.h>
#include <qfont.h>
#include <qcolor.h>
#include <qfontinfo.h>
#include <qstring.h>
//#include <q3listbox.h>
#include "complib.h"
#include "lcolistboxitem.h"

#include <QDebug>


QVariant SLCoListBoxItem::data(int role)
{
    qDebug() << role;
    switch(role) {
    case Qt::DisplayRole:
        return "test";
        //return QString::fromLocal8Bit(m_pCompIndex->name());
    case Qt::DecorationRole:
        return QColor(m_bAlias ? "red" : "black");
    default:
        return QVariant("default");
    }
}


QListWidgetItem * SLCoListBoxItem::clone () const
{
    SLCoListBoxItem* copied = new SLCoListBoxItem;
    copied->m_bAlias = m_bAlias;
    copied->m_pCompIndex = m_pCompIndex;
    return copied;
}

/*
void SLCoListBoxItem::paint (QPainter * p)
{
 QFont font;
 //QFontInfo fontInfo(font);
 QColor col;
 int xOffset = 0;

 int dpi,point,pixelSize;
 QDesktopWidget *desktop = QApplication::desktop();
 dpi = desktop->logicalDpiX();
 point = font.pointSize();
 pixelSize = point*dpi/72;


 //int pixelSize = fontInfo.pixelSize();
 //p->drawText(0,pixelSize,text());
 const char* pname = m_pCompIndex->name();
 if(m_bAlias){
  col = QColor("red");
  p->setPen(col);
  xOffset = 20;
 }
//    else{
//    	col = QColor("black");
//    }

 //p->drawText(0,15,QString::fromLocal8Bit(pname));
 p->drawText(xOffset,pixelSize,QString::fromLocal8Bit(pname));

}

int SLCoListBoxItem::height ( const Q3ListBox * ) const
{
 QFont font;
 int dpi,point,pixelSize;
 QDesktopWidget *desktop = QApplication::desktop();
 dpi = desktop->logicalDpiX();
 point = font.pointSize();
 pixelSize = point*dpi/72;

//    QFontInfo fontInfo(font);
//    int pixelSize =fontInfo.pixelSize();
 return pixelSize+2;
}

int SLCoListBoxItem::width ( const Q3ListBox * ) const
{
 return 120;
}
*/
