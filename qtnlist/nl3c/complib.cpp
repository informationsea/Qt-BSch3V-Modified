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

//部品ライブラリに関係する３つのクラスのインプリメント
#include "stdafx.h"
#include <string.h>
#include <assert.h>
#include <math.h>
#include <list>
#include <string>
#include <stdlib.h>
using namespace std;

//#include <qstring.h>
#include <ctype.h>
#include "coord.h"
#include "ce3io.h"
#include "complib.h"
//#include "xbsch.h"
#ifdef _WINDOWS
	#include <io.h>
#endif


#ifdef _BSCH
	bool g_notSaveBitPtn=false;
#endif

typedef char* PCSTRING;
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

//#define NOT_BSCH3V		//BSch3V以外では、これを定義しておくと、読み込みが速くなる。

////////////////////////////////////////////////
// パターンクラス

SPtn::~SPtn(){				//デストラクタ
		delete[]m_pszName;
		delete[]m_pBuff;
		//delete[]m_pvBuff;
		for(int n=0;n<m_vPtnCount;n++){
			SPtnObj* pObj = m_ppvPtn[n];
			delete pObj;
		}
		delete[]m_ppvPtn;
	}


//コピーコンストラクタ
SPtn::SPtn(const SPtn& ptn)
{
	m_pszName=NULL;
	m_nBuffsize=0;
	m_pBuff=NULL;
	m_size=SSize(0,0);
	m_vPtnCount=0;
	m_ppvPtn=NULL;

	*this = ptn;
}

//代入演算子	
SPtn& SPtn::operator=(const SPtn& ptn)
{
	if(this != &ptn){
		delete[]m_pszName;
		delete[]m_pBuff;
		if(ptn.m_pszName != NULL){
			m_pszName = new char[strlen(ptn.m_pszName)+1];
			strcpy(m_pszName,(ptn.m_pszName));
		}else{
			m_pszName=NULL;
		}

		m_size = ptn.m_size;
		if(ptn.m_pBuff != NULL){
			m_pBuff = (unsigned char*)new char[m_nBuffsize = ptn.m_nBuffsize];
			for(int i=0;i<m_nBuffsize;i++) m_pBuff[i]=ptn.m_pBuff[i];
		}else{
			m_nBuffsize=0;
			m_pBuff=NULL;
		}


		for(int n=0;n<m_vPtnCount;n++){
			SPtnObj* pObj = m_ppvPtn[n];
			delete pObj;
		}
		delete[]m_ppvPtn;
		m_ppvPtn = NULL;
		m_vPtnCount = ptn.m_vPtnCount;
		if(m_vPtnCount>0){
			m_ppvPtn = new PSPtnObj[m_vPtnCount];
			for(int n=0;n<m_vPtnCount;n++){
				m_ppvPtn[n] =  ptn.m_ppvPtn[n]->duplicate();
			}
		}
	}
	return *this;
}

bool SPtn::writeCe3(SWriteCE3& wce3,const char* pszName)
{
	int x = m_size.w();
	int y = m_size.h();
	wce3.WriteRecord("+PTN");
	if(pszName != NULL){
		wce3.WriteRecordString("N",pszName);
	}else{
		wce3.WriteRecordString("N",m_pszName);
	}
	wce3.WriteRecordInt("X",x);
	wce3.WriteRecordInt("Y",y);
	wce3.WriteEOL();
	bool bWriteBMP = (m_pBuff!=NULL);
#ifdef _BSCH
	if(g_notSaveBitPtn && (m_vPtnCount>0)){
		bWriteBMP = false;
	}
#endif
	if(bWriteBMP){
		wce3.WriteRecord("+BMP");	wce3.WriteEOL();

		char linebuff[128];
		int buffXByte = ((x+15)/16)*2;	//バッファの水平方向バイト数
		int xByte = (x+7)/8;			//書き込みの水平方向バイト数。2文字で1バイトなので読み込み文字はこれの倍
		int buffOffset;
		int i,j,k;
		for(i=0;i<y;i++){
			buffOffset = buffXByte*i;
			for(j=0,k=0;j<xByte;j++){
				if(buffOffset>=m_nBuffsize) break;
				linebuff[k++]=((m_pBuff[buffOffset]&0xf0)>>4)+'@';
				linebuff[k++]=(m_pBuff[buffOffset]&0xf)      +'@';
				buffOffset++;
			}
			linebuff[k]='\0';
			wce3.WriteRecord(linebuff); wce3.WriteEOL();
		}
		wce3.WriteRecord("-BMP");	wce3.WriteEOL();
	}

	for(int i=0;i<m_vPtnCount;i++){
		m_ppvPtn[i]->writeCe3(wce3);
	}
	wce3.WriteRecord("-PTN");	wce3.WriteEOL();
	return true;
}



//#define VECTPTNBUFF_SIZE 32768

bool SPtn::readCe3(SReadCE3& rce3)
{
	SPtnObjList listPtn;	//ベクトルパターンのリスト
	bool retv = true;
	

	string name("");
	int x=0;
	int y=0;

	m_nBuffsize=0;
	delete[]m_pBuff;
	m_pBuff=NULL;

	for(int n=0;n<m_vPtnCount;n++){
		SPtnObj* pObj = m_ppvPtn[n];
		delete pObj;
	}
	delete[]m_ppvPtn;



	m_pBuff = NULL;
	int vBuffCount=0;
	int n,i;

//	unsigned char* ptnBuff=NULL;
//	SPtn* pPtn=NULL;

	string str("");
	while(rce3.ReadRecord(str)!=EOF){
		if(str[0] == '-'){
			if( (str != "-PTN") || (m_pBuff==NULL && listPtn.size()==0)){
				retv = false;
			}
			break;
		}else if(str[0] == '+'){
			SPtnObj* pPtnObj=NULL;
			if(str=="+BMP" && m_size.w()>0 && m_size.h()>0 && name!="" && m_pBuff==NULL){
				m_nBuffsize=((m_size.w()+15)/16)*2*m_size.h();
				if(NULL==(m_pBuff=readBmp(rce3))) return false;
			}else if(str=="+L"){
				pPtnObj = new SPtnObjLine;
			}else if(str=="+C"){
				pPtnObj = new SPtnObjCircle;
			}else if(str=="+PG"){
				pPtnObj = new SPtnObjPolygon;
			}else if(str=="+AR"){
				pPtnObj = new SPtnObjArc;
			}else if(str=="+TX"){
				pPtnObj = new SPtnObjText;
			}else{
				str[0]='-';
				if(rce3.SkipTo(str) == EOF){
					retv= false;
					break;
				}
			}
			if(pPtnObj!=NULL){
				if(pPtnObj->readCe3(rce3)){
					listPtn.push_back(pPtnObj);
				}else{
					delete pPtnObj;
				}
			}
		}else{
			int n;
			int l=str.length();
			for(n=0;n<l;n++){
				if(str[n]==':') break;
			}
			if(0<n && n<(l-1)){	// : で分けられたレコードである
				string var = str.substr(0,n);				//先頭から:の手前まで
				//int nParam = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
				if(var=="X"){
					x = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
					m_size.setW(x);
				}else
				if(var=="Y"){
					y = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
					m_size.setH(y);
				}else
				if(var=="N"){
					name = str.substr(n+1).c_str();
					m_pszName = new char[name.length()+1];
					strcpy(m_pszName,name.c_str());
				}
			}
		}
	}
	if(listPtn.size()>0){
		m_vPtnCount = listPtn.size();
		m_ppvPtn = new PSPtnObj[m_vPtnCount];
		SPtnObjListIterator ite = listPtn.begin();
		int i=0;
		while(ite != listPtn.end()){
			m_ppvPtn[i] = (*ite);
			i++;
			ite++;
		}
	}
	return retv;
//RdErr:
//	delete[]ptnBuff;
//	return false;
}




void SPtn::setSize(const SSize& sizePixel)
{
	int i,j;
	int newX=sizePixel.w();
	int newY=sizePixel.h();
	static unsigned char rightMaskTable[]={0xFF,0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE};

	//新しいバッファの準備
	assert(newX>0 && newY>0);
	if(newX>501) newX=501;					//xもｙも50グリッドぶんを上限とする。
	if(newY>501) newY=501;					//
	int newBuffXByte = ((newX+15)/16)*2;	//バッファの水平方向バイト数
	int newBuffSize = newBuffXByte*newY;
	unsigned char* newBuff= (unsigned char*)new char[newBuffSize];
	for(i=0;i<newBuffSize;i++) newBuff[i]=0;
	
	if(m_pBuff!=NULL){
		//旧バッファ関連の変数設定
		int x=m_size.w();
		int y=m_size.h();
		int buffXByte = ((x+15)/16)*2;	//バッファの水平方向バイト数

		//コピーするバイト数と右端のマスク値の設定
		int xCopyPixel =(newX < x ? newX : x);
		int xCopyByte  =(xCopyPixel+7)/8;
		unsigned char rightMask = rightMaskTable[xCopyPixel%8];
		int yCopyLine  =(newY < y ? newY : y);
	
		unsigned char* srcLeft  = m_pBuff;
		unsigned char* destLeft = newBuff;
		for(i=0;i<yCopyLine;i++){
			for(j=0;j<xCopyByte;j++) destLeft[j]=srcLeft[j];
			destLeft[xCopyByte-1] &= rightMask;
			srcLeft  += buffXByte;
			destLeft += newBuffXByte;
		}
		delete[]m_pBuff;
		m_pBuff=NULL;
	}
	m_pBuff = newBuff;
	m_nBuffsize = newBuffSize;
	m_size.setW(newX);
	m_size.setH(newY);
}

////////////////////////////////////////////////
// ピンクラス

//コンストラクタ
SPin::SPin()
{
	m_nBlock	= 0;			//m_nBlockの数だけピン番号文字列配列を管理している
	m_ppszNum	= NULL;			//ピン番号文字列配列へのポインタ
	m_pszName	= NULL;			//ピン名へのポインタ
	m_nType		= 0;			//ピンのタイプ
	m_nLTRB		= PIN_LOC_L;	//位置 LTRB
	m_nOffset	= 0;			//位置 オフセット
}

//デストラクタ
SPin::~SPin()
{
	deleteBuff();
}

//バッファのクリア
void SPin::deleteBuff()
{
	delete[]m_pszName;
	m_pszName=NULL;
	int n;
	for(n=0;n<m_nBlock;n++){
		char* pc = m_ppszNum[n];
		delete[]pc;
	}
	delete[]m_ppszNum;
	m_ppszNum=NULL;
}



//コピーコンストラクタ
SPin::SPin(const SPin& pin)
{
	m_nBlock	= pin.m_nBlock;
	m_nType		= pin.m_nType;
	m_nLTRB		= pin.m_nLTRB;
	m_nOffset	= pin.m_nOffset;
	if(m_nBlock>0){
		m_ppszNum = new PCSTRING[m_nBlock];
		for(int i=0;i<m_nBlock;i++){
			if(pin.m_ppszNum[i]!=NULL){
				m_ppszNum[i]=new char[strlen(pin.m_ppszNum[i])+1];
				strcpy(m_ppszNum[i],pin.m_ppszNum[i]);
			}else{
				m_ppszNum[i]=NULL;
			}
		}
	}else{
		m_ppszNum = NULL;
	}
	if(pin.m_pszName!=NULL){
		m_pszName = new char[strlen(pin.m_pszName)+1];
		strcpy(m_pszName,pin.m_pszName);
	}else{
		m_pszName = NULL;
	}
}

bool SPin::readCe3(SReadCE3& rce3,int block)
{
	deleteBuff();

	setBlock (block);
	int nPinSetCount=0;

	string str("");
	while(rce3.ReadRecord(str)!=EOF){
		if(str[0] == '-'){
			if(str != "-PIN"){
				return false;
			}else{
				break;
			}
		}else if(str[0] == '+'){
			str[0]='-';
			if(rce3.SkipTo(str) == EOF) return false;
		}else{
			int n;
			int l=str.length();
			for(n=0;n<l;n++){
				if(str[n]==':') break;
			}
			if(0<n && n<l){	// : で分けられたレコードである
				string var = str.substr(0,n);				//先頭から:の手前まで
				n++;
				if(var=="N"){
					setPinName(str.substr(n).c_str());
				}else if(var=="L"){
					switch ((str.substr(n))[0]){
					case 'L': m_nLTRB = PIN_LOC_L; break;
					case 'T': m_nLTRB = PIN_LOC_T; break;
					case 'R': m_nLTRB = PIN_LOC_R; break;
					case 'B': m_nLTRB = PIN_LOC_B; break;
					}
					int nOffset = atoi(str.substr(n+1).c_str());
					if(nOffset>=0 && nOffset<=255){
						m_nOffset = nOffset;  
					}
				}else if(var=="T"){
					int nType=0;
					int i=0;
					int len=str.substr(n).length();
					for(i=0;i<len;i++){
						switch ((str.substr(n))[i]){
						case 'Z': nType |= PIN_TYPE_ZLENG;		break;
						case 'S': nType |= PIN_TYPE_SMALL;		break;
						case 'N': nType |= PIN_TYPE_NEGATIVE;	break;
						case 'C': nType |= PIN_TYPE_CLOCK;		break;
						case 'm': nType |= PIN_TYPE_NUMHIDE;	break;
						}
					}
					m_nType = nType;
				}else if(var=="M"){
					if(nPinSetCount<block){
						setPinNum(nPinSetCount,str.substr(n).c_str());
						nPinSetCount++;
					}
				}
			}	
		}
	}
	return true;
}

//ピンの書き込み
//+PIN,N:\P\R,L:T3,T:N,M:4,M:10,-PIN
bool SPin::writeCe3(SWriteCE3& wce3)
{

	string name="";
	const char* rawpinname = pinName();
	while(*rawpinname){
		if(*rawpinname & 0x80){
			name += '\\';
		}
		name += *rawpinname & 0x7F;
		rawpinname++;
	}

	char szLocation[32];
	switch (m_nLTRB){
	case PIN_LOC_L: szLocation[0] = 'L'; break;
	case PIN_LOC_T: szLocation[0] = 'T'; break;
	case PIN_LOC_R: szLocation[0] = 'R'; break;
	case PIN_LOC_B: 
	default:		szLocation[0] = 'B'; break;
	}
	sprintf(szLocation+1,"%d",m_nOffset);

	char szType[32];
	char* pc=szType;
	if(m_nType & PIN_TYPE_ZLENG)	*pc++ = 'Z';
	if(m_nType & PIN_TYPE_SMALL)	*pc++ = 'S';		//	case 'S': nType |= PIN_TYPE_SMALL;		break;
	if(m_nType & PIN_TYPE_NEGATIVE)	*pc++ = 'N';		//	case 'N': nType |= PIN_TYPE_NEGATIVE;	break;
	if(m_nType & PIN_TYPE_CLOCK)	*pc++ = 'C';		//	case 'C': nType |= PIN_TYPE_CLOCK;		break;
	if(m_nType & PIN_TYPE_NUMHIDE)	*pc++ = 'm';		//	case 'm': nType |= PIN_TYPE_NUMHIDE;	break;
	*pc='\0';


	wce3.WriteRecord("+PIN");
	wce3.WriteRecordString("N",name);
	wce3.WriteRecordString("L",szLocation);
	wce3.WriteRecordString("T",szType);
	for(int i=0;i<m_nBlock;i++){
		const char* cp = pinNum(i);
		if(cp==NULL){
			wce3.WriteRecordString("M","");
		}else{
			wce3.WriteRecordString("M",cp);
		}
	}
	wce3.WriteRecord("-PIN");
	return true;
}


//代入演算子	
SPin& SPin::operator=(const SPin& pin)
{
	if(this != &pin){
		deleteBuff();
		m_nBlock	= pin.m_nBlock;
		m_nType		= pin.m_nType;
		m_nLTRB		= pin.m_nLTRB;
		m_nOffset	= pin.m_nOffset;
		if(m_nBlock>0){
			m_ppszNum = new PCSTRING[m_nBlock];
			for(int i=0;i<m_nBlock;i++){
				if(pin.m_ppszNum[i]!=NULL){
					m_ppszNum[i]=new char[strlen(pin.m_ppszNum[i])+1];
					strcpy(m_ppszNum[i],pin.m_ppszNum[i]);
				}else{
					m_ppszNum[i]=NULL;
				}
			}
		}
		if(pin.m_pszName!=NULL){
			m_pszName = new char[strlen(pin.m_pszName)+1];
			strcpy(m_pszName,pin.m_pszName);
		}
	}
	return *this;
}


//ブロック数をセットする。
void SPin::setBlock(int nBlock)
{
	int n;
	char**	ppszNum = new PCSTRING[nBlock];
	for(n=0;n<nBlock;n++) ppszNum[n]=NULL;
	int m = (nBlock < m_nBlock ? nBlock : m_nBlock);
	for(n=0;n<m;n++) ppszNum[n]=m_ppszNum[n];
	for(n=nBlock;n<m_nBlock;n++){	//旧ブロックサイズの方が大きい場合は余った文字列を解放
		char* pc = m_ppszNum[n];
		delete[]pc;
	}
	delete[]m_ppszNum;
	m_ppszNum = ppszNum;
	m_nBlock = nBlock;

}

//ピン名をセットする。
void SPin::setPinName(const char* name)
{
	delete[]m_pszName;
	if(name== NULL){
		m_pszName=NULL;
		return;
	}
	char buff[256];
	int n=0;
	int m=0;

	int len = strlen(name);
	
	while(n<len && m<255){
		if(name[n]=='\\'){
			if(name[n+1]){
				buff[m]=name[n+1] | 0x80;
				n++;
			}
		}else{
			buff[m]=name[n];
		}
		n++;m++;
	}
	buff[m]='\0';
	m_pszName = new char[m+1];
	strcpy(m_pszName,buff);
}

//ピン番号をセットする。
void SPin::setPinNum(int nBlock,const char* num)
{
	if(m_nBlock==0) return;
	if(nBlock<0 || nBlock>=m_nBlock ) return;
	char* pc =m_ppszNum[nBlock];
	delete[]pc;
	if(num== NULL){
		m_ppszNum[nBlock]=NULL;
		return;
	}
	int len = strlen(num);
	m_ppszNum[nBlock] = new char[len+1];
	strcpy(m_ppszNum[nBlock],num);
}

const char* SPin::pinNum(int nBlock) const
{
	if(nBlock<0 || nBlock>=m_nBlock) return NULL;
	return m_ppszNum[nBlock];
}

////////////////////////////////////////////////////////////////////////////
//SCompInfo のインプリメント

//コピーコンストラクタ
SCompInfo::SCompInfo(const SCompInfo& info)
{
	if(info.m_pszRef != NULL){
		m_pszRef = new char[strlen(info.m_pszRef)+1];	//デフォルト部品番号
		strcpy(m_pszRef,info.m_pszRef);
	}else{
		m_pszRef = NULL;
	}
	m_size = info.m_size;						//部品のサイズ
	m_nBlock = info.m_nBlock;					//パッケージあたりのブロック数
	if(info.m_pPtn != NULL){
		m_pPtn = new SPtn(*(info.m_pPtn));		//LB3から読み込んだパターンへのポインタ
	}else{
		m_pPtn =NULL;
	}
	m_nPinCount = info.m_nPinCount;			//ピン数
	if(m_nPinCount>0){
		m_ppPin = new PSPin[m_nPinCount];	//LB3から読み込んだピンクラスの配列へのポインタ
		for(int i=0;i<m_nPinCount;i++){
			m_ppPin[i]=new SPin(*(info.m_ppPin[i]));
		}
	}else{
		m_ppPin = NULL;
	}
	m_note = info.m_note;
}

//代入演算子
SCompInfo& SCompInfo::operator=(const SCompInfo& info)
{
	if(this != &info){
		deleteBuff();
		if(info.m_pszRef != NULL){
			m_pszRef = new char[strlen(info.m_pszRef)+1];	//デフォルト部品番号
			strcpy(m_pszRef,info.m_pszRef);
		}
		m_size = info.m_size;						//部品のサイズ
		m_nBlock = info.m_nBlock;					//パッケージあたりのブロック数
		if(info.m_pPtn != NULL){
			m_pPtn = new SPtn(*(info.m_pPtn));		//LB3から読み込んだパターンへのポインタ
		}
		m_nPinCount = info.m_nPinCount;			//ピン数
		if(m_nPinCount>0){
			m_ppPin = new PSPin[m_nPinCount];	//LB3から読み込んだピンクラスの配列へのポインタ
			for(int i=0;i<m_nPinCount;i++){
				m_ppPin[i]=new SPin(*(info.m_ppPin[i]));
			}
		}
		m_note = info.m_note;
	}
	return *this;
}


void SCompInfo::deleteBuff()
{
	delete[]m_pszRef;
	m_pszRef = NULL;
	delete m_pPtn;
	m_pPtn = NULL;
	if(m_ppPin){
		int n;
		for(n=0;n<m_nPinCount;n++){
			SPin* pPin = m_ppPin[n];
			delete pPin;
		}
		delete[]m_ppPin;
	}
}

//デストラクタ
SCompInfo::~SCompInfo()
{
	deleteBuff();
}

//ファイルからの読み込み
bool SCompInfo::readCe3(SReadCE3& rce3,string& name,string& ptn)
{
	deleteBuff();

	string ref("U");
	name = "";
	ptn = "";
	int x=0;
	int y=0;
	int b=0;

	//SCompInfo* pComp = new SCompInfo;
	//SPin* pPin;
	//SPtn* pPtn;
	//SCompIndex* pIndex;

	SPinList listPin;	//ピンのリスト
	

	string str("");
	while(rce3.ReadRecord(str)!=EOF){
		if(str[0] == '-'){
			if(str != "-COMP" || name=="" || x<=0 || y<=0 || b<=0){
				goto RdErr;
			}else{
				break;
			}
		}else if(str[0] == '+'){
			if(str=="+PIN"){
				if(x<=0 || y<=0 || b<=0) goto RdErr;
				SPin* pPin = new SPin;
				if(!pPin->readCe3(rce3,b)){
					delete pPin;
					goto RdErr;
				}
				listPin.push_back(pPin);
			}else{
				str[0]='-';
				if(rce3.SkipTo(str) == EOF) goto RdErr;
			}
		}else{
			int n;
			int l=str.length();
			for(n=0;n<l;n++){
				if(str[n]==':') break;
			}
			if(0<n && n<l){	// : で分けられたレコードである
				string var = str.substr(0,n);				//先頭から:の手前まで
				n++;
				if(var=="N"){
					name=str.substr(n);
				}else if(var=="X"){
					x=atoi(str.substr(n).c_str());
					if(x<0) goto RdErr;
				}else if(var=="Y"){
					y=atoi(str.substr(n).c_str());
					if(y<0) goto RdErr;
				}else if(var=="B"){
					if(b!=0) goto RdErr;
					b=atoi(str.substr(n).c_str());
					if(b<0 || b>256) goto RdErr;
				}else if(var=="R"){
					ref=str.substr(n);
				}else if(var=="P"){
					ptn=str.substr(n);
				}else if(var=="NOTE"){
					m_note=str.substr(n);
				}
			}
		}
	}

//	if(ptn!=""){	//パターンが設定ずみならパターンへのポインタを検索する。
//		SPtnListIterator ite = m_listPtn.begin();
//		while(ite != m_listPtn.end()){
//			pPtn = *ite;
//			if(pPtn->m_pszName == ptn){
//				//pComp->m_pPtn = pPtn;
//				pComp->m_pPtn = new SPtn(*pPtn);	//複数の部品定義でパターンを共有しない
//				break;
//			}
//			ite++;
//		}
//	}
	m_size.setW(x);
	m_size.setH(y);
	m_nBlock  = b;
	m_pszRef = new char[ref.length()+1];
	strcpy(m_pszRef,ref.c_str());
	m_nPinCount = listPin.size();
	if(m_nPinCount>0){
		m_ppPin=new PSPin[m_nPinCount];
		int n;
		for(n=0;n<m_nPinCount;n++){
			m_ppPin[n]=NULL;
		}
		n=0;
		list<SPin*>::iterator ite = listPin.begin();
		while(ite != listPin.end() && n<m_nPinCount){ 
			SPin* pPin = *ite;
			m_ppPin[n++] = pPin;
			ite++;
		}
	}

//	pIndex = new SCompIndex;
//	pIndex->setName(name.c_str());
//	pIndex->setCompInfo(pComp);
//	m_listCompIndex.push_back(pIndex);
//	m_listCompInfo.push_back(pComp);

	return true;
RdErr:
	SPinListIterator ite = listPin.begin();
	while(ite != listPin.end()){ 
		SPin* pPin = *ite;
		delete pPin;
		ite++;
	}
	return false;
}


bool SCompInfo::writeCe3(SWriteCE3& wce3,const char* pszName)
{
	if(pszName == NULL || *pszName == '\0') return false;
	if(m_pPtn != NULL){
		if(!m_pPtn->writeCe3(wce3,pszName)) return false;//パターンの書き込みの際は共有を前提としない。
		//wce3.WriteEOL();
	}
	int x = m_size.w();
	int y = m_size.h();
	int b = m_nBlock;
	wce3.WriteRecord("+COMP");
	wce3.WriteRecordString("N",pszName);
	wce3.WriteEOL();

	wce3.WriteRecordInt("X",x);
	wce3.WriteRecordInt("Y",y);
	wce3.WriteRecordInt("B",b);
	wce3.WriteEOL();

	wce3.WriteRecordString("R",m_pszRef);
	wce3.WriteEOL();
	if(m_pPtn != NULL){
		wce3.WriteRecordString("P",pszName);
		wce3.WriteEOL();
	}
	int n;
	for(n=0;n<m_nPinCount;n++){
		m_ppPin[n]->writeCe3(wce3);
		wce3.WriteEOL();
	}
	wce3.WriteRecordString("NOTE",m_note);
	wce3.WriteRecord("-COMP");
	wce3.WriteEOL();
	return true;
}


//デフォルト部品番号の設定
void SCompInfo::setRef(const char* ref)
{
	delete[]m_pszRef;
	m_pszRef = NULL;
	if(ref==NULL) return;
	int len = strlen(ref);
	m_pszRef = new char[len+1];
	strcpy(m_pszRef,ref);
}

//サイズの設定 パターンを有する場合はパターンのサイズも変更
void SCompInfo::setSize(const SSize& size,bool createPtn)
{
	m_size=size;
	if(m_pPtn!=NULL || createPtn){
		if(m_pPtn==NULL) m_pPtn=new SPtn;
		m_pPtn->setSize(SSize(size.w()*10+1,size.h()*10+1));
	}
}

void SCompInfo::setUsePattern(bool usepattern)
{
	if(!usepattern){
		delete m_pPtn;
		m_pPtn = NULL;
	}else if(m_pPtn == NULL){
		m_pPtn=new SPtn;
		m_pPtn->setSize(SSize(m_size.w()*10+1,m_size.h()*10+1));
	}
}

void SCompInfo::setBlock(int block)
{
	int n;
	for(n=0;n<m_nPinCount;n++)	m_ppPin[n]->setBlock(block);
	m_nBlock = block;
}

void SCompInfo::addPin(SPin* ppin)
{
	if(ppin == NULL) return;
	PSPin*	ppPin = new PSPin[m_nPinCount+1];
	int n;
	for(n=0;n<m_nPinCount;n++){
		ppPin[n] = m_ppPin[n];
	}
	ppPin[n]=ppin;
	delete[]m_ppPin;
	m_ppPin = ppPin;
	m_nPinCount++;
}

//指定した位置のピン情報を得る
SPin* SCompInfo::pinInfo(int ltrb,int offset)
{
	int n;
	for(n=0;n<m_nPinCount;n++){
		if(m_ppPin[n]->ltrb() == ltrb && m_ppPin[n]->offset() == offset) return m_ppPin[n];
	}
	return NULL;
}

void SCompInfo::removePin(int ltrb,int offset,bool move)
{
	int n,m;
	SPin* ppin=pinInfo(ltrb,offset);
	if(ppin!=NULL){
		PSPin*	ppPin = new PSPin[m_nPinCount-1];
		for(n=0,m=0;n<m_nPinCount;n++){
			if(m_ppPin[n]!=ppin){
				ppPin[m++]=m_ppPin[n];
			}
		}
		while(m<(m_nPinCount-1)) ppPin[m++]=NULL;	//ピン配列に同じピン情報が重複していたときだけ実行される
		delete ppin;
		delete[]m_ppPin;
		m_ppPin = ppPin;
		m_nPinCount--;
	}
	if(move){
		for(n=0;n<m_nPinCount;n++){
			if(m_ppPin[n]->ltrb() == ltrb && m_ppPin[n]->offset() > offset){
				m_ppPin[n]->setOffset(m_ppPin[n]->offset()-1);
			}
		}
	}
}


void SCompInfo::insertNullPin(int ltrb,int offset)
{
	int n;
	for(n=0;n<m_nPinCount;n++){
		if(m_ppPin[n]->ltrb() == ltrb && m_ppPin[n]->offset() >= offset){
			m_ppPin[n]->setOffset(m_ppPin[n]->offset()+1);
		}
	}
}

void SCompIndex::setName(const char* name)
{
	delete[]m_lpszName;
	m_lpszName=NULL;
	if(name== NULL)	return;
	int len = strlen(name);
	m_lpszName = new char[len+1];
	strcpy(m_lpszName,name);
}



//部品番号を得る
const char* SCompIndex::number() const
{
	if(m_pCompInfo==NULL) return NULL;
	return m_pCompInfo->m_pszRef;
}

//サイズを得る
SSize SCompIndex::size() const
{
	if(m_pCompInfo==NULL) return SSize(0,0);
	return m_pCompInfo->m_size;
}

//配置時の図面上のサイズを得る
SSize SCompIndex::sizePlace(int dir) const
{
	SSize wh = size();
	int w = wh.w()*10;
	int h = wh.h()*10;
	if(dir & 1){	//横サイズと縦サイズが入れ替わる
		int n=w; w=h; h=n;
	}
	return SSize(w,h);
}

//パッケージあたりのブロック数を得る
int	  SCompIndex::block() const
{
	if(m_pCompInfo==NULL) return 0;
	return m_pCompInfo->m_nBlock;
}

//パターンへのポインタを得る
SPtn*	SCompIndex::pattern() const
{
	if(m_pCompInfo==NULL) return NULL;
	return m_pCompInfo->m_pPtn;
}

//ピンの数を得る
int	SCompIndex::pinCount() const
{
	if(m_pCompInfo==NULL) return 0;
	return m_pCompInfo->m_nPinCount;
}


//ピンの情報を得る
SPin* SCompIndex::pin(int index) const
{
	if(m_pCompInfo==NULL) return NULL;
	if(index<0 || index>= m_pCompInfo->m_nPinCount) return NULL;
	return m_pCompInfo->m_ppPin[index];
}

//ピンの情報を得る
//向きの情報から、ピンの位置情報(LTRBと、原点からのオフセット)を返す
SPin* SCompIndex::pin(int index,int dir,int& nLtrb,SPoint& ptEnd) const
{
	if(m_pCompInfo==NULL) return NULL;
	if(index<0 || index>= m_pCompInfo->m_nPinCount) return NULL;
	SPin* pininfo = m_pCompInfo->m_ppPin[index];

	int ltrb = pininfo->ltrb(); // L:0 T:1 R:2 B:3
	int offset = pininfo->offset()*10;

	nLtrb = (ltrb + (dir & 3)) &3;
	if(dir & 4){
		if(nLtrb==0){
			nLtrb = 2;
		}else if(nLtrb==2){
			nLtrb = 0;
		}
	}
	
	SSize wh = sizePlace(dir);

	int w = wh.w();
	int h = wh.h();
//	int x = m_p1.x();
//	int y = m_p1.y();
	switch(nLtrb){
	case 0:		// L
		ptEnd.setX(-w-PIN_LENGTH);
		if(dir == 2 || dir == 3 || dir == 6 || dir == 7){
			ptEnd.setY(-offset);
		}else{
			ptEnd.setY(-h+offset);
		}
		break;
	case 1:		// T
		ptEnd.setY(-h-PIN_LENGTH);
		if(dir == 1 || dir == 2 || dir == 4 || dir == 7){
			ptEnd.setX(-offset);
		}else{
			ptEnd.setX(-w+offset);
		}
		break;
	case 2:		// R
		ptEnd.setX(+PIN_LENGTH);
		if(dir == 2 || dir == 3 || dir == 6 || dir == 7){
			ptEnd.setY(-offset);
		}else{
			ptEnd.setY(-h+offset);
		}
		break;
	case 3:		// B
		ptEnd.setY(+PIN_LENGTH);
		if(dir == 1 || dir == 2 || dir == 4 || dir == 7){
			ptEnd.setX(-offset);
		}else{
			ptEnd.setX(-w+offset);
		}
		break;
	}
	return pininfo;	

}



//デストラクタ
SCompLib::~SCompLib()
{
//	SPtnListIterator itePtn = m_listPtn.begin();
//	while(itePtn != m_listPtn.end()){
//		delete (*itePtn);
//		itePtn++;
//	}
	deleteContent();//内容の破棄
}

//ライブラリ名の設定
void SCompLib::setLibraryName(const char* name)
{
	delete[]m_lpszLibraryName;
	int length = strlen(name);
	m_lpszLibraryName = new char[length+1];
	strcpy(m_lpszLibraryName,name);
}


//ライブラリ名を得る
const char* SCompLib::libraryName() const
{
	return m_lpszLibraryName;
}

//n番目の部品のインデックスを得る
SCompIndex* SCompLib::compIndex(int nIndexNum) const
{
	//引数のインデックス順が部品インデックス数より多い場合は何もしない
	if(nIndexNum<0 || nIndexNum >= count()){	
		return NULL;
	}
	return m_ppCompIndex[nIndexNum];
}


//パターンのビットマップの読み込み
//ビットマップデータを読み込んだバッファを返す
//ワードアラインメント
//水平方向はバイトアライン
unsigned char* SPtn::readBmp(SReadCE3& rce3)
{
	int x=m_size.w();
	int y=m_size.h();
	
	assert(x>0 && y>0);
	if(x>501) x=501;				//xもｙも50グリッドぶんを上限とする。
	if(y>501) y=501;				//
	string str("");
	int buffXByte = ((x+15)/16)*2;	//バッファの水平方向バイト数
	int xByte = (x+7)/8;			//読み込みの水平方向バイト数。2文字で1バイトなので読み込み文字はこれの倍
	//int nBuffSize=buffXByte * y;


	unsigned char* buff= (unsigned char*)new char[m_nBuffsize];
	
	for(int n=0;n<m_nBuffsize; n++){
		buff[n]=0;
	}

	int yCount=0;
	while(rce3.ReadRecord(str)!=EOF){
		if(str[0] == '-'){
			if(str=="-BMP") return buff; 
			else goto RdErr;
		}else if(str[0] == '+'){
			str[0]='-';
			if(rce3.SkipTo(str) == EOF) goto RdErr;
		}else if(yCount<y){
			unsigned char* pbuff=buff+ yCount * buffXByte;
			int nMax=str.length()/2;
			if(nMax > xByte) nMax = xByte;
			int m=0;
			int n;
			for(n=0;n<nMax;n++){
				int uppr = str[m++] - '@';
				int lowr = str[m++] - '@';
				if(uppr>=0 && uppr<=15 && lowr>=0 && lowr<=15){
					pbuff[n]=(uppr<<4)+ lowr;
				}else{
					pbuff[n]=0;
				}
			}
			yCount++;
		}
	}
RdErr:
	delete[]buff;
	return NULL;
}


//パターンの読み込み
bool SCompLib::ReadLb3Ptn(SReadCE3& rce3)
{
	SPtn* pPtn=new SPtn;
	if(pPtn->readCe3(rce3)){
//		m_listPtn.push_back(pPtn);
		m_listPtn.push_front(pPtn);
		return true;
	}else{
		delete pPtn;
		return false;
	}
}

//既存の部品名と重複していないかどうか
bool SCompLib::isUniqueName(const string& name)
{
	SCompIndexListIterator iteIndex = m_listCompIndex.begin();
	while(iteIndex != m_listCompIndex.end()){
		if(name==(*iteIndex)->name()) return false;
		iteIndex++;
	}
	return true;
}


//既存の部品名と重ならない名前を作る
string SCompLib::uniqueName(const string& nameOld)
{
	string name = nameOld;
	string uniqueName("");
	if(isUniqueName(name)){
		return name;
	}else{
		int length=name.length();
		int pos=length-1;
		int nval=0;
		if(name[pos]==')'){			//末尾が「)」だったとき
			while(pos>0){
				pos--;
				if(name[pos]=='(' || !isdigit(name[pos]) ) break;
			}
			if(name[pos]=='('){		//末尾が「(数字)」だったとき
				nval=atoi(name.substr(pos+1,length-pos-2).c_str());
				name = name.substr(0,pos);
			}
		}
		char nstr[32];
		int i=4096;
		while(i--){
			nval++;
			sprintf(nstr,"(%d)",nval);
			uniqueName = name+nstr;
			if(isUniqueName(uniqueName)) break;
		}
		return uniqueName;
	}
}


bool SCompLib::ReadLb3Comp(SReadCE3& rce3)
{
	SCompIndex* pIndex;
	SPtn* pPtn;
	//SCompIndex* pIndex;

	string name("");
	string ptnname("");

	SCompInfo* pComp = new SCompInfo;
	if(pComp->readCe3(rce3,name,ptnname)){
		if(ptnname!=""){	//パターンが設定ずみならパターンへのポインタを検索する。
			SPtnListIterator ite = m_listPtn.begin();
			while(ite != m_listPtn.end()){
				pPtn = *ite;
				if(pPtn->m_pszName == ptnname){
					//pComp->m_pPtn = pPtn;
					pComp->m_pPtn = new SPtn(*pPtn);	//複数の部品定義でパターンを共有しない
					break;
				}
				ite++;
			}
		}
		name = uniqueName(name);
	
		pIndex = new SCompIndex;
		pIndex->setName(name.c_str());
		pIndex->setCompInfo(pComp);

		m_listCompIndex.push_back(pIndex);
		m_listCompInfo.push_back(pComp);

		return true;
	}else{
		delete pComp;
		return false;
	}
}
		


bool SCompLib::ReadLb3Alias(SReadCE3& rce3)
{
	string name("");
	string alias("");
	SCompIndex* pIndexNew;
	SCompIndex* pIndex;
	SCompIndexListIterator ite;
	string uniqName("");

	string str("");
	while(rce3.ReadRecord(str)!=EOF){
		if(str[0] == '-'){
			if(str != "-ALIAS" || name=="" || alias==""){
				goto RdErr;
			}else{
				break;
			}
		}else if(str[0] == '+'){
			str[0]='-';
			if(rce3.SkipTo(str) == EOF) goto RdErr;
		}else{
			int n;
			int l=str.length();
			for(n=0;n<l;n++){
				if(str[n]==':') break;
			}
			if(0<n && n<(l-1)){	// : で分けられたレコードである
				string var = str.substr(0,n);				//先頭から:の手前まで
				if(var=="N"){
					name=str.substr(n+1);
				}else if(var=="A"){
					alias=str.substr(n+1);
				}
			}
		} 
	}
	uniqName = uniqueName(alias);

	ite = m_listCompIndex.begin();
	while(ite != m_listCompIndex.end()){
		pIndex = *ite;
		if(pIndex->name() == name){
			pIndexNew = new SCompIndex;
			pIndexNew->setName(uniqName.c_str());
			pIndexNew->setCompInfo(pIndex->compInfo());
			m_listCompIndex.push_back(pIndexNew);
			break;
		}
		ite++;
	}
	return true;
RdErr:
	return false;
}

//LB3ライブラリの読み込み
bool SCompLib::ReadLb3(SReadCE3& rce3,bool singleComponent /* = false */)//LB3ライブラリの読み込み
{
	string str("");
	bool retv = true;

	while(rce3.ReadRecord(str)!=EOF){
		if(str[0] == '-'){
			if(str != "-BSCH3_LIB_V.1.0"){
				retv = false;
				break;
			}else{
				clearPtnList(); //パターンリスト(m_listPtn)のクリア
				if(rce3.SkipTo("+BSCH3_LIB_V.1.0")==EOF) break;
			}
		}else if(str[0] == '+'){
			if(str == "+PTN"){
				if(!ReadLb3Ptn(rce3)){
					retv = false;
					break;
				}
			}else
			if(str == "+COMP"){
				if(!ReadLb3Comp(rce3)){
					retv = false;
					break;
				}
				if(singleComponent)break;
			}else
			if(str == "+ALIAS"){
				if(!ReadLb3Alias(rce3)){
					retv = false;
					break;
				}
			}else{
				str[0]='-';
				if(rce3.SkipTo(str)==EOF){
					retv = false;
					break;
				}
			}
		}else{
			int n;
			int l=str.length();
			for(n=0;n<l;n++){
				if(str[n]==':') break;
			}
			if(0<n && n<l){
				string var = str.substr(0,n);				//先頭から:の手前まで
				n++;
				if(var == "PROP"){
					m_prop_string= str.substr(n);
				}
			}
		}
	}
	clearPtnList(); //パターンリスト(m_listPtn)のクリア
	return retv;
}

//メンバー変数パターンリストのクリア
void SCompLib::clearPtnList()
{
	SPtnListIterator itePtn = m_listPtn.begin();
	while(itePtn != m_listPtn.end()){
		delete (*itePtn++);
	}
	m_listPtn.clear();
}

bool SCompLib::WriteAlias(SWriteCE3& wce3,const char*  alias,const char*  name)
{
	if(alias == NULL || *alias == '\0') return false;
	if(name  == NULL || *name  == '\0') return false;
	wce3.WriteRecord("+ALIAS");
	wce3.WriteRecordString("A",alias);
	wce3.WriteRecordString("N",name);
	wce3.WriteRecord("-ALIAS");
	wce3.WriteEOL();
	return true;
}

//部品の書き込み
//bool SCompLib::WriteLb3Comp(SWriteCE3& wce3,const SCompInfo& compinfo,const char*  name)
//{
//}


//内容の破棄
void SCompLib::deleteContent()
{
	SCompInfoListIterator iteInfo = m_listCompInfo.begin();
	while(iteInfo != m_listCompInfo.end()){
		delete (*iteInfo);
		iteInfo++;
	}
	m_listCompInfo.clear();

	SCompIndexListIterator iteIndex = m_listCompIndex.begin();
	while(iteIndex != m_listCompIndex.end()){
		delete (*iteIndex);
		iteIndex++;
	}
	m_listCompIndex.clear();

	delete[]m_lpszLibraryName;					//ライブラリ名バッファの解放
	m_lpszLibraryName = NULL;

	delete[]m_ppCompIndex;
	m_ppCompIndex = NULL;

}



//ファイル書き込み
bool SCompLib::writeLibraryFile(const char* lpszFileName)
{
	FILE* fp;
	if((fp = fopen(lpszFileName,"w"))==NULL) return false;
	SWriteCE3 wce3;
	wce3.Attach(fp);
	wce3.WriteRecord("+BSCH3_LIB_V.1.0");
	wce3.WriteEOL();
	wce3.WriteRecordString("PROP",m_prop_string);
	wce3.WriteEOL();
	SCompIndexListIterator iteIndex = m_listCompIndex.begin();
	while(iteIndex != m_listCompIndex.end()){
		string name = (*iteIndex)->name();
		SCompInfo* pInfo = (*iteIndex)->compInfo();
		bool bAlias = false;
		string aliasFor("");
		SCompIndexListIterator iteIndex2 = m_listCompIndex.begin();
		while(iteIndex2 != iteIndex && iteIndex2 != m_listCompIndex.end()){
			if((*iteIndex2)->compInfo()==pInfo){
				bAlias = true;
				aliasFor = (*iteIndex2)->name();	//書き込み済みの名前
				break;
			}
			iteIndex2++;
		}
		if(bAlias){
			WriteAlias(wce3,name.c_str(),aliasFor.c_str());
		}else{
			pInfo->writeCe3(wce3,name.c_str());
		}
		iteIndex++;
	}
	wce3.WriteRecord("-BSCH3_LIB_V.1.0");
	wce3.WriteEOL();

	fclose(fp);
	return true;
}



//ファイル読み込み
bool SCompLib::readLibraryFile(const char* lpszFileName)
{

	int nCount;
	if(m_lpszLibraryName!=NULL){	//既に設定済みなら何もしない
		return false;
	}

	//qDebug("***** %s *****",lpszFileName);

	SReadCE3 rce3;
	string str("");
	SCompIndexListIterator iteIndex;

	FILE* fp=fopen(lpszFileName,"r");
	if(fp==NULL) return false;
	rce3.Attach(fp);
	if(rce3.ReadRecord(str)==EOF) goto RdErr;
	if(str == "+BSCH3_DATA_V.1.0"){
		rce3.SkipTo("+BSCH3_LIB_V.1.0");
	}else{
		if(str != "+BSCH3_LIB_V.1.0") goto RdErr;
	}
	if(!ReadLb3(rce3)) goto RdErr;
	fclose(fp);

	int n;
	int len;

	nCount = m_listCompIndex.size();

	m_ppCompIndex = new PSCompIndex[nCount];
	iteIndex = m_listCompIndex.begin();
	n=0;
	while(iteIndex != m_listCompIndex.end()){
		//const char* pname=(*iteIndex)->name();
		//qDebug("   %s",pname);
		m_ppCompIndex[n++]=(*iteIndex);
		iteIndex++;
	}
	//ライブラリ名の設定
	setLibraryName(lpszFileName);

	//タイムスタンプの記録 OS依存
#ifdef _WINDOWS
	_finddata_t findData;
	if(_findfirst(lpszFileName,&findData)!=-1){
		m_aux1=findData.time_write;
	}
#endif

	return true;

RdErr:
	fclose(fp);
	return false;
}



