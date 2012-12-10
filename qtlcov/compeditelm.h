/****************************************************************************
    Qt-LCoV Library Editor for Qt-BSch3V
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

#ifndef _COMPEDITELM_H
#define _COMPEDITELM_H

//#pragma once


class SCompEditElm
{
public:
    SCompEditElm(void);
    virtual ~SCompEditElm(void);

    int m_placeOpCount;            //配置された操作カウント
    int m_deleteOpCount;        //ここで示された操作カウントにおいて消去されている
    SCompEditElm* m_pOrigin;    //一時データにコピーしたときの元のデータ

    int m_dataType;
    void* m_pData;
};

#endif
