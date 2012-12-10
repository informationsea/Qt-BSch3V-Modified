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
#include "proppindlg.h"
#include "complib.h"


prop_pindlg::prop_pindlg(QWidget *parent)
    : QDialog(parent)
{
    m_arrayPinNumber = NULL;
    ui.setupUi(this);
    ui.radioNorm->setIcon(QIcon(":/images/pin_norm.png"));
    ui.radioNeg->setIcon(QIcon(":/images/pin_n.png"));
    ui.radioCk->setIcon(QIcon(":/images/pin_c.png"));
    ui.radioNegCk->setIcon(QIcon(":/images/pin_nc.png"));
    ui.radioS->setIcon(QIcon(":/images/pin_s.png"));
    ui.radioZero->setIcon(QIcon(":/images/pin_zleng.png"));
    connect(ui.spinBlock,SIGNAL(valueChanged(int)), this, SLOT( blockChanged(int)));
    connect(ui.buttonOK,SIGNAL(clicked()),this,SLOT(onOK()));
    setName("NAME");
    setNumber(1,NULL);
    setHideNumber(false);
    setPinType(PIN_TYPE_NOMAL);
}

void prop_pindlg::setName(const QString& text)
{
    ui.editName->setText(text);
}

void prop_pindlg::setNumber(int block,QString* arrayPinNumber)
{
    ui.spinBlock->setMinimum(1);
    ui.spinBlock->setMaximum(block);
    ui.spinBlock->setValue(1);
    m_arrayPinNumber = arrayPinNumber;
    m_currentBlock = 1;
    if(m_arrayPinNumber) {
        ui.editNumber->setText(m_arrayPinNumber[0]);
    }
}

QString prop_pindlg::name()
{
    return ui.editName->text();
}

void prop_pindlg::setHideNumber(bool hide)
{
    ui.checkHide->setChecked(hide);
}

bool prop_pindlg::hideNumber()
{
    return ui.checkHide->isChecked();
}

void prop_pindlg::setPinType(int n)
{
    n&=0x0f;
    ui.radioZero->setChecked(n == PIN_TYPE_ZLENG);
    ui.radioCk->setChecked(n == PIN_TYPE_CLOCK);
    ui.radioS->setChecked(n == PIN_TYPE_SMALL);
    ui.radioNeg->setChecked(n == PIN_TYPE_NEGATIVE);
    ui.radioNegCk->setChecked(n == (PIN_TYPE_NEGATIVE|PIN_TYPE_CLOCK));
    ui.radioNorm->setChecked(n == PIN_TYPE_NOMAL);
}

int prop_pindlg::pinType()
{
    if(ui.radioZero->isChecked())     return PIN_TYPE_ZLENG;
    if(ui.radioCk->isChecked())    	return PIN_TYPE_CLOCK;
    if(ui.radioS->isChecked())    	return PIN_TYPE_SMALL;
    if(ui.radioNeg->isChecked())    return PIN_TYPE_NEGATIVE;
    if(ui.radioNegCk->isChecked())    return PIN_TYPE_NEGATIVE|PIN_TYPE_CLOCK;
    return PIN_TYPE_NOMAL;
}

void prop_pindlg::blockChanged(int value)
{
    if(m_arrayPinNumber) {
        m_arrayPinNumber[m_currentBlock-1] = ui.editNumber->text();
        ui.editNumber->setText(m_arrayPinNumber[value-1]);
        m_currentBlock = value;
    }
}

void prop_pindlg::onOK()
{
    if(m_arrayPinNumber) {
        m_arrayPinNumber[m_currentBlock-1] = ui.editNumber->text();
    }
    accept();
}

