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

#ifndef XBSCHDOCSCH_H
#define XBSCHDOCSCH_H

#include "xbschdoc.h"


class SXBSchDocSch : public SXBSchDoc
{

public:
    SXBSchDocSch(QWidget *mainWindow):SXBSchDoc(mainWindow) {}
    ~SXBSchDocSch() {}

    //***** 編集 *****
    //コピー
    //コピーが行われたときにtrueを返す
    bool copy();

    //貼り付け
    //ドキュメントに変更があったときtrueを返す
    bool paste(const SPoint& pt);

    //貼り付け可能か？
    bool canPaste();
};

#endif


/*
  Local Variables:
  mode: c++
  End:
 */
