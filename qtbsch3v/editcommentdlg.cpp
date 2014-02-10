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


#include <QtGui>
#include "editcommentdlg.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QFontDialog>

editcommentdlg::editcommentdlg(QWidget *parent)
    : QDialog(parent)
{
    //setModal(true);    //exec()で起動するなら、この設定はいらない。
    ui.setupUi(this);
    connect(ui.fontButton,SIGNAL(clicked()), this, SLOT( fontbuttonClicked()));
}

void editcommentdlg::setText(const QString& text)
{
    ui.lineEdit->setText(text);
}

QString editcommentdlg::text()
{
    return ui.lineEdit->text();
}

void editcommentdlg::fontbuttonClicked()
{
    int dpi,point,pixel;
    QDesktopWidget *desktop = QApplication::desktop();
    dpi = desktop->logicalDpiX();
    QFont font;
    bool ok;
    pixel = m_font.pixelSize() < 0 ? font.pointSize()*qApp->desktop()->logicalDpiX()/72 : font.pixelSize();;
    point = (pixel*72+dpi-1)/dpi;
    font = m_font;
    font.setPointSize(point);
    //this->hide();
    font = QFontDialog::getFont(&ok,font,this);
    //this->show();
    if(ok) {
        printf("Font:%s\n",(const char *)font.toString().toLocal8Bit());
        point = font.pointSize();
        pixel = point*dpi/72;
        font.setPixelSize(pixel);
        m_font = font;
    }
}

