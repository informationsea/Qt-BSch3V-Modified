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
#ifndef PROPCOMPONENTDLGIMPL_H
#define PROPCOMPONENTDLGIMPL_H

#include <QtGui>
#include "ui_propcomponentdlg.h"

class prop_componentdlg : public QDialog
{
    Q_OBJECT

public:
    prop_componentdlg(QWidget *parent = 0);

    void setName(const QString& text);
    QString name();
    void setRef(const QString& text);
    QString ref();
    void setWidth(int n);
    int width();
    void setHeight(int n);
    int height();
    void setBlock(int n);
    int block();
    void setUsePattern(bool use);
    bool usePattern();
    void setUseBitPattern(bool use);
    bool useBitPattern();

    bool isValidName( const char * psz );

private:
    Ui::propcomponentdlg ui;

protected  slots:
    void usePatternChanged(int state);
    void onOK();
};



#endif
