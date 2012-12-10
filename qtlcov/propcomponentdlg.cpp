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

#include "propcomponentdlg.h"

prop_componentdlg::prop_componentdlg(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    connect(ui.buttonOK,SIGNAL(clicked()), this, SLOT( onOK()));
    connect(ui.checkUsePtn,SIGNAL(stateChanged(int)), this, SLOT( usePatternChanged(int)));
    setName("NAME");
    setRef("U");
    setWidth(2);
    setHeight(2);
    setBlock(1);
    setUsePattern(false);

}

void prop_componentdlg::setName(const QString& text)
{
    ui.editName->setText(text);
}

QString prop_componentdlg::name()
{
    return ui.editName->text();
}

void prop_componentdlg::setRef(const QString& text)
{
    ui.editRef->setText(text);
}

QString prop_componentdlg::ref()
{
    return ui.editRef->text();
}

void prop_componentdlg::setWidth(int n)
{
    ui.editWidth->setText(QString::number(n));
}

int prop_componentdlg::width()
{
    return ui.editWidth->text().toUInt();
}

void prop_componentdlg::setHeight(int n)
{
    ui.editHeight->setText(QString::number(n));
}

int prop_componentdlg::height()
{
    return ui.editHeight->text().toUInt();
}

void prop_componentdlg::setBlock(int n)
{
    ui.editBlock->setText(QString::number(n));
}

int prop_componentdlg::block()
{
    return ui.editBlock->text().toUInt();
}

void prop_componentdlg::setUsePattern(bool use)
{
    ui.checkUsePtn->setChecked(use);
    usePatternChanged(use);
}

bool  prop_componentdlg::usePattern()
{
    return ui.checkUsePtn->isChecked();
}

void  prop_componentdlg::setUseBitPattern(bool use)
{
    if(!usePattern()) return;
    ui.checkNotUseBitPtn->setChecked(!use);
}

bool  prop_componentdlg::useBitPattern()
{
    return !(ui.checkNotUseBitPtn->isChecked());
}

bool  prop_componentdlg::isValidName( const char * psz )
{
    int n = strlen(psz);
    if(n == 0 || n > 15) return false;
    while(*psz) {
        if(  *psz < 0x21
                ||*psz > 0x7E
                ||*psz == '{'
                ||*psz == '}') return false;
        psz++;
    }
    return true;
}

void prop_componentdlg::usePatternChanged(int state)
{
    if(!state) {
        ui.checkNotUseBitPtn->setChecked(true);
        ui.checkNotUseBitPtn->setEnabled(false);
    } else {
        ui.checkNotUseBitPtn->setEnabled(true);
    }
}

void prop_componentdlg::onOK()
{
    QString str;
    str = name();
    if(!isValidName(str.toLatin1())) {
        QMessageBox::critical( this, "Component property",
                               "Invalid component name",
                               "OK",QString::null,QString::null,0,0);
        return;
    }
    int n;

    n = width();
    if(n < 1||255 < n) {
        QMessageBox::critical( this, "Component property",
                               "Width must be between 1 and 255.",
                               "OK",QString::null,QString::null,0,0);
        return;
    }

    n = height();
    if(n < 1||255 < n) {
        QMessageBox::critical( this, "Component property",
                               "Height must be between 1 and 255.",
                               "OK",QString::null,QString::null,0,0);
        return;
    }


    n = block();
    if(n < 1||255 < n) {
        QMessageBox::critical( this, "Component property",
                               "Block must be between 1 and 255.",
                               "OK",QString::null,QString::null,0,0);
        return;
    }

    accept();
}



