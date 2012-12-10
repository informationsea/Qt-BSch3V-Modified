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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <list>
using namespace std;
#include "ce3io.h"
//#include "csvio.h"
#include "cfgdata.h"


//#define CSVBUFFLEN 1024

SCfgData::SCfgData()
{
	//listCfgData.setAutoDelete(true);
}

SCfgData::~SCfgData()
{
	clear();
}

void SCfgData::clear()
{
	SCfgListIte ite = listCfgData.begin();
	while(ite != listCfgData.end()){
		SCfgDataElm* elm = *ite++;
		delete elm;
	}
	listCfgData.clear();
}

//既存のグループとキーを検索
SCfgDataElm* SCfgData::findExisting(const string& sGroup,const string& sKey)
//SCfgDataElm* SCfgData::findExisting(const char* pszgroup,const char* pszkey)
{
	SCfgListIte ite = listCfgData.begin();
	//既存のグループとキーのチェック
	while(ite != listCfgData.end()){
		SCfgDataElm* elm = *ite++;
		if(elm->group == sGroup && elm->key == sKey) return elm;
	}
	return NULL;
}


//設定データのロード
bool SCfgData::load(const char* filename)
{
	string str;
	string strGroup;
	string strKey;
	string strData;

	FILE* fp = fopen(filename,"r");
	if(fp==NULL) return false;
	SReadCE3 rce3;
	rce3.Attach(fp);

	while(rce3.ReadRecord(str)!=EOF){
		if(str[0]!='+') break;
		strGroup = str.substr(1);
		while(rce3.ReadRecord(str)!=EOF){
			if(str[0]=='-') break;
			int n;
			int l=str.length();
			for(n=0;n<l;n++){
				if(str[n]==':') break;
			}
			if(0<n && n<(l-1)){	// : で分けられたレコードである
				strKey = str.substr(0,n);	//先頭から:の手前まで
				strData =str.substr(n+1);	//:から終端まで

				SCfgDataElm* elm = new SCfgDataElm;
				elm->group	= strGroup;
				elm->key	= strKey;
				elm->data	= strData;
				//リストへのappend
				listCfgData.push_back(elm);
			}
		}
	}
	fclose(fp);
	return true;

}

//設定データのファイルへの保存
bool SCfgData::save(const char* filename)
{

	FILE* fp = fopen(filename,"w");
	if(fp==NULL) return false;

	SWriteCE3 wce3;
	wce3.Attach(fp);
	while(1){
		SCfgDataElm* elm = NULL;
		SCfgListIte ite = listCfgData.begin();
		while(ite != listCfgData.end()){
			elm = *ite++;
			//保存済みのものは groupが""になっている。
			if(elm->group!="" && elm->key!="" && elm->data!="") break;
			elm = NULL;
		}

		if(elm != NULL){
			string strTag = "+";
			string strGroup = elm->group;
			strTag += strGroup;
			if(wce3.WriteRecord(strTag)) return false;
			if(wce3.WriteEOL()) return false;
			while(elm != NULL){
				if(strGroup == elm->group){
					string strRecord;
					elm->group = "";
					strRecord = elm->key;
					strRecord += ":";
					strRecord += elm->data;
					if(wce3.WriteRecord(strRecord)) return false;
					if(wce3.WriteEOL()) return false;
				}
				if(ite != listCfgData.end()){
					elm = *ite++;
				}else{
					elm = NULL;
				}
			}
			strTag[0]='-';
			if(wce3.WriteRecord(strTag)) return false;
			if(wce3.WriteEOL()) return false;
		}else{
			break;
		}
	}
	fclose(fp);
	clear();				//いったんクリアして
	return load(filename);	//再読み込み
}

//groupとkeyを指定して、文字列dataを設定する
void SCfgData::setString(const string& sGroup,const string& sKey,const string& sData)
{

	if( sGroup=="" || sKey=="") return;
	//既存のグループとキーのチェック
	SCfgDataElm* elm = findExisting(sGroup,sKey);
	if(!elm){	//既存のグループとキーが見つからなかった場合
		//elmの新規作成と、group key data の格納
		elm = new SCfgDataElm;
		elm->group 	= sGroup;
		elm->key 	= sKey;
		elm->data 	= sData;
		listCfgData.push_back(elm);
	}else{
		elm->data = sData;
	}		
}

//groupとkeyを指定して、整数値dataを設定する
//整数値を文字列に変換してからsetStringを呼んでいる
void SCfgData::setInt(const string& sGroup,const string& sKey,int data)
{
	char szData[64];
	sprintf(szData,"%d",data);
	setString(sGroup,sKey,szData);
}

bool SCfgData::getString(const string& sGroup,const string& sKey,string& sData)
{
	if( sGroup=="" || sKey=="" ) return false;

	//既存のグループとキーのチェック
	SCfgDataElm* elm = findExisting(sGroup,sKey);
	if(!elm){	//既存のグループとキーが見つからなかった場合
		sData="";
	}else{
		sData = elm->data;
	}
	return sData!="";
}

bool SCfgData::getInt(const string& sGroup,const string& sKey,int& data)
{
	if( sGroup=="" || sKey=="" ) return false;

	//既存のグループとキーのチェック
	SCfgDataElm* elm = findExisting(sGroup,sKey);
	if(!elm){	//既存のグループとキーが見つからなかった場合
		data=0;
		return false;
	}else{
		data=atoi(elm->data.c_str());
		return true;
	}
}
/*
//ダンプ
void SCfgData::dump()
{
	SCfgListIte ite = listCfgData.begin();
	//既存のグループとキーのチェック
	while(ite != listCfgData.end()){
	printf("START SCfgData::dump()**************\n");
	while(elm){
		SCfgDataElm* elm = *ite++;
		printf("%s,%s,%s\n",elm->group.c_str(),elm->key.c_str(),elm->data.c_str() );
	}
	printf("****************SCfgData::dump() END\n");
	return;
}
*/
