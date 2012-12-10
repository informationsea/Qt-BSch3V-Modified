/****************************************************************************
    QtNlist   Netlist & Partslist utility for Qt-BSch3V
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

#ifndef QNLDIALOG_H
#define QNLDIALOG_H

#include "ui_qnlmaindlg.h"


class QnlDialog : public QDialog
{
    Q_OBJECT

public:
    QnlDialog( QWidget* parent = 0);
	~QnlDialog(){};
	
private:
	Ui::qnlMain ui;

public slots:
	virtual void accept();
	virtual void OnButtonRef();
    virtual void OnButtonDel();
    virtual void OnButtonAbout();
    virtual void OnSelChange(int);
//    virtual void OnButtonExecute();

	
    virtual void OnModeChange(bool);
//    virtual void OnButtonQuit();
//	virtual void reject();

protected:
	
	void setButtonEnable();
	void setDstName(QString& srcFile);

private:
    QPixmap image0;

};

#endif // QTNLISTDIALOG_H
