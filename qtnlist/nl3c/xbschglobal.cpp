/****************************************************************************
    QtNlist   Netlist & Partslist utility for Qt-BSch3V
    Copyright (C) 2004-2005 H.Okada

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
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <list>
#include <string>
using namespace std;

//#include <qstring.h>
//#include <qcstring.h>

#include "xbsch.h"
#include "coord.h"
#include "xbschglobal.h"
#include "complib.h"
#include "cfgdata.h"


//////////////////////////////////////////////////////////////////////
// 大文字小文字を区別しない文字列比較
// Visual C++ には stricmpがあるが標準のC++には無いようなのでここで定義
int strcmp_i(const char* s1,const char* s2)
{
	int c1;
	int c2;

	while(1){
		if(*s1=='\0' && *s2=='\0') return 0; 
		c1 = toupper(*s1++);
		c2 = toupper(*s2++);
		if(c1 != c2) return c1-c2;
	}
}



//////////////////////////////////////////////////////////////////////
//コンフィギュレーションファイル関係
SCfgData g_cfg;
string g_strCfgFileName;	//コンフィギュレーションファイル名

//////////////////////////////////////////////////////////////////////
//ライブラリ関連
static SCompLib* g_pCompLib[MAX_LIB];	//部品ライブラリ
static int s_libraryCount=0;

//コンフィギュレーションファイルからライブラリの情報を得て読み込む
bool g_ReadLibrary()
{
	string filename;
	int n;
	char szKey[32];

	for(n=0;n<MAX_LIB;n++) g_pCompLib[n]=NULL;
	for(n=0;n<MAX_LIB;n++){
		sprintf(szKey,"LIB%d",n);
		if(g_cfg.getString("Library",szKey,filename)){
			g_pCompLib[n]=new SCompLib;
			if(!g_pCompLib[n]->readLibraryFile(filename.c_str())){
				delete g_pCompLib[n];
				g_pCompLib[n]=NULL;
				return false;
			}
		}else{
			break;
		}
	}
	s_libraryCount = n;
	return true;
}

//ライブラリのメモリの解放を行う
void g_FreeLibrary()
{
	int n;
	for(n=0;n<s_libraryCount;n++){
		delete g_pCompLib[n];
		g_pCompLib[n] = NULL;
	}
	while(n<MAX_LIB) g_pCompLib[n++]=NULL;
	s_libraryCount=0;
}

//ライブラリから部品インデックスを得る
const SCompIndex* g_SearchComponentIndex(const char* pszName,int *pnLib,int *pnIndex,SCompLib* pLibOptional)
{
	int i,n;
	int nCount;
	const SCompIndex* pCompIndex;
	for(i=0;i<=s_libraryCount;i++){
		SCompLib* pLib = NULL;
		if(i<s_libraryCount){
			pLib = g_pCompLib[i];
		}else{
			pLib=pLibOptional;
		}
		if(pLib==NULL) return NULL;
		nCount=pLib->count();	//ライブラリに登録されている部品数
		for(n=0;n<nCount;n++){
			pCompIndex=pLib->compIndex(n);
			if(!strcmp_i(pszName,pCompIndex->name())){
				if(pnLib)  *pnLib=i;
				if(pnIndex) *pnIndex=n;
				return pCompIndex;
			}
		}
	}
	return NULL;
}

//ライブラリから論理反転部品のインデックスを得る
const SCompIndex* g_SearchLogicalInvertComponentIndex(const char* pszName)
{
	if(pszName==NULL)return NULL;

	//元の部品のライブラリ番号とインデックス情報の取得
	int nCompLibNum;
	const SCompIndex* pIndex		= g_SearchComponentIndex(pszName,&nCompLibNum,NULL,NULL);
	if(pIndex == NULL) return NULL;

	string strInvertName=pszName;
	int n=strInvertName.length();
	if(n>1 && (strInvertName[n-1]=='i' || strInvertName[n-1]=='I')){
		//末尾がiなら末尾を消去
		strInvertName=strInvertName.substr(0,n-1);
	}else{
		//末尾がiでなければiを追加
		strInvertName=strInvertName+'i';
	}

	int nInvCompLibNum;
	const SCompIndex* pInvIndex	= g_SearchComponentIndex(strInvertName.c_str(),&nInvCompLibNum,NULL,NULL);
	if(pInvIndex == NULL || nInvCompLibNum != nCompLibNum) return NULL;


	//その他適合条件のチェック
	//ピン数
	if(pIndex->pinCount() != pInvIndex->pinCount()) return NULL;

	//ブロック数
	if(pIndex->block() != pInvIndex->block()) return NULL;

	//部品サイズ
	if(pIndex->size() != pInvIndex->size()) return NULL;	

	return pInvIndex;
}

//管理しているライブラリの数を返す
int g_LibraryCount(){return s_libraryCount;}

//ライブラリへのポインタを返す
const SCompLib* g_LibraryInfo(int nLib){
	if(nLib>=0 && nLib<s_libraryCount) return g_pCompLib[nLib];
	else return NULL;
}
