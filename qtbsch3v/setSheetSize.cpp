/****************************************************************************
    Qt-BSch3V schematic capture
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


#include <QtGui>
#include <QMessageBox>
#include "setSheetSize.h"
#include "SheetSizeInfo.h"

setSheetSizeDlg::setSheetSizeDlg(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    m_width =1500;
    m_height =1000;
    setWindowModality(Qt::WindowModal);
}

int setSheetSizeDlg::exec()
{
    //ui.editWidth->setMaxLength(4);
    ui.editWidth->setInputMask("0000");
    ui.editWidth->setText(QString::number(m_width));

    //ui.editHeight->setMaxLength(4);
    ui.editHeight->setInputMask("0000");
    ui.editHeight->setText(QString::number(m_height));

    return QDialog::exec();
}

void setSheetSizeDlg::accept()
{
    int n = ui.editWidth->text().toInt();
    if(n < SHEETSIZE_X_MIN || n > SHEETSIZE_X_MAX) {
        QMessageBox::critical( this, "Set Sheet size",
                               "Width is out of range.");

        return;
    } else {
        m_width = n;
    }
    n = ui.editHeight->text().toInt();
    if(n < SHEETSIZE_Y_MIN || n > SHEETSIZE_Y_MAX) {
        QMessageBox::critical( this, "Set Sheet size",
                               "Height is out of range.");
        return;
    } else {
        m_height = n;
    }
    QDialog::accept();
}


