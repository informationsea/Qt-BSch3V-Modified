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


#include <QMessageBox>
#include <QPageSetupDialog>
#include "sheetmessagebox.h"
#include "printOptionDlg.h"



printOptionDlg::printOptionDlg(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowModality(Qt::ApplicationModal);

    m_left    = 12;
    m_right    = 12;
    m_top    = 12;
    m_bottom    = 12;
    m_color = true;

    //connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
    //connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

int printOptionDlg::exec()
{
    QString str;
    str.sprintf("%d",m_left);
    ui.editLeft->setText(str);
    str.sprintf("%d",m_right);
    ui.editRight->setText(str);
    str.sprintf("%d",m_top);
    ui.editTop->setText(str);
    str.sprintf("%d",m_bottom);
    ui.editBottom->setText(str);

    ui.colorCheckbox->setCheckState(m_color ? Qt::Checked : Qt::Unchecked);

    return QDialog::exec();
}

void printOptionDlg::accept()
{
#define PRINTMARGIN_UPPERLIMIT 50

    m_left     	= ui.editLeft->text().toInt();
    m_right     = ui.editRight->text().toInt();
    m_top     	= ui.editTop->text().toInt();
    m_bottom     = ui.editBottom->text().toInt();
    m_color     = ui.colorCheckbox->checkState() == Qt::Checked;

    if(     m_left < 0 || m_left > PRINTMARGIN_UPPERLIMIT
            ||    m_right < 0 || m_right > PRINTMARGIN_UPPERLIMIT
            ||    m_top < 0 || m_top > PRINTMARGIN_UPPERLIMIT
            ||    m_bottom < 0 || m_bottom > PRINTMARGIN_UPPERLIMIT) {


        QMessageBox::critical( NULL, "Print Option","Invalid print margin");
        return;
    }

    QDialog::accept();
}
