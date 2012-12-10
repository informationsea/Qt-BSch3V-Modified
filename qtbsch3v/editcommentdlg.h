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

#ifndef EDITCOMMENTDLGIMPL_H
#define EDITCOMMENTDLGIMPL_H

#include <QtGui>
#include "ui_editcommentdlg.h"

class editcommentdlg : public QDialog
{
    Q_OBJECT

public:
    editcommentdlg(QWidget *parent = 0);

    void setFont(QFont& fn) {
        m_font = fn;
    }
    QFont font() {
        return m_font;
    }
    void setText(const QString& text);
    QString text();


private:
    QFont m_font;
    QString m_text;
    Ui::editcommentdlg ui;

protected  slots:
    void fontbuttonClicked();
};

#endif


/*
  Local Variables:
  mode: c++
  End:
 */
