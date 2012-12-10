/****************************************************************************
    NL3W netlist utility for BSch3V
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

// BSch用のNListから移植
// BSch用 NListとの相違点
// ・ラベル、タグの文字列の前後の空白は無視する
// ・部品は番号があれば良しとする。

#ifdef _MSVC
	#pragma warning (disable: 4786)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef _WINDOWS
	#include <direct.h>
#else
	#include <unistd.h>
#endif
	
#include <ctype.h>
#include <string>
#include <algorithm>
#include <list>
using namespace std;

#include "netlist.h"
#include "xbschglobal.h"

SSingleNetList::~SSingleNetList()
{
	int n;
	for(n=0;n<m_nPinCount;n++){
		delete m_pPinArray[n];
	}
	delete[]m_pPinArray;
}

//SXBSchObjのポインタリストの解放
void SNetList::clearXBSchObjList(XBSchObjList& listObj)
{
	XBSchObjListIterator ite;
	ite = listObj.begin();
	while(ite != listObj.end()){
		delete(*ite);
		ite++;
	}
	listObj.clear();
}

void SNetList::clearPlistElem()
{
	if(m_pArrayPlistElem==NULL) return;
	int n=m_listComponent.size();
	for(int i=0;i<n;i++){
		delete m_pArrayPlistElem[i];
	}
	delete[]m_pArrayPlistElem;
}

//SNlPinのポインタリストの解放
void SNetList::clearNlPin(NlPinList& listPin)
{
	NlPinListIterator ite;
	ite = listPin.begin();
	while(ite != listPin.end()){
		delete(*ite);
		ite++;
	}
	listPin.clear();
}

void SNetList::clearNetList(NetList& netlist)
{
	NetListIterator ite = netlist.begin();
	while(ite!=netlist.end()){
		delete (*ite);
		ite++;
	}
	netlist.clear();
}

SNetList::SNetList()
{
	m_xOffset=0;
	m_pArrayPlistElem=NULL;
}


SNetList::~SNetList()
{
	clear();
}

void SNetList::clear()
{
	clearPlistElem();
	clearXBSchObjList(m_listComponent);
	m_listNcTagPower.clear();	//このリストが管理している実体はm_listTagPowerでdeleteする。
	clearXBSchObjList(m_listTagPower);
	clearXBSchObjList(m_listWire);
	clearXBSchObjList(m_listJunc);
	clearXBSchObjList(m_listLabel);
	clearNlPin(m_listNlPin);
	clearNlPin(m_listNlNcPin);
	clearNlPin(m_listNoNumPin);
	clearNetList(m_listNet);
	m_listNlConfPin.clear();
	m_xOffset=0;
}

//文字列の両端のホワイトスペースの削除
string SNetList::removeWS(const string& str)
{
	int b=0;
	int e=str.length();
	if(e==0)return str;
	e--;
	while(str[b]==' '||str[b]=='\t') b++;
	while(b<e && (str[e]==' '||str[e]=='\t')) e--;
	return str.substr(b,e-b+1);
}

//水平または垂直の線の結合をテストする。
//結合している場合や重なっている場合は結合した2本の両端を返す
bool SNetList::testLineJoint(int& al,int& ar,int bl,int br)
{
	int n;

	// L<R を保証する。
	// BSchのデータではこのようになっているはずなので冗長かもしれない
	if(al > ar){
		n=al;
		al = ar;
		ar = n;
	}
	if(bl > br){
		n=bl;
		bl = br;
		br = n;
	}
	if(ar < bl) return false;//  al----ar  [隙間]   bl----br
	if(br < al) return false;//  bl----br  [隙間]   al----ar
	if(al > bl) al=bl;
	if(ar < br) ar=br;
	return true;
}

//ワイヤーの連結整理
//水平または垂直のワイヤーのみが対象
void SNetList::jointWire()
{
	//検索元
	XBSchObjListIterator ite;
	SXBSchWireNl* pObj;
	SPoint pa1;
	SPoint pa2;

	//検索対象
	XBSchObjListIterator ite2;
	XBSchObjListIterator iteOld;	//オブジェクトの消去用
	SXBSchWireNl* pObj2;
	SPoint pb1;
	SPoint pb2;
	
	bool fH;	//水平フラグ
	
	bool fChange=true;

	while(fChange){
		//qDebug("s_JointWire() LOOP");
		fChange=false;
		ite=m_listWire.begin();
		while(ite != m_listWire.end()){
			int lt,rb;
			bool fJoint=false;
			pObj=(SXBSchWireNl*)(*ite);
			pa1=pObj->p1();
			pa2=pObj->p2();
			if(pa1.y()==pa2.y()){		//水平なら
				fH = true;
				lt = pa1.x();
				rb = pa2.x();
			}else{
				if(pa1.x()!=pa2.x()){	//垂直でもなければ
					ite++;				//リスト検索をひとつ進める
					continue;			
				}
				fH = false;
				lt = pa1.y();
				rb = pa2.y();
			}
			//連結できるものを検索する
			ite2=ite;
			ite2++;						//イテレータ自らの次のものから検索する。
			while(ite2 != m_listWire.end()){
				iteOld = ite2;
				ite2++;
				pObj2=(SXBSchWireNl*)(*iteOld);
				pb1=pObj2->p1();
				pb2=pObj2->p2();
				if(fH){
					if(pa1.y()!=pb1.y() || pb1.y()!=pb2.y()) continue;
					if(testLineJoint(lt,rb,pb1.x(),pb2.x())){
						fJoint=true;
						delete pObj2;
						ite2 = m_listWire.erase(iteOld);
					}
				}else{
					if(pa1.x()!=pb1.x() || pb1.x()!=pb2.x()) continue;
					if(testLineJoint(lt,rb,pb1.y(),pb2.y())){
						fJoint=true;
						delete pObj2;
						ite2 = m_listWire.erase(iteOld);
					}
				}
			}
			if(fJoint){
				if(fH){		
					pa1.setX(lt);	pa2.setX(rb);
				}else{
					pa1.setY(lt);	pa2.setY(rb);
				}
				pObj->setP1(pa1);
				pObj->setP2(pa2);
				fChange=true;
			}
			ite++;
		}
	}
}


//ワイヤーを接合点で分割する
//水平または垂直のワイヤーのみが対象
void SNetList::divideWire()
{
	XBSchObjListIterator iteJ;
	SXBSchJunc* pJ;
	XBSchObjListIterator iteW;
	SXBSchWireNl* pW;
	SXBSchWireNl* pWNew;
	SPoint point;
	
	iteJ = m_listJunc.begin();
	while(iteJ != m_listJunc.end()){
		pJ = (SXBSchJunc*)(*iteJ);
		point = pJ->p1();
		iteW = m_listWire.begin();
		while(iteW != m_listWire.end()){
			//int wcount = m_listWire.size();
			//printf("%d\n",wcount);
			pW = (SXBSchWireNl*)(*iteW);
			pWNew = pW->divide(point);
			if(pWNew){
				m_listWire.push_back(pWNew);
			}
			iteW++;
		}
		iteJ++;
	}
}



//タグと電源部品を元に線とピンに名前をつける
void SNetList::nameFromTagPower()
{
	XBSchObjListIterator iteTP;
	SXBSchComponent* pPC;
	SXBSchTag* pTag;

	XBSchObjListIterator iteW;
	SXBSchWireNl* pW;
	NlPinListIterator itePin;
	SNlPin* pPin;

	SPoint p1;
	SPoint p2;
	SPoint point;

	string signalName;

	int nPinCount;
	int i;

	iteTP = m_listTagPower.begin();
	while(iteTP != m_listTagPower.end()){
		if((*iteTP)->id() == SXBSchObj::ID_COMPONENT){
			pPC = (SXBSchComponent*)(*iteTP);
			signalName = pPC->name();			//信号名に使うために部品名を得る
			if(signalName.length() == 0){		//部品名が空白だったら...
				signalName = pPC->orgname();	//代わりにライブラリ部品名を使う
			}
		}else{
			pTag = (SXBSchTag*)(*iteTP);
			signalName = pTag->text();//信号名に使うためにタグテキストを得る
		}
		signalName = removeWS(signalName);
		if(signalName.length()==0){
			iteTP++;
			continue;
		}
		nPinCount=(*iteTP)->pinCount();

		bool bUsed=false;

		for(i=0;i<nPinCount;i++){
			point=(*iteTP)->pinPosition(i);
			iteW = m_listWire.begin();
			while(iteW != m_listWire.end()){
				pW = (SXBSchWireNl*)(*iteW); 
				p1 = pW->p1();
				p2 = pW->p2();
				if(point == p1 || point == p2){
					pW->m_listSignalName.push_back(signalName);
					bUsed=true;
				}
				iteW++;
			}
			itePin = m_listNlPin.begin();
			while(itePin != m_listNlPin.end()){
				pPin = (SNlPin*)(*itePin); 
				if(point == pPin->m_point){
					pPin->m_listSigName.push_back(signalName);
					bUsed=true;
				}
				itePin++;
			}
		}
		if(!bUsed){
			m_listNcTagPower.push_back(*iteTP);
		}
		iteTP++;
	}
}


//ラベルを元に線に線名を追加
void SNetList::nameFromLabel()
{
	XBSchObjListIterator iteLabel;
	SXBSchLabel* pLabel;
	XBSchObjListIterator iteW;
	SXBSchWireNl* pWire;


	SPoint p1;
	SPoint p2;
	SPoint point;
	string signalName;
	bool fH;

	iteLabel = m_listLabel.begin();
	while(iteLabel != m_listLabel.end()){
		pLabel = (SXBSchLabel*)(*iteLabel);
		iteLabel++;
		signalName = pLabel->text();//線名に使うためにラベル文字列を得る
		signalName = removeWS(signalName);
		if(signalName.length()==0) continue;
		fH = pLabel->horizontal();
		point = pLabel->p1();

		iteW = m_listWire.begin();
		while(iteW != m_listWire.end()){
			pWire = (SXBSchWireNl*)(*iteW);
			iteW++;
			p1 = pWire->p1();
			p2 = pWire->p2();
			if(!fH){
				if( point.x()!=p1.x() || p1.x()!=p2.x() )continue;
				if(   point.y() >= p1.y() && point.y() <= p2.y() 
				   || point.y() <= p1.y() && point.y() >= p2.y() )
					pWire->m_listSignalName.push_back(signalName);
			}else{
				if( point.y()!=p1.y() || p1.y()!=p2.y() )continue;
				if(   point.x() >= p1.x() && point.x() <= p2.x() 
				   || point.x() <= p1.x() && point.x() >= p2.x() )
					pWire->m_listSignalName.push_back(signalName);
			}

		}
	}
}

//SXBSchDocが管理している図面要素から次の要素をコピーして振り分け先のリストに追加する。
//  接合点
//  ワイヤー		: ワイヤーはSXBSchWireNlに変換する
//  ラベル
//  タグ/電源部品
//  部品			: 部品番号が欠けているもの、部品番号が同じで部品名が異なる部品は除く
void SNetList::setData(XBSchObjList* plistSrc)
{
	SXBSchObj* pNewObj;
	int x=m_xOffset;
	int y=0;
	m_xOffset+=10000;
	XBSchObjListIterator ite=plistSrc->begin();
	while(ite!=plistSrc->end()){
		SXBSchObj* pObj = (*ite++);
		switch(pObj->id()){
		case SXBSchObj::ID_JUNCTION:
			pNewObj = pObj->duplicate();
			pNewObj->move(x,y);
			m_listJunc.push_back(pNewObj);
			break;
		case SXBSchObj::ID_WIRE:
			{
				SXBSchWire* pWire = (SXBSchWire*)pObj;
				pNewObj =(SXBSchObj*)(new SXBSchWireNl(*pWire));
				pNewObj->move(x,y);
				m_listWire.push_back(pNewObj);
			}
			break;
		case SXBSchObj::ID_LABEL:
			pNewObj = pObj->duplicate();
			pNewObj->move(x,y);
			m_listLabel.push_back(pNewObj);
			break;
		case SXBSchObj::ID_TAG:
			pNewObj = pObj->duplicate();
			pNewObj->move(x,y);
			m_listTagPower.push_back(pNewObj);
			break;
		case SXBSchObj::ID_COMPONENT:
			{
				SXBSchComponent* pComp=(SXBSchComponent*)pObj;
				if(strlen(pComp->refnum())==0){		//部品番号がない場合は通常部品としては扱わない
					if(pComp->pinCount()==1){		//ピン数が1つのときだけ電源部品として扱う
						pNewObj = pObj->duplicate();
						pNewObj->move(x,y);
						m_listTagPower.push_back(pNewObj);
					}
				}else{
					pNewObj = pObj->duplicate();
					pNewObj->move(x,y);
					m_listComponent.push_back(pNewObj);
				}
			}
			break;
		}
	}
	//fprintf(stderr,"Junction :%d\n",m_listJunc.size());
	//fprintf(stderr,"Wire     :%d\n",m_listWire.size());
	//fprintf(stderr,"Component:%d\n",m_listComponent.size());
	//fprintf(stderr,"Tag&Power:%d\n",m_listTagPower.size());
	//fprintf(stderr,"Label    :%d\n",m_listLabel.size());
}
			

//有効な部品のピンを列挙する
void SNetList::enumPin()
{
	XBSchObjListIterator ite;
	SXBSchComponent* pComp;
	string refnum;
	string pinnum;
	SPoint point;
	int nPinCount;
	int i;
	int nBlock;

	ite = m_listComponent.begin();
	while(ite != m_listComponent.end()){
		pComp = (SXBSchComponent*)(*ite);
		ite++;
		refnum  = pComp->refnum();
		nPinCount = pComp->pinCount();
		nBlock = pComp->block();
		for(i=0;i<nPinCount;i++){
			int nLTRB;												//呼び出しのためのダミー
			const SPin* pPinInfo = pComp->pinEnd(i,nLTRB,point);	//ピン情報の取得
			if(pPinInfo == NULL) break;
			pinnum = pPinInfo->pinNum(nBlock);
			//if(pinnum.length()==0) break;
			SNlPin* pNlPin = new SNlPin;							//ネットリスタが使うピン形式を生成
			pNlPin->m_strRefNum = refnum;							//部品番号
			pNlPin->m_strPinNum = pinnum;							//ピン番号
			pNlPin->m_point = point;								//ピン位置
			if(pinnum.length()==0){
				m_listNoNumPin.push_back(pNlPin);
			}else{	
				//ピンを管理するリストに追加
				m_listNlPin.push_back(pNlPin);
			}
		}
	}
}



//指定文字列が文字列リストに存在するかどうかどうかのチェック
bool SNetList::quaryExist(stringList& strlist,string& str)
{
	if(strlist.end() != find(strlist.begin(),strlist.end(),str)) return true;
	else return false;
}

//指定点が点リストに存在するかどうかどうかのチェック
bool SNetList::quaryExist(PointList& pointlist,SPoint point)
{
	if(pointlist.end() != find(pointlist.begin(),pointlist.end(),point)) return true;
	else return false;
}



//ピンの名前(ここでは部品番号とピン番号から構成される、ネットリストでのピンの名前)による大小比較
int SNetList::CompareSNlPin( const void *ppin1, const void *ppin2 )
{
	string strPrefix1;
	string strSuffix1;
	string strPrefix2;
	string strSuffix2;
	int nComp;
	int num1,num2;

	SNlPin* pPin1 = *(SNlPin**)(ppin1);
	SNlPin* pPin2 = *(SNlPin**)(ppin2);

	//リファレンス番号で比較
	parseRefNum(pPin1->m_strRefNum,strPrefix1,num1);
	parseRefNum(pPin2->m_strRefNum,strPrefix2,num2);

	nComp = strcmp(strPrefix1.c_str(),strPrefix2.c_str());
	if(nComp) return nComp;
	nComp = num1-num2;
	if(nComp) return nComp;


	//ピン番号で比較
	parseRefNum(pPin1->m_strPinNum,strPrefix1,num1);
	parseRefNum(pPin2->m_strPinNum,strPrefix2,num2);

	nComp = strcmp(strPrefix1.c_str(),strPrefix2.c_str());
	if(nComp) return nComp;
	num1 = atoi(strSuffix1.c_str());
	num2 = atoi(strSuffix2.c_str());
	nComp = num1-num2;
	return nComp;

}


//SPartsListElementの部品番号による比較
int SNetList::CompareSPartsListElement( const void *p1, const void *p2 )
{
	SPartsListElement* pElem1 = *(SPartsListElement**)(p1);
	SPartsListElement* pElem2 = *(SPartsListElement**)(p2);
	int nComp = strcmp(pElem1->m_prefix.c_str(),pElem2->m_prefix.c_str());
	if(nComp) return nComp;
	int n = pElem1->m_suffix - pElem2->m_suffix;
	if(n!=0) return n;
	return strcmp(pElem1->m_name.c_str(),pElem2->m_name.c_str());
}


void SNetList::enumConflictingPin()
{
	NlPinListIterator itePin1 = m_listNlPin.begin();
	while(itePin1 != m_listNlPin.end()){
		if(!(*itePin1)->m_checked){
			NlPinListIterator itePin2=itePin1;
			itePin2++;
			bool firstFind=true;
			while(itePin2 != m_listNlPin.end()){
				if(!(*itePin2)->m_checked){
					if(   ((*itePin1)->m_strRefNum == (*itePin2)->m_strRefNum)
						&&((*itePin1)->m_strPinNum == (*itePin2)->m_strPinNum) ){
						if(firstFind){
							firstFind=false;
							m_listNlConfPin.push_back(*itePin1);
						}
						(*itePin2)->m_checked = true;
						m_listNlConfPin.push_back(*itePin2);
					}
				}
				itePin2++;
			}
			if(!firstFind){		//重複が見つかった
				m_listNlConfPin.push_back(NULL);
			}
		}
		itePin1++;
	}
}



//ネット抽出
SSingleNetList* SNetList::NetList1()
{
	//printf("newnet\n");
	SNlPin*		pPin;
	SPoint		point;
	NlPinList	listPin;							//ネットに含まれるピン
	stringList  listSignalNameAll;					//ネットに関わる未処理信号名
	stringList  listSignalName;						//ネットに関わる未処理信号名
	PointList	listPoint;							//ネットに関わる座標

	XBSchObjListIterator ite1;
	SSingleNetList* pSingleNet;
	
	NlPinListIterator itePin = m_listNlPin.begin();	//ピンのリストからピンをひとつ取り出す
	if(itePin == m_listNlPin.end()) return NULL;	//ピンがなければ何もせずに戻る

	pPin=(*itePin);
	m_listNlPin.erase(itePin);						//取り出したピンは元のリストから消去して
	pSingleNet = new SSingleNetList;
	listPin.push_back(pPin);						//ネットに登録
	listPoint.push_back(pPin->m_point);				//ピン位置を座標のリストに登録する。
	
	//ピンの信号名を取得して、すべて信号名のリストに登録する。
	stringListIterator iteStr=pPin->m_listSigName.begin();
	while(iteStr!=pPin->m_listSigName.end()){
		listSignalNameAll.push_back(*iteStr);
		listSignalName.push_back(*iteStr);
		iteStr++;
	}

	//処理が発生しなくなるまで１. ２.を繰り返す
	//１．未処理信号名がなくなるまで(1)(2)を繰り返し実行する。
	//	(1)信号名によるピンの抽出
	//	    同じ信号名を持つピンの取り出し
	//	    取り出したピンが持つ別の信号名の追加登録
	//	    取り出したピンの座標の登録
	//	(2)信号名によるワイヤーの抽出
	//	    同じ信号名を持つワイヤーの取り出し
	//		取り出したワイヤーが持つ別の信号名の追加登録
	//	    取り出したワイヤーの座標の登録
	//
	//２．未処理座標がなくなるまで(3)(4)を繰り返し実行する。
	//	(3)座標によるピンの抽出
	//		その座標を持つピンの取り出し
	//		取り出したピンが持つ別の信号名の追加登録
	//	(4)座標によるワイヤーの取り出し
	//		その座標を持つワイヤーの取り出し
	//		取り出したワイヤーが持つ別の信号名の追加登録
	//		取り出したワイヤーの座標の登録
	//

	stringListIterator iteSignalName;
	PointListIterator  itePoint;

	bool fDone = true;
	int  loopCount = 0;
	while(fDone){
		//printf("%d\n",loopCount);
		fDone = false;
		//***** １．信号名処理 *****
		iteSignalName = listSignalName.begin();
		while(iteSignalName != listSignalName.end()){
			//printf("SigName");
			fDone = true;
			string strSignalName = (*iteSignalName);
			
			//***** (1)信号名によるピンの抽出 *****
			NlPinListIterator itePin = m_listNlPin.begin();
			NlPinListIterator itePinBackUp;
			while(itePin != m_listNlPin.end()){				//信号名について全てのピンをスキャンする
				SNlPin* pPin =(*itePin);
				itePinBackUp = itePin;
				itePin++;
				if(quaryExist(pPin->m_listSigName,strSignalName)){							//ピンが持つ信号名のひとつが検索中の信号名に合致
					m_listNlPin.erase(itePinBackUp);										//ピンのリストからはずす
					listPin.push_back(pPin);												//ネットのピンのリストに追加する
					stringListIterator itePinSignalName = pPin->m_listSigName.begin();		//ピンが持つ信号名をすべて、ネットの信号名に登録
					while(itePinSignalName != pPin->m_listSigName.end()){					//
						if(!quaryExist(listSignalNameAll,(*itePinSignalName))){				//ただし、新規に登録するのは、未登録のものだけ
							listSignalNameAll.push_back(*itePinSignalName);
							listSignalName.push_back(*itePinSignalName);
						}
						itePinSignalName++;
					}
					if(!quaryExist(listPoint,pPin->m_point)){
						listPoint.push_back(pPin->m_point);
					}
				}
    		}// ***** (1)信号名によるピンの抽出 *****
	

			// ***** (2)信号名によるワイヤーの抽出 *****
			XBSchObjListIterator iteWire = m_listWire.begin();
			XBSchObjListIterator iteWireBackUp;
			while(iteWire != m_listWire.end()){				//信号名について全てのワイヤーをスキャンする
				SXBSchWireNl* pWire =(SXBSchWireNl*)(*iteWire);
				iteWireBackUp = iteWire;
				iteWire++;
				if(quaryExist(pWire->m_listSignalName,strSignalName)){						//ワイヤーが持つ信号名のひとつが検索中の信号名に合致
					m_listWire.erase(iteWireBackUp);										//ワイヤーのリストからはずす
					stringListIterator iteWireSignalName = pWire->m_listSignalName.begin();	//ワイヤーが持つ信号名をすべて、ネットの信号名に登録
					while(iteWireSignalName != pWire->m_listSignalName.end()){					//
						if(!quaryExist(listSignalNameAll,(*iteWireSignalName))){	//ただし、新規に登録するのは、未登録のものだけ
							listSignalNameAll.push_back(*iteWireSignalName);
							listSignalName.push_back(*iteWireSignalName);
						}
						iteWireSignalName++;
					}
					if(!quaryExist(listPoint,pWire->p1())){
						listPoint.push_back(pWire->p1());
					}
					if(!quaryExist(listPoint,pWire->p2())){
						listPoint.push_back(pWire->p2());
					}
					delete pWire;
				}
			} // ***** (2)信号名によるワイヤーの抽出 *****
			iteSignalName = listSignalName.erase(iteSignalName);
		}// ここまで ***** １．信号名処理 *****

		itePoint = listPoint.begin();
		// ***** ２．座標処理 *****
		while(itePoint != listPoint.end()){//座標処理繰り返し
			//printf("Point");
			fDone = true;
			SPoint point = (*itePoint);
			// ***** (3)座標によるピンの抽出 *****
			NlPinListIterator itePin = m_listNlPin.begin();
			NlPinListIterator itePinBackUp;
			while(itePin != m_listNlPin.end()){	//座標について全てのピンをスキャンする
				SNlPin* pPin =(*itePin);
				itePinBackUp = itePin;
				itePin++;
				if(pPin->m_point == point){													//ピンの座標が検索中の座標と合致
					m_listNlPin.erase(itePinBackUp);										//ピンのリストからはずす
					listPin.push_back(pPin);												//ネットのピンのリストに追加する
					stringListIterator itePinSignalName = pPin->m_listSigName.begin();		//ピンが持つ信号名をすべて、ネットの信号名に登録
					while(itePinSignalName != pPin->m_listSigName.end()){					//
						if(!quaryExist(listSignalNameAll,(*itePinSignalName))){	//ただし、新規に登録するのは、未登録のものだけ
							listSignalNameAll.push_back(*itePinSignalName);
							listSignalName.push_back(*itePinSignalName);
						}
						itePinSignalName++;
					}
				}
			} //ここまで***** (3)座標によるピンの抽出 *****

			// ***** (4)座標によるワイヤーの抽出 *****
			XBSchObjListIterator iteWire = m_listWire.begin();
			XBSchObjListIterator iteWireBackUp;
			while(iteWire != m_listWire.end()){		//座標について全てのワイヤーをスキャンする
				SXBSchWireNl* pWire =(SXBSchWireNl*)(*iteWire);
				iteWireBackUp = iteWire;
				iteWire++;
				bool fHit=false;
				if(pWire->p1() == point){
					fHit=true;
					if(!quaryExist(listPoint,pWire->p2())){
						listPoint.push_back(pWire->p2());
					}
				}else if(pWire->p2() == point){
					fHit=true;
					if(!quaryExist(listPoint,pWire->p1())){
						listPoint.push_back(pWire->p1());
					}
				}
				if(fHit){
					stringListIterator iteWireSignalName = pWire->m_listSignalName.begin();	//ワイヤーが持つ信号名をすべて、ネットの信号名に登録
					while(iteWireSignalName != pWire->m_listSignalName.end()){				//
						if(!quaryExist(listSignalName,(*iteWireSignalName))){	//ただし、新規に登録するのは、未登録のものだけ
							listSignalNameAll.push_back(*iteWireSignalName);
							listSignalName.push_back(*iteWireSignalName);
						}
						iteWireSignalName++;
					}
					delete pWire;
					m_listWire.erase(iteWireBackUp);										//ワイヤーのリストからはずす
				}
    		}// ***** (4)座標によるワイヤーの抽出 *****
		
			itePoint=listPoint.erase(itePoint);
		}// while(itePoint != listPoint.end()){
		loopCount++ ;
	}

	if(listSignalNameAll.size()>0) pSingleNet->m_SignalName=listSignalNameAll.front();

	pSingleNet->m_nPinCount=listPin.size();
	if(pSingleNet->m_nPinCount>0){
		pSingleNet->m_pPinArray = new PSNlPin[pSingleNet->m_nPinCount];
	
		NlPinListIterator itePin=listPin.begin();
		int n=0;
		while(itePin!=listPin.end()){
			pSingleNet->m_pPinArray[n++]=(*itePin++);
		}
		qsort(pSingleNet->m_pPinArray,pSingleNet->m_nPinCount, sizeof(PSNlPin), CompareSNlPin );
	}
	return pSingleNet;
}




/*
void dumpobj(XBSchObjList* listobj)
{
	SWriteCE3 wce3;
	wce3.Attach(stdout);
	XBSchObjListIterator ite=listobj->begin();
	int count=0;
	while(ite != listobj->end()){
		fprintf(stderr,"%d\n",count++);
		(*ite++)->writeCe3(wce3,NULL);
	}
}
*/

void  SNetList::createNetList(bool bExcludeNC)
{
	jointWire();			//ワイヤーの連結整理
	divideWire();			//ワイヤーを接合点で分割する
	enumPin();				//有効な部品のピンを列挙する
	enumConflictingPin();	//重複ピンを列挙する
	nameFromTagPower();		//タグと電源部品を元に線とピンに名前をつける
	nameFromLabel();		//ラベルを元に線に線名を追加
	char signalName[32];
	int  signalNumber=0;
	SSingleNetList* pSingleNet; 

	while(NULL!=(pSingleNet=NetList1())){
		if(pSingleNet->m_nPinCount<=1){
			if(pSingleNet->m_nPinCount==1){
                SNlPin* pNlNcPin = new SNlPin;
				pNlNcPin->m_strPinNum =	pSingleNet->m_pPinArray[0]->m_strPinNum;
				pNlNcPin->m_strRefNum =	pSingleNet->m_pPinArray[0]->m_strRefNum;
				pNlNcPin->m_point =		pSingleNet->m_pPinArray[0]->m_point;
				m_listNlNcPin.push_back(pNlNcPin);
			}
				
			delete pSingleNet;
			continue;
		}else{
			if(bExcludeNC && (pSingleNet->m_SignalName=="NOCONNECTION")){
				delete pSingleNet;
				continue;
			}
			if(pSingleNet->m_SignalName==""){
				sprintf(signalName,"N%05d",signalNumber++);
				pSingleNet->m_SignalName=string(signalName);
			}
			m_listNet.push_back(pSingleNet);
		}
	}
}

void SNetList::parseRefNum(const string& refnum,string& prefix,int& suffix)
{
	int len = refnum.length();
	int pos = len-1;
	while(pos>=0){
		if(!isdigit(refnum[pos])) break;
		pos--;
	}
	pos++;	//プレフィックスの文字数が得られる
	prefix = refnum.substr(0,pos);
	if(len<=pos)suffix=0;
	else suffix = atoi(refnum.substr(pos,len-pos).c_str());
}



void SNetList::createPartsList()
{
	
	int i;
	int nCount = m_listComponent.size();
	m_pArrayPlistElem = new PSPartsListElement[nCount];
	for(i=0;i<nCount;i++) m_pArrayPlistElem[i]=NULL;

	XBSchObjListIterator ite = m_listComponent.begin();
	
	i=0;
	while(ite != m_listComponent.end()){
		SXBSchComponent* pComp=(SXBSchComponent*)(*ite++);
		string prefix;
		int  suffix;
		parseRefNum(pComp->refnum(),prefix,suffix);
		m_pArrayPlistElem[i]=new SPartsListElement;
		m_pArrayPlistElem[i]->m_prefix = prefix;
		m_pArrayPlistElem[i]->m_suffix = suffix;
		m_pArrayPlistElem[i]->m_name = pComp->name();
		m_pArrayPlistElem[i]->m_note = pComp->note();

		i++;
	}
	qsort(m_pArrayPlistElem,nCount, sizeof(PSPartsListElement), CompareSPartsListElement );
}


void convertToCsvRecord(const string& src,string& dest)
{
	int len= src.size();
	dest="\"";
	for(int i=0;i<len;i++){
		if(src[i]=='\"'){
			dest+="\"\"";
		}else if(src[i]=='\r' || src[i]=='\n'){
			dest+=" ";
		}else{
			dest+=src[i];
		}
	}
	dest+="\"";
}	


//CSV形式の部品リスト出力
int SNetList::writePartsList(FILE* fp)
{
	if(m_pArrayPlistElem==NULL) return -1;
	string name;
	string prefix;
	int suffix;
	string outString;

	int n=m_listComponent.size();
	for(int i=0;i<n;i++){
		if(i>0 && (prefix==m_pArrayPlistElem[i]->m_prefix && suffix==m_pArrayPlistElem[i]->m_suffix)){
			if(name == m_pArrayPlistElem[i]->m_name){
				outString = "*";
			}else{
				outString = "?";
			}
		}else{
			outString = "";
		}
		fprintf(fp,"%s,",outString.c_str());
		convertToCsvRecord(m_pArrayPlistElem[i]->m_prefix,outString);
		fprintf(fp,"%s,%d,",outString.c_str(),m_pArrayPlistElem[i]->m_suffix);
		convertToCsvRecord(m_pArrayPlistElem[i]->m_name,outString);
		fprintf(fp,"%s,",outString.c_str());
		convertToCsvRecord(m_pArrayPlistElem[i]->m_note,outString);
		fprintf(fp,"%s\n",outString.c_str());
		prefix	= m_pArrayPlistElem[i]->m_prefix;
		suffix	= m_pArrayPlistElem[i]->m_suffix;
		name	= m_pArrayPlistElem[i]->m_name;
	}
	return 0;
}

int SNetList::writeReport(FILE* fp)
{
	if(fp==NULL) return -1;
	NlPinListIterator ite;
	int sheet,x,y;

	fprintf(fp,"*** CONFLICTING PINS ***\n");
	ite = m_listNlConfPin.begin();
	while(ite != m_listNlConfPin.end()){
		fprintf(fp,"%s.%s\n",(*ite)->m_strRefNum.c_str(),(*ite)->m_strPinNum.c_str());
		while(ite != m_listNlConfPin.end() && (*ite)!=NULL){
			x=(*ite)->m_point.x();
			y=(*ite)->m_point.y();
			sheet=x/10000+1;
			fprintf(fp,"%d:(%d,%d)\n",sheet,x,y);
			ite++;
		}
		if(ite != m_listNlConfPin.end()) ite++;
	}



	fprintf(fp,"*** UNCONNECTED PINS ***\n");
	int n=0;
	ite = m_listNlNcPin.begin();
	while(ite != m_listNlNcPin.end()){
		if(n==5){
			fprintf(fp,"\n");
			n=0;
		}else if(n>0){
			fprintf(fp," ");
		}
		fprintf(fp,"%s.%s",(*ite)->m_strRefNum.c_str(),(*ite)->m_strPinNum.c_str());
		n++;
		ite++;
	}
	fprintf(fp,"\n\n");

	string signalName;

	XBSchObjListIterator iteTP;
	SXBSchTag* pTag;
	SXBSchComponent* pPC;

	fprintf(fp,"*** UNCONNECTED TAGS ***\n");
	iteTP = m_listNcTagPower.begin();
	while(iteTP != m_listNcTagPower.end()){
		if((*iteTP)->id() == SXBSchObj::ID_TAG){
			pTag = (SXBSchTag*)(*iteTP);
			x=pTag->p1().x();
			y=pTag->p1().y();
			sheet=x/10000+1;
			x=x%10000;
			signalName = pTag->text();
			fprintf(fp,"%d:(%d,%d) %s\n",sheet,x,y,signalName.c_str());
		}
		iteTP++;
	}
	fprintf(fp,"\n");

	fprintf(fp,"*** UNCONNECTED POWER SYMBOLS ***\n");
	iteTP = m_listNcTagPower.begin();
	while(iteTP != m_listNcTagPower.end()){
		if((*iteTP)->id() == SXBSchObj::ID_COMPONENT){
			pPC = (SXBSchComponent*)(*iteTP);
			x=pPC->p1().x();
			y=pPC->p1().y();
			sheet=x/10000+1;
			x=x%10000;
			signalName = pPC->name();			//信号名に使うために部品名を得る
			if(signalName.length() == 0){		//部品名が空白だったら...
				signalName = pPC->orgname();	//代わりにライブラリ部品名を使う
			}
			fprintf(fp,"%d:(%d,%d) %s\n",sheet,x,y,signalName.c_str());
		}
		iteTP++;
	}
	fprintf(fp,"\n");

	fprintf(fp,"*** NO NUMBER PINS ***\n");
	n=0;
	ite = m_listNoNumPin.begin();
	while(ite != m_listNoNumPin.end()){
		if(n==2){
			fprintf(fp,"\n");
			n=0;
		}else if(n>0){
			fprintf(fp," ");
		}
		x=(*ite)->m_point.x();
		y=(*ite)->m_point.y();
		sheet=x/10000+1;
		x=x%10000;
		fprintf(fp,"%s %d:(%d,%d)",(*ite)->m_strRefNum.c_str(),sheet,x,y);
		n++;
		ite++;
	}
	fprintf(fp,"\n\n");



	return 0;
}

//テレシス形式のネットリスト出力
int SNetList::writeTeleNetList(FILE* fp)
{
	string signalname;
	string pinname;
	string name;
	string prefix;
	int suffix;
	
	if(fp==NULL) return -1;
	
	fprintf(fp,"$PACKAGES\n");

	if(m_pArrayPlistElem==NULL) return -1;
	string outString;

	int n=m_listComponent.size();
	for(int i=0;i<n;i++){
		if(    (i>0)
			&& (prefix == m_pArrayPlistElem[i]->m_prefix)
			&& (suffix == m_pArrayPlistElem[i]->m_suffix)
			){
			continue;
		}
		prefix = m_pArrayPlistElem[i]->m_prefix;
		suffix = m_pArrayPlistElem[i]->m_suffix;
		name   = m_pArrayPlistElem[i]->m_name;

		fprintf(fp,"! %s; %s%d\n",name.c_str(),prefix.c_str(),suffix);
	}

	fprintf(fp,"$NETS\n");
	SSingleNetList* pSingleNet;

	NetListIterator iteNet=m_listNet.begin();
	while(iteNet!=m_listNet.end()){
		pSingleNet = (*iteNet++);
		fprintf(fp,"%s;  ",pSingleNet->m_SignalName.c_str());
		int n=0;
		for(int i=0;i< pSingleNet->m_nPinCount;i++){
			if(n==5){
				fprintf(fp," ,\n     ");
				n=0;
			}else if(n>0){
				fprintf(fp," ");
			}
			fprintf(fp,"%s.%s",pSingleNet->m_pPinArray[i]->m_strRefNum.c_str(),pSingleNet->m_pPinArray[i]->m_strPinNum.c_str());
			n++;
		}
		fprintf(fp,"\n");

	}
	fprintf(fp,"$END\n");
	return 0;
}

//PADS形式のネットリスト出力
int SNetList::writePADSNetList(FILE* fp)
{
	string signalname;
	string pinname;
	if(fp==NULL) return -1;
	

	fprintf(fp,"*PADS*\n*NET*\n\n");
	SSingleNetList* pSingleNet;

	NetListIterator iteNet=m_listNet.begin();
	while(iteNet!=m_listNet.end()){
		pSingleNet = (*iteNet++);
		fprintf(fp,"*SIG %s\n",pSingleNet->m_SignalName.c_str());
		int n=0;
		for(int i=0;i< pSingleNet->m_nPinCount;i++){
			if(n==5){
				fprintf(fp,"\n");
				n=0;
			}else if(n>0){
				fprintf(fp,"    ");
			}
			fprintf(fp,"%s.%s",pSingleNet->m_pPinArray[i]->m_strRefNum.c_str(),pSingleNet->m_pPinArray[i]->m_strPinNum.c_str());
			n++;
		}
		fprintf(fp,"\n\n");

	}
	fprintf(fp,"*END*\n");
	return 0;
}





//ネットリストを作成する。
int netlist(int srcfiles, const char* srcfile[],SNetlistOption& option)
{
	SNetList netlist;
	if(option.fpDst==NULL) return -1;

	for(int n=0;n<srcfiles;n++){
		SXBSchDoc* pDoc=new SXBSchDoc;
		fprintf(stderr,"Read %s",srcfile[n]);
		if(pDoc->readFile(srcfile[n])!=0){
			fprintf(stderr,"\n");
			return -3;
		}
		fprintf(stderr,"\n");
		netlist.setData(&(pDoc->m_listMain));
		delete pDoc;
	}
	if(!option.bPlistMode){
		netlist.createPartsList();
		netlist.createNetList(option.bExcludeNC);
		netlist.writeTeleNetList(option.fpDst);
		if(option.fpRpt) netlist.writeReport(option.fpRpt);
	}else{
		netlist.createPartsList();
		netlist.writePartsList(option.fpDst);
	}
	return 0;
}


