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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <qcolor.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <list>
#include <string>
using namespace std;

#include "coord.h"
#include "xbsch.h"
#include "xbschobj.h"
#include "xbschcomponent.h"
#include "xbschpattern.h"
#include "drawcomponent.h"
#include "drawcomponentbase.h"

//部品の描画
void g_drawComponent(QPainter* p,SXBSchComponent* pObj,unsigned  nMode,SRect* prcClip,int nScale,int nScaleMul)
{

    if(!(nMode & DRAW_INH_PARTBODY)) {
        g_drawComponentBase(p,
                            pObj->compInfoIndex(),
                            pObj->p1(),
                            pObj->size(),
                            pObj->dir(),
                            pObj->block(),
                            nMode,
                            prcClip,
                            nScale,nScaleMul);
    }

    QColor colText;
    switch(nMode & 0xffff) {
    case DRAW_ON:
        colText = QColor("#060035");
        break;
    case DRAW_MONO:
        colText = Qt::color1;
        break;
    case DRAW_OFF:
        colText = QColor("white");
        break;
    case DRAW_TEMP:
        colText = QColor("red");
        break;
    case DRAW_DARK:
        colText = QColor("orange");
        break;
    }
    SRect rc;
    SSize sz;
    if(!(nMode & DRAW_INH_PARTNAME)) {
        rc = pObj->nameArea();
        if(prcClip == NULL || rc.intersect(*prcClip)) {
            sz = g_drawComponentNumName(p,rc,nMode,!pObj->nameHorizontal(),colText,pObj->name(),nScale,nScaleMul);
            pObj->setNameDrawSize(sz);
        }
    }
    if(!(nMode & DRAW_INH_PARTNUM)) {
        rc = pObj->refnumArea();
        if(prcClip == NULL || rc.intersect(*prcClip)) {
            sz = g_drawComponentNumName(p,rc,nMode,!pObj->refnumHorizontal(),colText,pObj->refnum(),nScale,nScaleMul);
            pObj->setRefnumDrawSize(sz);
        }
    }
}

