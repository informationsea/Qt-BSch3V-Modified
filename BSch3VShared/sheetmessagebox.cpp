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

#include "sheetmessagebox.h"

QMessageBox* SheetMessageBox::makeMessageBox
( QWidget * parent, const QString & title, const QString & text,
  QMessageBox::StandardButtons buttons,
  QMessageBox::StandardButton defaultButton)
{

    QMessageBox *mesbox = new QMessageBox(parent);
    if(parent)
        mesbox->setWindowModality(Qt::WindowModal);
    else
        mesbox->setWindowModality(Qt::ApplicationModal);
#ifdef Q_WS_MACX
    mesbox->setText(title);
    mesbox->setInformativeText(text);
#else
    mesbox->setWindowTitle(title);
    mesbox->setText(text);
#endif
    mesbox->setStandardButtons(buttons);
    mesbox->setDefaultButton(defaultButton);

    return mesbox;
}

QMessageBox::StandardButton SheetMessageBox::critical
( QWidget * parent, const QString & title, const QString & text,
  QMessageBox::StandardButtons buttons,
  QMessageBox::StandardButton defaultButton)
{

    QMessageBox *mesbox = makeMessageBox(parent,title,text,buttons,defaultButton);
    mesbox->setIcon(QMessageBox::Critical);

    mesbox->exec();
    QMessageBox::StandardButton ret =
        mesbox->standardButton(mesbox->clickedButton());

    delete mesbox;

    return ret;
}

QMessageBox::StandardButton SheetMessageBox::information
( QWidget * parent, const QString & title, const QString & text,
  QMessageBox::StandardButtons buttons,
  QMessageBox::StandardButton defaultButton)
{

    QMessageBox *mesbox = makeMessageBox(parent,title,text,buttons,defaultButton);
    mesbox->setIcon(QMessageBox::Information);

    mesbox->exec();
    QMessageBox::StandardButton ret =
        mesbox->standardButton(mesbox->clickedButton());

    delete mesbox;

    return ret;
}

QMessageBox::StandardButton SheetMessageBox::question
( QWidget * parent, const QString & title, const QString & text,
  QMessageBox::StandardButtons buttons,
  QMessageBox::StandardButton defaultButton)
{

    QMessageBox *mesbox = makeMessageBox(parent,title,text,buttons,defaultButton);
    mesbox->setIcon(QMessageBox::Question);

    mesbox->exec();
    QMessageBox::StandardButton ret =
        mesbox->standardButton(mesbox->clickedButton());

    delete mesbox;

    return ret;
}

QMessageBox::StandardButton SheetMessageBox::warning
( QWidget * parent, const QString & title, const QString & text,
  QMessageBox::StandardButtons buttons,
  QMessageBox::StandardButton defaultButton)
{

    QMessageBox *mesbox = makeMessageBox(parent,title,text,buttons,defaultButton);
    mesbox->setIcon(QMessageBox::Warning);

    mesbox->exec();
    QMessageBox::StandardButton ret =
        mesbox->standardButton(mesbox->clickedButton());

    delete mesbox;

    return ret;
}
