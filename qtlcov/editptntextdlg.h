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

#ifndef EDITPTNTEXTIMPL_H
#define EDITPTNTEXTIMPL_H

#include <QtGui>
#include "ui_editptntextdlg.h"

class editptntextdlg : public QDialog
{
    Q_OBJECT

public:
    editptntextdlg(QWidget *parent = 0);
    void setUseLabelFont(bool useLFont);
    bool useLabelFont();
    void setFont(QFont& fn) {
        m_font = fn;
    }
    QFont font() {
        return m_font;
    }
    void setText(const QString& text);
    QString text();
    void setAlign(int align);
    int Align();

private:
    QFont m_font;
    //bool m_useLabelFont;
    //QString m_text;
    Ui::editptntextdlg ui;
    //int m_align;

protected  slots:
    void fontbuttonClicked();
    void useLabelFontCheckChanged(int state);
};



#endif
