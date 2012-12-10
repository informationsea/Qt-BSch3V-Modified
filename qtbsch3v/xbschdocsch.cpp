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

//XBSchのドキュメントの管理を行う。

#include <qapplication.h>
#include <qwidget.h>
#include <qstring.h>
//#include <SSize.h>
//#include <SPoint.h>
#include <qclipboard.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <list>
using namespace std;
#include "ce3io.h"
#include "xbschobj.h"
#include "xbschjunc.h"
#include "xbschlabel.h"
#include "xbschcomment.h"
#include "xbschtag.h"
#include "xbschcomponent.h"

#include "xbschline.h"
#include "xbschentry.h"
#include "xbschdelobj.h"
#include "xbschsheetinfo.h"
#include "xbschdoc.h"
#include "xbschdocsch.h"

//***** Qt依存 *****
//コピー
//コピーが行われたときにtrueを返す
bool SXBSchDocSch::copy()
{
    if(!m_listTemp.size())return false;
    SPoint ptOrigin = SPoint(m_rcTempArea.l(),m_rcTempArea.t());
    ptOrigin.setX(((ptOrigin.x()+5)/10)*10);
    ptOrigin.setY(((ptOrigin.y()+5)/10)*10);

    string str;
    SWriteCE3 wce3;
    wce3.Attach(&str);

    wce3.WriteRecord("+BSCH3_DATA_V.1.0");
    wce3.WriteEOL();
    writeCe3(wce3,&m_listTemp,true,&ptOrigin);
    wce3.WriteRecord("-BSCH3_DATA_V.1.0");
    wce3.WriteEOL();

    QClipboard *cb = QApplication::clipboard();
    cb->setText(QString::fromLocal8Bit(str.c_str()));

    return true;
}

//***** Qt依存 *****
//貼り付け
//ドキュメントに変更があったときtrueを返す
bool SXBSchDocSch::paste(const SPoint& pt)
{
    if(!canPaste()) return false;
    QClipboard *cb = QApplication::clipboard();
    string cbbuff = cb->text().toStdString();

    SReadCE3 rce3;
    rce3.Attach(cbbuff.c_str());

    string str;

    //bool readLib = (str == +BSCH3_LIB_V.1.0);

    rce3.ReadRecord(str);
    if(str == "+BSCH3_DATA_V.1.0") {

        readCe3(rce3,&m_listTemp,&pt,NULL);
        if(!m_listTemp.size()) return false;

        moveToLayer(m_editLayer);

        setupRcTempArea();

        m_bDirty = true;
        return true;
    } else {
        return false;
    }
}
//貼り付け可能か？
bool SXBSchDocSch::canPaste()
{
    QString strID("+BSCH3_DATA_V.1.0");
    QClipboard *cb = QApplication::clipboard();
    QString str = cb->text().left(strID.length());
    if(str == strID) return true;
    return false;
}


