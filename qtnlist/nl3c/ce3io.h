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
//CE3ファイルの処理

//カンマまたは改行区切り
//コントロール文字とスペース、カンマ、% は %xx(xxはアスキーコード)に置き換える。


#ifndef CE3IO_H
#define CE3IO_H

#include <stdio.h>
#include <string>
using namespace std;

class SReadCE3{
protected:
	const char*	m_buff;
	const char*	m_pread;
	FILE*	m_pfile;
	int		m_lineCount;	 
	int  getC();
	bool isSpace(char cc);
	bool isDelim(char cc);
public:
	SReadCE3();		//コンストラクタ
	~SReadCE3();	//デストラクタ
	bool Attach(FILE* pFile);
	bool Attach(const char* buff);
	int ReadRecord(string& s);
	int ReadRecord(char* pBuff,int nBuffLen);
	int Line(){return m_lineCount;}
	int SkipTo(char* pszSkipTo);
	int SkipTo(const string& strSkipTo);
};

class SWriteCE3{
protected:
	FILE*	m_pfile;
	string*	m_pstr;
	bool	m_bNewLine; 
	int  putC(int c);

public:
	SWriteCE3();	//コンストラクタ
	~SWriteCE3();	//デストラクタ
	bool Attach(FILE* pFile);
	bool Attach(string* pstr);
	int WriteRecord(const char* pBuff);
	int WriteRecord(const string& s);
	int WriteRecordInt(const string& var,int val);
	int WriteRecordString(const string& var,const string& val);
	int WriteEOL();
};

#endif
