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
/****************************************************************************
** XBSch図面要素線クラスの実装
****************************************************************************/
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string>
#include <stdlib.h>
using namespace std;
#include "ce3io.h"
#include "xbschobj.h"
#include "xbschline.h"
//#include <SPoint.h>
//#include <SRect.h>

//コンストラクタ
SXBSchLine::SXBSchLine()
{
}

//選択状態のときのスナップポイントを取得する
SPoint SXBSchLine::snapPoint()
{
	if(m_Selection & SELECT_P1) 		return m_p1;
	else if(m_Selection & SELECT_P2) 	return m_p2;
	else 								return m_p1;
}

//点を指定して選択重要度を返す
//線オブジェクトの重要度は常に1
int  SXBSchLine::qSelectWeight(const SPoint& pt)
{
	SRect rcSpot(pt.x()-2,pt.y()-2,5,5);
	if(testSelection(rcSpot)){
		return (1 | ON_OBJ); 
	}else{
		return 0;
	}
}

//範囲を指定して選択を行う
unsigned SXBSchLine::testSelection(const SRect& rc)
{
	unsigned nOutcode1 = SetOutCode(m_p1,rc);
	unsigned nOutcode2 = SetOutCode(m_p2,rc);
	if(nOutcode1 & nOutcode2) return 0;
	if((nOutcode1 == 0 )||(nOutcode2 == 0 )) return SELECT_ALL;
	unsigned nOutcode = nOutcode1 | nOutcode2;
	if(nOutcode == (OUTCODE_LEFT  | OUTCODE_RIGHT)) return SELECT_ALL;
	if(nOutcode == (OUTCODE_UPPER | OUTCODE_LOWER)) return SELECT_ALL;

	int x,y;
	int x1 = m_p1.x();
	int y1 = m_p1.y();
	int x2 = m_p2.x();
	int y2 = m_p2.y();

	if(nOutcode & OUTCODE_LEFT){	//片方が左に出ている
		x=rc.l();
		y=y1+((x-x1)*(y2-y1))/(x2-x1);
		if(y >= rc.t() && y <= rc.b()) return SELECT_ALL;
	}
	if(nOutcode & OUTCODE_RIGHT){	//片方が右に出ている
		x=rc.r();
		y=y1+((x-x1)*(y2-y1))/(x2-x1);
		if(y >= rc.t() && y <= rc.b()) return SELECT_ALL;
	}
	if(nOutcode & OUTCODE_UPPER){	//片方が上に出ている
		y=rc.t();
		x=x1+((y-y1)*(x2-x1))/(y2-y1);
		if(x >= rc.l() && x <= rc.r()) return SELECT_ALL;
	}
	if(nOutcode & OUTCODE_LOWER){	//片方が下に出ている
		y=rc.b();
		x=x1+((y-y1)*(x2-x1))/(y2-y1);
		if(x >= rc.l() && x <= rc.r()) return SELECT_ALL;
	}
	return 0;
}

//範囲を選択してドラッグ選択を行う
unsigned SXBSchLine::testSelectionForDrag(const SRect& rc)
{
	unsigned nResult = 0;
	unsigned nOutcode1 = SetOutCode(m_p1,rc);
	unsigned nOutcode2 = SetOutCode(m_p2,rc);
	if(nOutcode1 & nOutcode2) return 0;
	if(nOutcode1 == 0 ) nResult |= SELECT_P1;
	if(nOutcode2 == 0 ) nResult |= SELECT_P2;
	return nResult;
}

//描画が必要かどうかを返す
bool SXBSchLine::qRedraw(const SRect& rc)
{
	SRect rcDraw(rc.l()-1,rc.t()-1,rc.r()+1,rc.b()+1);
	if(testSelection(rcDraw)) 	return true;
	else 						return false;
}

SRect SXBSchLine::area()
{
	SRect rcResult = SRect(SPoint(INT_MAX,INT_MAX),SPoint(INT_MIN,INT_MIN));
	if(m_Selection & (SELECT_P1 | SELECT_P2)){
		if(m_Selection & SELECT_P1) rcResult |= SRect(m_p1.x()-2,m_p1.y()-2,5,5);
		if(m_Selection & SELECT_P2) rcResult |= SRect(m_p2.x()-2,m_p2.y()-2,5,5);
	}else{
		rcResult |= SRect(m_p1.x()-2,m_p1.y()-2,5,5);
		rcResult |= SRect(m_p2.x()-2,m_p2.y()-2,5,5);
	}
	return rcResult;
}

SRect SXBSchLine::rotateRect(){
	SRect rc(m_p1,m_p2);
	rc.normalize();
	return rc;
}
//ストリームからの読み込み
bool SXBSchLine::readCe3Ex(SReadCE3& rce3,const SPoint* pptOrigin,const char* pTag)
{
	assert(pTag != NULL);
	string str;
	bool bInitX1 = false;
	bool bInitY1 = false;
	bool bInitX2 = false;
	bool bInitY2 = false;
	while(1){
		if(rce3.ReadRecord(str)==EOF) return false;
		if(str[0] == '-'){
			if( (str.substr(1) != pTag) || !bInitX1 || !bInitY1 || !bInitX2 || !bInitY2){
				return false;
			}else{
				break;	//break while(1){ }
			}
		}else if(str[0] == '+'){
			str[0]='-';
			if(rce3.SkipTo(str)==EOF) return false;
		}else{
			int n;
			int l=str.length();
			for(n=0;n<l;n++){
				if(str[n]==':') break;
			}
			if(0<n && n<(l-1)){	// : で分けられたレコードである
				string var = str.substr(0,n);				//先頭から:の手前まで
				int nParam = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換

#ifdef _BSCH	//Qt-BSch doesn't support layer.
				if(var=="L"){
					if(isValidLayer(nParam)){
						m_Layer	= nParam;			
					}
				}else
#endif
				
				if(var == "X1"){
					if(pptOrigin) nParam += pptOrigin->x();		
					m_p1.setX(nParam);
					bInitX1 = true;
				}else
				if(var == "Y1"){
					if(pptOrigin) nParam += pptOrigin->y();		
					m_p1.setY(nParam);
					bInitY1 = true;
				}else
				if(var == "X2"){
					if(pptOrigin) nParam += pptOrigin->x();		
					m_p2.setX(nParam);
					bInitX2 = true;
				}else
				if(var == "Y2"){
					if(pptOrigin) nParam += pptOrigin->y();		
					m_p2.setY(nParam);
					bInitY2 = true;
				}
			}

		}
	}

	return true;
}

//ストリームへの書き込み
bool SXBSchLine::writeCe3Ex(SWriteCE3& wce3,const SPoint* pptOrigin,const char* pTag)
{
	assert(pTag != NULL);

	char sz[32];
	int x1 = m_p1.x();
	int y1 = m_p1.y();
	int x2 = m_p2.x();
	int y2 = m_p2.y();
	if(pptOrigin){
		x1 -= pptOrigin->x();
		y1 -= pptOrigin->y();
		x2 -= pptOrigin->x();
		y2 -= pptOrigin->y();
	}

	sprintf(sz,"+%s",pTag);
	wce3.WriteRecord(sz);
	wce3.WriteRecordInt("L",m_Layer);
	wce3.WriteRecordInt("X1",x1);
	wce3.WriteRecordInt("Y1",y1);
	wce3.WriteRecordInt("X2",x2);
	wce3.WriteRecordInt("Y2",y2);

//	sprintf(sz,"X1:%d",x1);
//	wce3.WriteRecord(sz);
//	sprintf(sz,"Y1:%d",y1);
//	wce3.WriteRecord(sz);
//	sprintf(sz,"X2:%d",x2);
//	wce3.WriteRecord(sz);
//	sprintf(sz,"Y2:%d",y2);
//	wce3.WriteRecord(sz);

	sprintf(sz,"-%s",pTag);
	wce3.WriteRecord(sz);
	wce3.WriteEOL();
	return true;
}

//ワイヤーデータクラス
//ストリームへの書き込み　+WIRE ~ -WIRE
bool SXBSchWire::readCe3(SReadCE3& rce3,const SPoint* pptOrigin,SXBSchDoc*)
{
	return readCe3Ex(rce3,pptOrigin,"WIRE");
}

bool SXBSchWire::writeCe3(SWriteCE3& wce3,const SPoint* pptOrigin)
{
	return writeCe3Ex(wce3,pptOrigin,"WIRE");
}



//複製を行う
SXBSchObj* SXBSchWire::duplicate()
{
	SXBSchObj* newObj =new SXBSchWire(*this);
	return newObj;
}


//バスデータクラス
//ストリームからの読み込み
bool SXBSchBus::readCe3(SReadCE3& rce3,const SPoint* pptOrigin,SXBSchDoc*)
{
	return readCe3Ex(rce3,pptOrigin,"BUS");
}

//ストリームへの書き込み　+BUS ~ -BUS
bool SXBSchBus::writeCe3(SWriteCE3& wce3,const SPoint* pptOrigin)
{
	return writeCe3Ex(wce3,pptOrigin,"BUS");
}


//複製を行う
SXBSchObj* SXBSchBus::duplicate()
{
	SXBSchObj* newObj =new SXBSchBus(*this);
	return newObj;
}


//破線データクラス

//ストリームからの読み込み
bool SXBSchDash::readCe3(SReadCE3& rce3,const SPoint* pptOrigin,SXBSchDoc*)
{
	return readCe3Ex(rce3,pptOrigin,"DASH");
}

//ストリームへの書き込み　+DASH ~ -DASH
bool SXBSchDash::writeCe3(SWriteCE3& wce3,const SPoint* pptOrigin)
{
	return writeCe3Ex(wce3,pptOrigin,"DASH");
}


//複製を行う
SXBSchObj* SXBSchDash::duplicate()
{
	SXBSchObj* newObj =new SXBSchDash(*this);
	return newObj;
}
