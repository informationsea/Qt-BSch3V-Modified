/****************************************************************************
    BSch3V and Utility common file.
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
#include <assert.h>
#include <limits.h>

#include "xbsch.h"
#include "xbschstringlist.h"
#include "ce3io.h"
#include "xbschobj.h"
#include "xbschline.h"
#include "xbschwirenl.h"


SXBSchWireNl::SXBSchWireNl(SXBSchWire& w)
{
	setP1(w.p1());
	setP2(w.p2());
}

SXBSchWireNl* SXBSchWireNl::divide(const SPoint& pt)
{
	SXBSchWireNl* pNewWire = NULL;
	if(pt.x() == m_p1.x() && pt.x() == m_p2.x()){
		if(pt.y() >= m_p1.y() && pt.y() >= m_p2.y()) return NULL;
		if(pt.y() <= m_p1.y() && pt.y() <= m_p2.y()) return NULL;
		pNewWire = new SXBSchWireNl(*this);
	}else if(pt.y() == m_p1.y() && pt.y() == m_p2.y()){
		if(pt.x() >= m_p1.x() && pt.x() >= m_p2.x()) return NULL;
		if(pt.x() <= m_p1.x() && pt.x() <= m_p2.x()) return NULL;
		pNewWire = new SXBSchWireNl(*this);
	}
	if(pNewWire != NULL){
		m_p2=pt;
		pNewWire->setP1(pt);
	}
	return pNewWire;
}
