/****************************************************************************
    Qt-LCoV  Library Editor for Qt-BSch3V
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
#include "editptntextdlg.h"
#include <QDesktopWidget>
#include <QFontDialog>


editptntextdlg::editptntextdlg(QWidget *parent)
    : QDialog(parent)
{
    //setModal(true);    //exec()で起動するなら、この設定はいらない。
    ui.setupUi(this);
    connect(ui.buttonFont,SIGNAL(clicked()), this, SLOT( fontbuttonClicked()));
    connect(ui.checkUseLabelFont,SIGNAL(stateChanged(int)), this, SLOT( useLabelFontCheckChanged(int)));
    setText("");
    setAlign(0);
    setUseLabelFont(true);
}

void editptntextdlg::setText(const QString& text)
{
    ui.editText->setText(text);
}

QString editptntextdlg::text()
{
    return ui.editText->text();
}

void editptntextdlg::setAlign(int align)
{
    ui.radioLeft->setChecked((align & 3) == 0);
    ui.radioHCenter->setChecked((align & 3) == 2);
    ui.radioRight->setChecked((align & 3) == 1);
    ui.radioBottom->setChecked((align & 12) == 0);
    ui.radioVCenter->setChecked((align & 12) == 8);
    ui.radioTop->setChecked((align & 12) == 4);
}

int editptntextdlg::Align()
{
    int align = 0;
    if(ui.radioRight->isChecked()) align |= 1;
    else if(ui.radioHCenter->isChecked()) align |= 2;

    if(ui.radioTop->isChecked()) align |= 4;
    else if(ui.radioVCenter->isChecked()) align |= 8;

    return align;
}

void editptntextdlg::fontbuttonClicked()
{
    int dpi,point,pixel;
    QDesktopWidget *desktop = QApplication::desktop();
    dpi = desktop->logicalDpiX();
    QFont font;
    bool ok;
    pixel = m_font.pixelSize();
    point=(pixel*72+dpi-1)/dpi;
    font = m_font;
    font.setPointSize(point);
    font = QFontDialog::getFont(&ok,font,this);
    if(ok) {
        point = font.pointSize();
        pixel = point*dpi/72;
        font.setPixelSize(pixel);
        m_font = font;
    }
}

void editptntextdlg::useLabelFontCheckChanged(int state)
{
    ui.buttonFont->setEnabled(state == 0);
    if(!state) {
        m_font = QFont();
        m_font.setPixelSize(10);
    }
}

void editptntextdlg::setUseLabelFont(bool useLFont)
{
    ui.checkUseLabelFont->setChecked(useLFont);
    useLabelFontCheckChanged(useLFont);
}

bool editptntextdlg::useLabelFont()
{
    return ui.checkUseLabelFont->isChecked();
}


