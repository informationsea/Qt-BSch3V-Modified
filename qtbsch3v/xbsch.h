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

//////////////////////////////////////////
// xbschのための定義ファイル
//////////////////////////////////////////

#ifndef XBSCH_H
#define XBSCH_H

#define MAX_LIB 16

//下位16ビットが描画モード
//上位16ビットがオプションフラグ
enum DRAW_MODE {
    DRAW_ON           = 1,
    DRAW_OFF          = 2,
    //DRAW_XOR        = 3,
    DRAW_XORINVERT    = 4,
    DRAW_DARK         = 5,
    DRAW_MONO         = 6,
    DRAW_TEMP         = 7,
    DRAW_SINGLE       = 0x010000, //単一描画
    DRAW_INH_PARTNAME = 0x020000,
    DRAW_INH_PARTNUM  = 0x040000,
    DRAW_INH_PARTBODY = 0x080000,
    DRAW_FOR_PRINT    = 0x100000 //印刷用の描画
};

#define PIN_LENGTH 10

#endif


/*
  Local Variables:
  mode: c++
  End:
 */
