/****************************************************************************
    BSch3V schematic capture
    Copyright (C) 1997-2005 H.Okada

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

#ifndef COMPLIB_H
#define COMPLIB_H

#include "coord.h"

#include "SPtnObj.h"
#include "SPtnObjArc.h"
#include "SPtnObjCircle.h"
#include "SPtnObjLine.h"
#include "SPtnObjPolygon.h"
#include "SPtnObjText.h"


class SReadCE3;
class SWriteCE3;

#ifdef _BSCH
	extern bool g_notSaveBitPtn;
#endif

#define PIN_LENGTH 10


////////////////////////////////////////////////////////////////////
//部品ライブラリに関係する定義


enum PIN_TYPE
{
	PIN_TYPE_NOMAL		=0x00,
	PIN_TYPE_SMALL		=0x01,
	PIN_TYPE_ZLENG		=0x02,
	PIN_TYPE_CLOCK		=0x04,
	PIN_TYPE_NEGATIVE	=0x08,
	PIN_TYPE_NUMHIDE	=0x10
};

enum PIN_LOC
{
	PIN_LOC_L			=0x0,
	PIN_LOC_T			=0x1,
	PIN_LOC_R			=0x2,
	PIN_LOC_B			=0x3
};
/*
enum VECTPTN
{
	VECTPTN_L			=1,
	VECTPTN_C			=2,
	VECTPTN_PG			=3,
	VECTPTN_AR			=4
};
*/



typedef SPtnObj* PSPtnObj;
typedef list<SPtnObj*> SPtnObjList;
typedef list<SPtnObj*>::iterator SPtnObjListIterator;

//LB3ライブラリの読み込みで使うパターンクラス
class SPtn
{
protected:
	unsigned char* readBmp(SReadCE3& rce3);//,int x,int y,int nBuffSize);
	//bool readLine(SReadCE3& rce3,int* vBuff,int vBuffCount,int* count);
	//bool readCircle(SReadCE3& rce3,int* vBuff,int vBuffCount,int* count);
	//bool readCircleFill(SReadCE3& rce3,int* vBuff,int vBuffCount,int* count);
	//bool readArc(SReadCE3& rce3,int* vBuff,int vBuffCount,int* count);
	//bool readPolygon(SReadCE3& rce3,int* vBuff,int vBuffCount,int* count);

	//bool writeLine(SWriteCE3& wce3,int* buff,int* count);
	//bool writeCircle(SWriteCE3& wce3,int* buff,int* count);
	//bool writeCircleFill(SWriteCE3& wce3,int* buff,int* count);
	//bool writeArc(SWriteCE3& wce3,int* buff,int* count);
	//bool writePolygon(SWriteCE3& wce3,int* buff,int* count);


public:
	char*			m_pszName;		//パターン名
	SSize			m_size;			//サイズ
	int				m_nBuffsize;	//バッファのサイズ
	PSPtnObj*		m_ppvPtn;		//ベクトルパターンオブジェクトのポインタ配列
	int				m_vPtnCount;
	unsigned char*	m_pBuff;		//バッファへのポインタ
//	int*			m_pvBuff;		//ベクトル形式へのポインタ
//	int				m_nvBuffsize;	//ベクトル形式のバッファのサイズ

	SPtn(){							//コンストラクタ
		m_pszName=NULL;
		m_nBuffsize=0;
		m_pBuff=NULL;
//		m_nvBuffsize=0;
//		m_pvBuff=NULL;
		m_size=SSize(0,0);
		m_vPtnCount=0;
		m_ppvPtn=NULL;
	}

	SPtn(const SPtn&);				//コピーコンストラクタ
	SPtn& operator=(const SPtn&);	//代入演算子	

	virtual ~SPtn();				//デストラクタ
	bool readCe3(SReadCE3& rce3);
	bool writeCe3(SWriteCE3& wce3,const char* pszName);
	void setSize(const SSize& sizePixel);
};



////////////////////////////////////////////////////////////////////
//ピンクラスの定義
class SPin
{
protected:
	int			m_nBlock;	//m_nBlockの数だけピン番号文字列配列を管理している
	char**		m_ppszNum;	//ピン番号文字列配列へのポインタ
	char*		m_pszName;	//ピン名へのポインタ
	int			m_nType;	//ピンのタイプ
	int			m_nLTRB;	//位置 LTRB
	int			m_nOffset;	//位置 オフセット
protected:
	void deleteBuff();

public:
	SPin();					//コンストラクタ
	~SPin();
	SPin(const SPin&);				//コピーコンストラクタ
	SPin& operator=(const SPin&);	//代入演算子

	
	void setBlock(int  nBlock);	//ブロック数をセットする。
	void setPinName(const char* name);				//ピン名をセットする。
	void setPinNum(int nBlock,const char* name);	//ピン番号をセットする。
	void setType(int type){ m_nType=type;}
	void setLtrb(int ltrb){ m_nLTRB=ltrb;}
	void setOffset(int offset){m_nOffset=offset;}
	const char* pinName() const {return m_pszName;}
	const char* pinNum(int nBlock) const;
	int type() const {return m_nType;}
	int ltrb() const {return m_nLTRB;}
	int offset() const {return m_nOffset;}
	int block() const { return m_nBlock;}

	bool readCe3(SReadCE3& rce3,int block);
	bool writeCe3(SWriteCE3& rce3);
};

typedef SPin* PSPin;

typedef list<SPin*> SPinList;
typedef list<SPin*>::iterator SPinListIterator;


////////////////////////////////////////////////////////////////////
//部品情報クラスの定義
class SCompInfo
{
protected:
	void deleteBuff();

public:
	char*	m_pszRef;	//デフォルト部品番号
	SSize	m_size;		//部品のサイズ
	int		m_nBlock;	//パッケージあたりのブロック数
	SPtn*	m_pPtn;		//LB3から読み込んだパターンへのポインタ
	int		m_nPinCount;//ピン数
	PSPin*	m_ppPin;	//LB3から読み込んだピンクラスの配列へのポインタ
	string	m_note;		//ノート 2005/04/30
	SCompInfo(){		//コンストラクタ
		m_pszRef	=NULL;
		m_nBlock	=1;
		m_pPtn		=NULL;
		m_nPinCount	=0;
		m_ppPin		=NULL;
		m_note		="";
	}
	~SCompInfo();			//デストラクタ

	SCompInfo(const SCompInfo&);			//コピーコンストラクタ
	SCompInfo& operator=(const SCompInfo&);	//代入演算子

	bool readCe3(SReadCE3& rce3,string& name,string& ptnname);	//ファイルからの読み込み
	bool writeCe3(SWriteCE3& wce3,const char* pszName);

	void setRef(const char* ref);		//デフォルト部品番号の設定
	void setSize(const SSize& size,bool createPtn);	//サイズの設定 パターンを有する場合はパターンのサイズも変更
	void setUsePattern(bool usepattern);
	void setBlock(int block);
	void addPin(SPin* ppin);
	void removePin(int ltrb,int offset,bool move);
	void insertNullPin(int ltrb,int offset);
	SPin* pinInfo(int ltrb,int offset);	//指定した位置のピン情報を得る
};

////////////////////////////////////////////////////////////////////
//部品インデックスクラスの定義
class SCompIndex
{
	char*				m_lpszName;	//部品名
public:
	SCompInfo*			m_pCompInfo;//対応する部品情報クラスへのポインタ

	SCompIndex(){			//コンストラクタ
		m_lpszName	=NULL;
		m_pCompInfo =NULL;
	}
	~SCompIndex(){			//デストラクタ
		delete[]m_lpszName;
	}
	void setName(const char* name);
	void setCompInfo(SCompInfo* pcinfo){m_pCompInfo = pcinfo;}

	SCompInfo* compInfo() const{return m_pCompInfo;}
	const char*	name() const{return m_lpszName;}			//部品名を得る
	const char* number() const;			//部品番号を得る
	SSize size() const;					//サイズを得る
	SSize sizePlace(int dir) const;		//配置時の図面上のサイズを得る
	int	  block() const;				//パッケージあたりのブロック数を得る
	SPtn*	pattern() const;			//パターンへのポインタを得る
	int	pinCount() const;				//ピンの数を得る
	SPin* pin(int index) const;			//ピンの情報を得る
	//ピンの情報を得る
	//向きの情報から、ピンの位置情報(LTRBと、原点からのオフセット)を返す
	//SPin* SCompIndex::pin(int index,int dir,int& nLtrb,SPoint& ptEnd) const;
	SPin* pin(int index,int dir,int& nLtrb,SPoint& ptEnd) const;

};



typedef SCompIndex* PSCompIndex;

typedef list<SPtn*> SPtnList;
typedef list<SPtn*>::iterator SPtnListIterator;

typedef list<SCompInfo*> SCompInfoList;
typedef list<SCompInfo*>::iterator SCompInfoListIterator;

typedef list<SCompIndex*> SCompIndexList;
typedef list<SCompIndex*>::iterator SCompIndexListIterator;


////////////////////////////////////////////////////////////////////
//部品ライブラリクラス
class SCompLib
{
public:
	char*			m_lpszLibraryName;		//ライブラリ名
	string			m_prop_string;
	SPtnList		m_listPtn;				//読み込んだパターンのリスト
	SCompInfoList	m_listCompInfo;			//読み込み込んだ部品クラスのリスト
	SCompIndexList	m_listCompIndex;		//読み込み込んだ部品インデックスのリスト
	PSCompIndex*	m_ppCompIndex;			//読み込み込んだ部品インデックスのポインタ配列
	int				m_aux1;
	int				m_aux2;


public:
	SCompLib(){										//コンストラクタ
		m_lpszLibraryName=NULL;
		m_ppCompIndex=NULL;
		m_prop_string="";
		m_aux1 = 0;
		m_aux2 = 0;
		//m_nCount=0;
	}
	~SCompLib();										//デストラクタ
	bool ReadLb3(SReadCE3& rce3,bool singleComponent = false);//LB3ライブラリの読み込み
	void deleteContent();//内容の破棄

	bool writeLibraryFile(const char* lpszFileName);	//ファイル書き込み
	bool readLibraryFile(const char* lpszFileName);		//ファイル読み込み

	void setLibraryName(const char* name);				//ライブラリ名の設定
	const char* libraryName() const ;					//ライブラリ名を得る
	int count() const{return m_listCompIndex.size();}	//管理部品数を得る
	SCompIndex* compIndex(int nIndexNum) const;			//n番目の部品のインデックスを得る

	static bool WriteAlias(SWriteCE3& wce3,const char*  alias,const char*  name);
	string uniqueName(const string& name);	//既存の部品名と重ならない名前を作る

private:
	bool ReadLb3Ptn(SReadCE3& rce3);//パターンの読み込み
	bool ReadLb3Comp(SReadCE3& rce3);
	bool ReadLb3Alias(SReadCE3& rce3);

	void clearPtnList();	//メンバー変数パターンリストのクリア


	bool isUniqueName(const string& name);	//既存の部品名と重複していないかどうか

	//bool WriteLb3Pin(SWriteCE3& wce3,const SPin& pin);							//ピンの書き込み
	//bool WriteLb3Ptn(SWriteCE3& wce3,const SPtn& ptn,const char*  name);			//パターンの書き込み
	//bool WriteLb3Comp(SWriteCE3& wce3,const SCompInfo& compinfo,SWriteCE3& wce3);	//部品の書き込み
	
};


#endif
