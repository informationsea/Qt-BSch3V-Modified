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

#ifndef QNUTDIALOG_H
#define QNUTDIALOG_H

#include "ui_qnutmaindlg.h"


class qnutdialog : public QDialog
{
    Q_OBJECT

public:
    qnutdialog( QWidget* parent = 0);
	~qnutdialog(){};
	
private:
	Ui::qnutmaindlgui ui;

public slots:
	virtual void execute();
	virtual void OnButtonRef();
    virtual void OnButtonDel();
    virtual void OnButtonUp();
    virtual void OnButtonDown();
    virtual void OnButtonAbout();
    virtual void OnSelChange(int n);
	
	

protected:
	void setButtonEnable();
};

#endif // QNUTDIALOG_H
