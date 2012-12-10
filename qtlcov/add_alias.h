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

#ifndef ADD_ALIAS_H
#define ADD_ALIAS_H

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
class QLineEdit;
class QLabel;
class QPushButton;

class add_alias : public QDialog
{
    Q_OBJECT

public:
    add_alias( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~add_alias();

    QLineEdit* editAlias;
    QLabel* textLabel1;
    QPushButton* buttonOK;
    QPushButton* buttonCancel;
    QLabel* labelAliasOf;

protected:
    QGridLayout* add_aliasLayout;

protected slots:
    virtual void languageChange();

};

#endif // ADD_ALIAS_H
