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

#ifndef XBSCHGLOBAL_H
#define XBSCHGLOBAL_H

//#include "qstring.h"
#include "cfgdata.h"

class SCompIndex;
class SCompLib;

//////////////////////////////////////////////////////////////////////
// 大文字小文字を区別しない文字列比較
int strcmp_i(const char* s1,const char* s2);

//////////////////////////////////////////////////////////////////////
//コンフィギュレーションファイル関係
//extern SCfgData g_cfg;
extern string g_strCfgFileName;

//////////////////////////////////////////////////////////////////////
//ライブラリ関連

//コンフィギュレーションファイルからライブラリの情報を得て読み込む
bool g_ReadLibrary();

//ライブラリのメモリの解放を行う
void g_FreeLibrary();

//ライブラリから部品インデックスを得る
const SCompIndex* g_SearchComponentIndex(const char* pszName,int* pnLib,int* pnIndex,SCompLib* pLibOptional);

//ライブラリから論理反転部品のインデックスを得る
const SCompIndex* g_SearchLogicalInvertComponentIndex(const char* pszName);

int g_LibraryCount();
const SCompLib* g_LibraryInfo(int nLib); 

#endif
