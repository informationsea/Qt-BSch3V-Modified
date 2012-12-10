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
#ifndef PROPPINDLGIMPL_H
#define PROPPINDLGIMPL_H

#include <QtGui>
#include "ui_proppindlg.h"

class prop_pindlg : public QDialog
{
    Q_OBJECT

public:
    prop_pindlg(QWidget *parent = 0);

    void setName(const QString& text);
    QString name();
    void setNumber(int block,QString* arrayPinNumber);
    void setHideNumber(bool hide);
    bool hideNumber();
    void setPinType(int n);
    int pinType();

private:
    Ui::proppindlg ui;

    QString* m_arrayPinNumber;
    int m_currentBlock;

protected  slots:
    void blockChanged(int value);
    void onOK();
};



#endif
