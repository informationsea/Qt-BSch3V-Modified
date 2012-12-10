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

/****************************************************************************
** XBSch図面要素接合点クラスの実装
****************************************************************************/
//#include "stdafx.h"
#include <stdio.h>
//#include <qnamespace.h>
#include <string>
using namespace std;
#include "ce3io.h"
#include "xbschobj.h"
#include "xbschjunc.h"


//コンストラクタ
SXBSchJunc::SXBSchJunc()
{
}


bool SXBSchJunc::readCe3(SReadCE3& rce3,const SPoint* pptOrigin,SXBSchDoc*)
{
	string str("");
	bool bInitX1 = false;
	bool bInitY1 = false;
	while(1){
		if(rce3.ReadRecord(str)==EOF) return false;
		if(str[0] == '-'){
			if( (str != "-JUNCTION") || !bInitX1 || !bInitY1){
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
				if(var=="X"){
					if(pptOrigin) nParam += pptOrigin->x();		
					m_p1.setX(nParam);
					bInitX1 = true;
				}else 
				if(var=="Y"){
					if(pptOrigin) nParam += pptOrigin->y();		
					m_p1.setY(nParam);
					bInitY1 = true;
				}
			}
		}
	}

	return true;
}

/*
//ストリームからの読み込み ～-JUNCTION
bool SXBSchJunc::readStream(SCsvIStream* pStrm,const SPoint* pptOrigin)
{
	if(!pStrm) return false;
	QString str;
	QString var;
	QString val;
	int nParam=0;
	bool bInitX1 = false;
	bool bInitY1 = false;
	while(1){
		int nResult = pStrm->getRecord(str);
		if(nResult == SCsvIStream::GETRECORD_EOF || nResult == SCsvIStream::GETRECORD_ERROR){
			return false;
		}
		//str = deleteLeftSpace(str);
		if(str[0] == '-'){
			if( (str != "-JUNCTION") || !bInitX1 || !bInitY1){
				return false;
			}else{
				break;	//break while(1){ }
			}
		}else if(str[0] == '+'){
			if(pStrm->skipTo(str.replace(1,1,"-")) != SCsvIStream::GETRECORD_NORMAL)return false;
		}else{
			if(SCsvIStream::splitToVariableValue(str,var,val)){
				nParam = val.toInt();
				if(var=="X"){
					if(pptOrigin) nParam += pptOrigin->x();		
					m_p1.setX(nParam);
					bInitX1 = true;
				}else 
				if(var=="Y"){
					if(pptOrigin) nParam += pptOrigin->y();		
					m_p1.setY(nParam);
					bInitY1 = true;
				}
			}
		}
	}

	return true;
}
*/

bool SXBSchJunc::writeCe3(SWriteCE3& wce3,const SPoint* pptOrigin)
{
//	char sz[32];
	int x = m_p1.x();
	int y = m_p1.y();
	if(pptOrigin){
		x -= pptOrigin->x();
		y -= pptOrigin->y();
	}

	wce3.WriteRecord("+JUNCTION");
	wce3.WriteRecordInt("L",m_Layer);
	wce3.WriteRecordInt("X",x);
	wce3.WriteRecordInt("Y",y);


	wce3.WriteRecord("-JUNCTION");
	wce3.WriteEOL();
	return true;
}



//複製を行う
SXBSchObj* SXBSchJunc::duplicate()
{
	SXBSchObj* newObj =new SXBSchJunc(*this);
	return newObj;
}

//点を指定して選択重要度を返す
int SXBSchJunc::qSelectWeight(const SPoint& p)
{
	int nResult = PointAndRectMag(p,area());
	if(nResult) return (nResult | ON_OBJ);
	else		return 0;
}

//範囲を指定して選択を行う
unsigned SXBSchJunc::testSelection(const SRect& rc)
{
	if( rc.intersect(area()) ){
		return SELECT_ALL;
	}else{
		return 0;
	}
}

//範囲を選択してドラッグ選択を行う
unsigned SXBSchJunc::testSelectionForDrag(const SRect& rc)
{
	if( rc.intersect(area()) ){
		return SELECT_ALL;
	}else{
		return 0;
	}
}

//描画が必要かどうかを返す
bool SXBSchJunc::qRedraw(const SRect& rc)
{
	return rc.intersect(area());
}

//占有範囲を返す
SRect SXBSchJunc::area()
{
	SRect rc(m_p1.x()-2,m_p1.y()-2,5,5);
	return rc;
}

SRect SXBSchJunc::rotateRect()
{
	return SRect(m_p1,m_p1);
}