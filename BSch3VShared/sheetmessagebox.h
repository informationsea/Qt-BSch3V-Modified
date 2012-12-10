/*******************************************************************************
 *   This file is part of Qt-BSch3V Modified.
 *   Copyright (C) 2002-2005 H.Okada
 *   Copyright (C) 2010-2011 Stork Lab
 *
 *   Qt-BSch3V Modified is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Qt-BSch3V Modified is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#ifndef SHEETMESSAGEBOX_H
#define SHEETMESSAGEBOX_H

#include "BSch3VShared_global.h"

#include <QMessageBox>

class SheetMessageBox
{
private:
    SheetMessageBox() {}
    static QMessageBox* makeMessageBox
    ( QWidget * parent, const QString & title, const QString & text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton );

public:
    static QMessageBox::StandardButton critical
    ( QWidget * parent, const QString & title, const QString & text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton );

    static QMessageBox::StandardButton information
    ( QWidget * parent, const QString & title, const QString & text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton );

    static QMessageBox::StandardButton question
    ( QWidget * parent, const QString & title, const QString & text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton );

    static QMessageBox::StandardButton warning
    ( QWidget * parent, const QString & title, const QString & text,
      QMessageBox::StandardButtons buttons = QMessageBox::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::NoButton );
};

#endif // SHEETMESSAGEBOX_H
