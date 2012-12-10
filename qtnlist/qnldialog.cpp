/****************************************************************************
    QtNlist   Netlist & Partslist utility for Qt-BSch3V
    Copyright (C) 2004-2006 H.Okada

    QtNList Modified for Qt-BSch3V Modified
    Copyright (C) 2010 Stork

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

#include "qnldialog.h"
#include <QPixmap>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include "nl3c/netlist.h"
#include "nlicon.xpm"


QnlDialog::QnlDialog(QWidget* parent) 
    : QDialog(parent)
{
    ui.setupUi(this);
    //setIcon(nlicon);

    ui.radioNetlist->setChecked ( true );
    ui.radioPartslist->setChecked ( false );
    ui.checkNotListNetNOCONNECTION->setChecked ( true );

    //connect( buttonOk, SIGNAL( clicked() ), this, SLOT( OnButtonExecute() ) );
    //connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( ui.refButton, SIGNAL( clicked() ), this, SLOT( OnButtonRef() ) );
    connect( ui.delButton, SIGNAL( clicked() ), this, SLOT( OnButtonDel() ) );
    connect( ui.radioNetlist, SIGNAL( toggled(bool) ), this, SLOT( OnModeChange(bool) ) );
    connect( ui.listFilename, SIGNAL( currentRowChanged(int) ), this, SLOT( OnSelChange(int) ) );
    connect( ui.aboutButton, SIGNAL( clicked() ), this, SLOT( OnButtonAbout() ) );
    setButtonEnable();
}

void QnlDialog::accept()
{
    SNetList netlist;
    QFileInfo fi;
    QString name;
    QString msg;
    QString srcName;// =listFilename->text(0);

    int srcfiles = ui.listFilename->count();
    if(srcfiles>0){
        QString dst = ui.lineEditOutput->text();
        QString rpt = ui.lineEditReport->text();

        FILE *fp = fopen(dst.toLocal8Bit(),"w");
        if(!fp){
            fi=QFileInfo(dst);
            name = fi.fileName();
            msg = "Destination file open error : ";
            msg += name;
            QMessageBox::warning(this,"Qt-Nlist",msg);
            return;
        }

        FILE *fpRpt = NULL;

        if(ui.radioNetlist->isChecked()){
            fpRpt = fopen(rpt.toLocal8Bit(),"w");
            if(!fpRpt){
                fi=QFileInfo(rpt);
                name = fi.fileName();
                msg = "Report file open error : ";
                msg += name;
                QMessageBox::warning(this,"Qt-Nlist",msg);
                return;
            }
        }


        for(int n=0;n<srcfiles;n++){
            QString srcName =(ui.listFilename->item(n))->text();
            SXBSchDoc* pDoc=new SXBSchDoc;
            if(pDoc->readFile(srcName.toLocal8Bit())!=0){
                fclose(fp);
                fi=QFileInfo(srcName);
                name = fi.fileName();
                msg = "Open error : ";
                msg += name;
                QMessageBox::warning(this,"Qt-Nlist",msg);
                return;
            }
            netlist.setData(&(pDoc->m_listMain));
            delete pDoc;
        }
        if(ui.radioNetlist->isChecked()){
            netlist.createPartsList();
            netlist.createNetList(ui.checkNotListNetNOCONNECTION->isChecked());
            netlist.writeTeleNetList(fp);
            netlist.writeReport(fpRpt);
        }else{
            netlist.createPartsList();
            netlist.writePartsList(fp);
        }
        fclose(fp);
        if(fpRpt) fclose(fpRpt);
        QMessageBox::information(this,QString::fromLocal8Bit("Qt-Nlist"),QString::fromLocal8Bit("Finished."));

    }else{
        setButtonEnable();
    }

}

void QnlDialog::OnButtonRef()
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
        QString srcName =(ui.listFilename->item(0))->text();
        setDstName(srcName);
    }
    setButtonEnable();
}

void QnlDialog::OnButtonDel()
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
        QString srcName =(ui.listFilename->item(nSelect))->text();
        setDstName(srcName);
    }else{
        ui.lineEditOutput->setText("");
    }
    setButtonEnable();
}


void QnlDialog::OnButtonAbout()
{
    QPixmap icon( (const char**)nlicon );

    QMessageBox mb(this);

    mb.setMinimumWidth(400);
    //mb.setCaption("About  Qt-Nlist");
    mb.setWindowTitle("About Qt-Nlist");
    mb.setTextFormat(Qt::RichText);
    mb.setText(
            "<center><b>Qt-Nlist Modified Version 0.47.003</b><br><br>"
            "Copyright (C) 2010 Stork Lab<br>"
            "URL  http://storklab.cyber-ninja.jp<br>"
            "E-Mail  storklab@gmail.com<br>"
            "</center>"
            "<center><b>Qt-Nlist  Version 0.47</b><br><br>"
            "Copyright(C) 2004-2006 H.Okada(Suigyodo)<br>"
            "Qt-Nlist comes with absolutely no warranty.<br>"
            "URL  http://www.suigyodo.com/online<br>"
            "E-Mail  hitoshi@suigyodo.com</center>"
            );
    mb.setIconPixmap(icon);
    mb.exec();
}


void QnlDialog::OnSelChange(int nSelect)
{
    int count = ui.listFilename->count();
    if(count==0){
        ui.lineEditOutput->setText("");
        return;
    }
    //	int nSelect = ui.listFilename->currentRow();
    if(nSelect<0){
        ui.listFilename->setCurrentRow (0);
        return;
    }
    QString srcName =(ui.listFilename->item(nSelect))->text();
    setDstName(srcName);
}

void QnlDialog::OnModeChange(bool)
{
    bool netlistMode = ui.radioNetlist->isChecked();
    ui.lineEditReport->setEnabled(netlistMode);
    ui.checkNotListNetNOCONNECTION->setEnabled(netlistMode);

    int count = ui.listFilename->count();
    if(count==0){
        ui.lineEditOutput->setText("");
        ui.lineEditReport->setText("");
        return;
    }

    int nSelect = ui.listFilename->currentRow();
    if(nSelect<0){
        ui.listFilename->setCurrentRow (0);
    }
    QString srcName= (ui.listFilename->item(nSelect))->text();
    setDstName(srcName);
}



void QnlDialog::setButtonEnable()
{
    int nCount = ui.listFilename->count();
    ui.delButton->setEnabled(nCount>0);
    ui.executeButton->setEnabled(nCount>0);
}


void QnlDialog::setDstName(QString& srcFile)
{
    QString newExt;
    if(ui.radioNetlist->isChecked()){
        newExt = ".net";
    }else{
        newExt = ".csv";
    }
    QFileInfo fi(srcFile);
    QString dstName;
    //QString ext = fi.extension(FALSE);
    QString ext = fi.suffix();
    if(ext.toLower() == "ce3"){
        int length = srcFile.length();
        dstName = srcFile.left(length - 4);
    }else{
        dstName = srcFile;
    }
    //	dstName += newExt;
    ui.lineEditOutput->setText(dstName+newExt);

    if(ui.radioNetlist->isChecked()){
        ui.lineEditReport->setText(dstName+".rpt");
    }else{
        ui.lineEditReport->setText("");
    }
}
