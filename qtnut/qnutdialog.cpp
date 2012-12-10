/****************************************************************************
    QtNut   Numbering utility for Qt-BSch3V
    Copyright (C) 2004-2006 H.Okada

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

#include "qnutdialog.h"
#include <QPixmap>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include "nu3w.xpm"
#include "snut.h"

#define PARTNUM_INIT_MAX 1000
#define PARTNUM_STEP_MAX 20



qnutdialog::qnutdialog(QWidget* parent) 
    : QDialog(parent)
{
    ui.setupUi(this);
    setWindowIcon(QIcon(nu3w));

    ui.checkBoxAnno->setChecked ( true );

    connect( ui.okButton, SIGNAL( clicked() ), this, SLOT( execute() ) );
    connect( ui.pushButtonRef, SIGNAL( clicked() ), this, SLOT( OnButtonRef() ) );
    connect( ui.pushButtonDel, SIGNAL( clicked() ), this, SLOT( OnButtonDel() ) );
    connect( ui.pushButtonUp, SIGNAL( clicked() ), this, SLOT( OnButtonUp() ) );
    connect( ui.pushButtonDown, SIGNAL( clicked() ), this, SLOT( OnButtonDown() ) );
    connect( ui.pushButtonAbout, SIGNAL( clicked() ), this, SLOT( OnButtonAbout() ) );
    connect( ui.listFilename, SIGNAL( currentRowChanged(int) ), this, SLOT( OnSelChange(int) ) );

    setButtonEnable();
}

void qnutdialog::execute()
{
    SNut snut;

    int nInit= ui.spinBoxInit->value();
    if(nInit<1 || nInit>PARTNUM_INIT_MAX){
        QMessageBox::warning(this,"Qt-Nut WARNING","Initial value must be 1..1000",
                             QMessageBox::Ok, 0);
        return;
    }

    int nStep= ui.spinBoxStep->value();
    if(nStep<1 || nStep>PARTNUM_STEP_MAX){
        QMessageBox::warning(this,"Qt-Nut WARNING","Step value must be 1..20",
                             QMessageBox::Ok, 0);
        return;
    }


    int nCount = ui.listFilename->count();
    if(nCount==0){
        return;
    }
    QString str;
    //int readCount=0;
    for(int n=0;n<nCount;n++){
        str = (ui.listFilename->item(n))->text();
        snut.readFile(str.toLocal8Bit());
    }
    if(snut.fileCount()==0)return;


    bool anumMode = ui.checkBoxAnno->isChecked();
    if(anumMode){
        snut.anum(nInit,nStep);
    }else{
        snut.renum(nInit,nStep);
    }
    snut.saveFiles();
    QMessageBox::information (this,"Qt-Nut","Finished", QMessageBox::Ok);
}

void qnutdialog::OnButtonRef()
{
    int count = ui.listFilename->count();
    int nSelect = -1;
    if(count>0){
        nSelect = ui.listFilename->currentRow();
    }

    QStringList sl = QFileDialog::getOpenFileNames( this,QString::null,QString::null,"*.ce3");

    if(!sl.isEmpty()){
        for ( QStringList::Iterator it = sl.begin(); it != sl.end(); ++it ){
            ui.listFilename->insertItem(nSelect,*it);
        }
    }
    if(ui.listFilename->count() > 0 && nSelect<0){
        ui.listFilename->setCurrentRow (0);
        //		QString srcName =(ui.listFilename->item(0))->text();
        //		setDstName(srcName);
    }
    setButtonEnable();
}

void qnutdialog::OnButtonDel()
{
    int nCount = ui.listFilename->count();
    if(nCount == 0) return;
    int nSelect = ui.listFilename->currentRow();
    if(nSelect<0)nSelect = 0;
    ui.listFilename->takeItem(nSelect);
    nCount--;
    if(nCount>0){
        if(nSelect>=nCount)nSelect= nCount-1;
        if(nSelect>=0)ui.listFilename->setCurrentRow(nSelect);
        //		QString srcName =(ui.listFilename->item(nSelect))->text();
        //		setDstName(srcName);
        //	}else{
        //		ui.lineEditOutput->setText("");
    }
    setButtonEnable();
}

void qnutdialog::OnButtonUp()
{
    int nCount = ui.listFilename->count();
    if(nCount == 0) return;
    int nSelect = ui.listFilename->currentRow();
    if(nSelect<1) return;
    QString fname = (ui.listFilename->item(nSelect))->text();
    ui.listFilename->takeItem(nSelect);
    nSelect--;
    ui.listFilename->insertItem(nSelect,fname);
    ui.listFilename->setCurrentRow(nSelect);
    setButtonEnable();
}

void qnutdialog::OnButtonDown()
{
    int nCount = ui.listFilename->count();
    if(nCount == 0) return;
    int nSelect = ui.listFilename->currentRow();
    if(nSelect>(nCount-2)) return;
    QString fname = (ui.listFilename->item(nSelect))->text();
    ui.listFilename->takeItem(nSelect);
    nSelect++;
    ui.listFilename->insertItem(nSelect,fname);
    ui.listFilename->setCurrentRow(nSelect);
    setButtonEnable();
}


void qnutdialog::OnButtonAbout()
{
    QPixmap icon( (const char**)nu3w );

    QMessageBox mb(this);

    mb.setMinimumWidth(400);
    mb.setWindowTitle("About  Qt-Nut");
    mb.setTextFormat(Qt::RichText);
    mb.setText(
                "<center><b>Qt-Nut  Version 0.43</b><br><br>"
                "Copyright(C) 2006 H.Okada(Suigyodo)<br>"
                "Numbering utility for Qt-BSch3V.<br>"
                "URL  http://www.suigyodo.com/online<br>"
                "E-Mail  hitoshi@suigyodo.com</center>"
		);
    mb.setIconPixmap(icon);
    mb.exec();
}

void qnutdialog::OnSelChange(int /* n */)
{
    setButtonEnable();
}

void qnutdialog::setButtonEnable()
{
    int nCount = ui.listFilename->count();
    int nSelect = ui.listFilename->currentRow();
    ui.okButton->setEnabled(nCount>0);
    if(nCount<1 || nSelect<0){
        ui.pushButtonDel->setEnabled(false);
        ui.pushButtonUp->setEnabled(false);
        ui.pushButtonDown->setEnabled(false);
        return;
    }
    ui.pushButtonDel->setEnabled(true);
    ui.pushButtonUp->setEnabled(nSelect>0);
    ui.pushButtonDown->setEnabled(nSelect<(nCount-1));
    //printf("setButtonEnable()   count:%d  select:%d\n",nSelect,nCount);
}
