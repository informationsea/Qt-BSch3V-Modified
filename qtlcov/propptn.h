/****************************************************************************
    Qt-LCoV Library Editor for Qt-BSch3V
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

#ifndef PROP_PTN_H
#define PROP_PTN_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QCheckBox;
class QComboBox;
class QPushButton;

class prop_ptn : public QDialog
{
    Q_OBJECT

public:
    prop_ptn( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~prop_ptn();

    QLabel* textLabel1;
    QCheckBox* checkDot;
    QCheckBox* checkFill;
    QComboBox* comboWidth;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;

protected:
    QGridLayout* prop_ptnLayout;

protected slots:
    virtual void languageChange();

};

#endif // PROP_PTN_H
