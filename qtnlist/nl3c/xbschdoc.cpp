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
//XBSchのドキュメントの管理を行う。

//#include <qapplication.h>
//#include <qwidget.h>
//#include <SSize.h>
//#include <SPoint.h>
//#include <qclipboard.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <list>
using namespace std;
#include "ce3io.h"
#include "xbschobj.h"
#include "xbschjunc.h"
#include "xbschlabel.h"
#include "xbschcomment.h"
#include "xbschtag.h"
#include "xbschcomponent.h"

#include "xbschline.h"
#include "xbschentry.h"
#include "xbschdelobj.h"
#include "xbschsheetinfo.h"
#include "xbschdoc.h"
#include <limits.h>



SXBSchDoc::SXBSchDoc()
{
	m_bDirty = false;
	m_sizeSheet = SSize(1500,1000);
//	m_pView = NULL;
	m_bSelectForDrag = false;
	init();
}

SXBSchDoc::~SXBSchDoc()
{
	init();
}

bool SXBSchDoc::isEmpty()
{
	if( (m_listMain.size() > 0) || (m_listTemp.size() > 0 )) return false;
	else return true;
}


//シートサイズの設定
void SXBSchDoc::SetSheetSize(const SSize& size)
{
	if(size.w()<500||size.w()>3000||size.h()<400||size.h()>2000)return;
	m_sizeSheet = size;
}

//図面要素の追加 操作番号を返す
int SXBSchDoc::add(SXBSchObj* pObj)
{
	if(pObj == NULL) return m_nOpCount;
	m_nOpCount++;
	m_bDirty = true;	
	pObj->setPlaceNum(m_nOpCount);	
	m_listMain.push_back(pObj);
	return m_nOpCount;
}

//選択状態で追加図面要素の追加 操作番号を返す
int  SXBSchDoc::addSelect(SXBSchObj* pObj)
{
	m_listTemp.push_back(pObj);
	setupRcTempArea();
	m_bDirty = true;	
	return m_nOpCount;
}

//点を指定しての選択
//点を指定しての選択では、1つのオブジェクトしか選ばない。
int SXBSchDoc::select(const SPoint& pt)
{
	resetSelect();
	SXBSchObj* pObjSelected = NULL;
	int maxSelectWeight=0;
	XBSchObjListIterator pObjIte = m_listMain.begin();
	while(pObjIte !=  m_listMain.end()){
		if( (*pObjIte)->layer() == m_editLayer
		  &&(*pObjIte)->refObj()==NULL){	//参照オブジェクトが存在するものは無視できる
			int SelectWeight = (*pObjIte)->qSelectWeight(pt) & 0xFFFF;
			if(SelectWeight > maxSelectWeight){
				maxSelectWeight = SelectWeight;
				pObjSelected = (*pObjIte);
			}
		}
		pObjIte++;
	}
	if(pObjSelected != NULL){
		SXBSchObj* pNewObj = pObjSelected->duplicate();
		pObjSelected->setRefObj(pNewObj);
		pNewObj->setRefObj(pObjSelected);
		pNewObj->setSelection(SXBSchObj::SELECT_ALL);
		m_listTemp.push_back(pNewObj);
	}
	setupRcTempArea();
	setupRotateRect();
	m_bSelectForDrag = false;
	return m_listTemp.size();
}

//範囲を指定しての選択
//選択とは、
//  メインのリストから該当するものを見つけ、
//  複製を一時オブジェクトに追加する
//選択した数を返す
int SXBSchDoc::select(const SRect& rc)
{
	resetSelect();
	XBSchObjListIterator pObjIte = m_listMain.begin();
	while(pObjIte != m_listMain.end()){
		if( (*pObjIte)->layer() == m_editLayer
		 && (*pObjIte)->refObj()==NULL){	//参照オブジェクトが存在するものは無視できる
			unsigned nSelStatus = (*pObjIte)->testSelection(rc);
			if(nSelStatus){
				SXBSchObj* pNewObj = (*pObjIte)->duplicate();
				(*pObjIte)->setRefObj(pNewObj);
				pNewObj->setRefObj(*pObjIte);
				pNewObj->setSelection(nSelStatus);
				m_listTemp.push_back(pNewObj);
			}
		}
		pObjIte++;
	}
	setupRcTempArea();
	setupRotateRect();

	m_bSelectForDrag = false;
	return m_listTemp.size();
}



bool SXBSchDoc::resetSelect_1(bool bDel)
{
	//一時オブジェクトのリストが空なら何もしない
	if(m_listTemp.size() == 0) return false;

	m_nOpCount++;
	m_bDirty = true;	

	XBSchObjListIterator pObjIte = m_listTemp.begin();
	while(pObjIte != m_listTemp.end()){
		if((*pObjIte)->refObj()){ //一時オブジェクトが参照オブジェクトを持つ場合、参照オブジェクトは元のデータを指す。
			SXBSchObj* pOldObj = (*pObjIte)->refObj();
			//削除オブジェクトの追加
			SXBSchDelObj* pDelObj = new SXBSchDelObj;
			//qDebug("Add DeleteObj");
			pDelObj->setPlaceNum(m_nOpCount);	
			pDelObj->setRefObj(pOldObj); //削除オブジェクトのrefObj()には対象オブジェクトへのポインタをセットする
			pOldObj->setRefObj(pDelObj); //元のオブジェクトには削除オブジェクトへのポインタをセットする
			m_listMain.push_back(pDelObj);
		}
		if(!bDel){
			//一時データをメインのリストに追加
			if((*pObjIte)->qValid()){
				(*pObjIte)->setPlaceNum(m_nOpCount);		//操作番号を更新する
				(*pObjIte)->setRefObj(NULL);				//参照オブジェクトは空にする（有効データになる）
				(*pObjIte)->resetSelection();				//選択フラグをクリアする。
				m_listMain.push_back(*pObjIte);
			}else{
				delete (*pObjIte);
			}
		}else{
			//一時データを消去
			delete (*pObjIte);
		}
		pObjIte++;
	}
	m_listTemp.clear();
	m_bSelectForDrag = false;

	delOldData(m_nOpCount-10);		//Undo回数の制限

	return true;
}

//m_rcTempArea と m_ptTempSnapPoint を一時オブジェクトのリストから設定する。
void SXBSchDoc::setupRcTempArea()
{
	if(m_listTemp.size() == 0)return;
	m_rcTempArea = SRect(SPoint(INT_MAX,INT_MAX),SPoint(INT_MIN,INT_MIN));

	XBSchObjListIterator pObjIte = m_listTemp.begin();
	bool bFind1st = true;
	while(pObjIte != m_listTemp.end()){
		m_rcTempArea |= (*pObjIte)->area();
		if(bFind1st){
			m_ptTempSnapPoint = (*pObjIte)->snapPoint();
			bFind1st = false;
		}
		pObjIte++;
	}
}

void SXBSchDoc::setupRotateRect()
{
	if(m_listTemp.size() == 0)return;
	m_rcRotateRect = SRect(SPoint(INT_MAX,INT_MAX),SPoint(INT_MIN,INT_MIN));

	XBSchObjListIterator pObjIte = m_listTemp.begin();
	while(pObjIte != m_listTemp.end()){
		m_rcRotateRect |= (*pObjIte)->rotateRect();
		pObjIte++;
	}
	//qDebug("%d,%d\n",m_rcRotateRect.l(),m_rcRotateRect.r());
}



bool SXBSchDoc::writeMinimumLb3(SWriteCE3& wce3,XBSchObjList* pList,bool bOwnOnly)
{
//typedef list<SCompIndex*> SCompIndexList;
//typedef list<SCompIndex*>::iterator SCompIndexListIterator;

	return true;
}


bool SXBSchDoc::writeCe3(SWriteCE3& wce3,XBSchObjList* pList,bool bAllObj,const SPoint* pptOrigin)
{
	SRect rcSheet(0,0,m_sizeSheet.w(),m_sizeSheet.h());
	if(pList){
		XBSchObjListIterator pObjIte = pList->begin();
		while(pObjIte != pList->end()){
			if(bAllObj || !(*pObjIte)->refObj()){
				if(rcSheet.intersect((*pObjIte)->area())){
					(*pObjIte)->writeCe3(wce3,pptOrigin);
				}
			}
			pObjIte++;
		}
	}
	return true;
}


bool SXBSchDoc::readCe3(SReadCE3& rce3,XBSchObjList* pList,const SPoint* pptOrigin,SXBSchSheetInfo* pSheetInfo)
{

	assert(pList!=NULL);
	string str;
	bool bFirstSheetInfo = true;
	SXBSchObj* pObj;
	while(rce3.ReadRecord(str)!=EOF){
		if(str == "") break;		//空のレコードは無視する
		pObj = NULL;
		if(str == "+SHEETINFO" && bFirstSheetInfo && pSheetInfo){
			bFirstSheetInfo = false;
			if(! pSheetInfo->readCe3(rce3,pptOrigin,this)) return false;
		}else if(str == "+JUNCTION"){
				pObj = new SXBSchJunc;
		}else if(str == "+LABEL"){
				pObj = new SXBSchLabel;
		}else if(str == "+COMMENT"){
				pObj = new SXBSchComment;
		}else if(str == "+TAG"){
				pObj = new SXBSchTag;
		}else if(str == "+WIRE"){
				pObj = new SXBSchWire;
		}else if(str == "+BUS"){
				pObj = new SXBSchBus;
		}else if(str == "+DASH"){
				pObj = new SXBSchDash;
		}else if(str == "+ENTRY"){
				pObj = new SXBSchEntry;
		}else if(str == "+BENTRY"){
				pObj = new SXBSchBusEntry;
		}else if(str == "+COMPONENT"){
				pObj = new SXBSchComponent;
		}else if(str == "-BSCH3_DATA_V.1.0"){
			return true;
		}else{
			if(str[0] != '+') return false;
			str[0]='-';
			if(rce3.SkipTo(str)==EOF) return false;
			//if(rce3.SkipTo(str.replace(1,1,"-")) != SCsvIStream::GETRECORD_NORMAL)return false;
		}
		if(pObj){
			if(pObj->readCe3(rce3,pptOrigin,this)){
				if(pObj->qValid()){
					pList->push_back(pObj);
				}else{
					delete pObj;
				}
			}else{
				delete pObj;
				return false;
			}
		}
	}		
	return true;
}





//選択の解除
//選択の解除とは、
//  一時データが指す元のオブジェクトを指す「削除オブジェクト」をメインのリストに追加し
//                      一時データをメインのリストに追加すること
bool SXBSchDoc::resetSelect()
{
	return resetSelect_1(false);
}


//初期化
void SXBSchDoc::init()
{
	m_bDirty = false;
	m_nOpCount = 0;
	m_nOldestOpCount = 0;
	m_visibleLayer =0xFF;
	m_editLayer = 0;
	

	XBSchObjListIterator pObjIte;

	pObjIte = m_listMain.begin();

	while(pObjIte != m_listMain.end()){
		delete (*pObjIte);
		pObjIte++;
	}
	m_listMain.clear();

 	pObjIte = m_listTemp.begin();
	while(pObjIte != m_listTemp.end()){
		delete (*pObjIte);
		pObjIte++;
	}
	m_listTemp.clear();

//printf("\n");
//printf("SXBSchDoc::init() m_listMain.count()=%d,m_listTemp.count()=%d\n",m_listMain.count(),m_listTemp.count());
}

//ファイルからの読み込み
//正常終了で0を返す
//  1:ファイル読み込みエラー
//  2:BSCH3ファイルではない
int SXBSchDoc::readFile(FILE* fp)
{
	if(fp ==NULL) return 1;
	SReadCE3 rce3;
	rce3.Attach(fp);

	string str;
	if(rce3.ReadRecord(str)==EOF){	//空のファイルのときの処理
		fclose(fp);
		return 0;
	}
	SXBSchSheetInfo sheetInfo;
	if(str != "+BSCH3_DATA_V.1.0") return 2;
	if(readCe3(rce3,&m_listMain,NULL,&sheetInfo)){
		m_sizeSheet.setW(sheetInfo.width());
		m_sizeSheet.setH(sheetInfo.height());
		m_editLayer = sheetInfo.editLayer();
		m_visibleLayer = sheetInfo.visbleLayer();
		return 0;
	}else{
		fclose(fp);
		return 1;
	}
}


int SXBSchDoc::readFile(const char* fname)
{
	init();
	m_bDirty = false;
	FILE* fp;
	if((fp = fopen(fname,"r"))==NULL) return 1;
	int retval = readFile(fp);
	fclose(fp);
	return retval;
}


int SXBSchDoc::writeFile(FILE* fp)
{
	if(fp ==NULL) return 1;
	SWriteCE3 wce3;
	wce3.Attach(fp);
	SXBSchSheetInfo sheetInfo;
	sheetInfo.setWidth( m_sizeSheet.w() );
	sheetInfo.setHeight( m_sizeSheet.h() );
	sheetInfo.setEditLayer(m_editLayer);
	sheetInfo.setVisbleLayer(m_visibleLayer);

	wce3.WriteRecord("+BSCH3_DATA_V.1.0");
	wce3.WriteEOL();
	sheetInfo.writeCe3(wce3,NULL);
	writeCe3(wce3,&m_listMain,false,NULL);
	wce3.WriteRecord("-BSCH3_DATA_V.1.0");
	if(wce3.WriteEOL()==EOF){
		return 1;
	}else{		
		return 0;
	}
}

//ファイルへの書き込み
//正常終了で0を返す
int SXBSchDoc::writeFile(const char* fname)
{
	resetSelect();
	FILE* fp;
	if((fp = fopen(fname,"w"))==NULL) return 1;
	int retval = writeFile(fp);
	fclose(fp);
	if(retval==0){
		init();
		readFile(fname);
		m_bDirty = false;
	}
	return retval;
}

/*
// ***** Qt依存 *****
//コピー
//コピーが行われたときにtrueを返す
bool SXBSchDoc::copy()
{
	if(!m_listTemp.size())return false;
	SPoint ptOrigin = SPoint(m_rcTempArea.l(),m_rcTempArea.t());
	ptOrigin.setX(((ptOrigin.x()+5)/10)*10);
	ptOrigin.setY(((ptOrigin.y()+5)/10)*10);

	string str;
	SWriteCE3 wce3;
	wce3.Attach(&str);

	wce3.WriteRecord("+BSCH3_DATA_V.1.0");
	wce3.WriteEOL();
	writeCe3(wce3,&m_listTemp,true,&ptOrigin);
	wce3.WriteRecord("-BSCH3_DATA_V.1.0");
	wce3.WriteEOL();

    QClipboard *cb = QApplication::clipboard();
    cb->setText(QString::fromLocal8Bit(str.c_str()));
	
	return true;
}
*/

//削除
//ドキュメントに変更があったときtrueを返す
bool SXBSchDoc::del()
{
	m_bDirty = true;	
	return resetSelect_1(true);
}

//切り取り
//ドキュメントに変更があったときtrueを返す
bool SXBSchDoc::cut()
{
	if(copy()){
		return del();
	}
	return false;
}

/*
// ***** Qt依存 *****
//貼り付け
//ドキュメントに変更があったときtrueを返す
bool SXBSchDoc::paste(const SPoint& pt)
{
	if(!canPaste()) return false;
    QClipboard *cb = QApplication::clipboard();
	string cbbuff=string(cb->text().local8Bit());

	SReadCE3 rce3;
	rce3.Attach(cbbuff.c_str());

	string str;

	rce3.ReadRecord(str);
	if(str != "+BSCH3_DATA_V.1.0") return false;

	readCe3(rce3,&m_listTemp,&pt,NULL);
	if(!m_listTemp.size()) return false;
	
	setupRcTempArea();

	m_bDirty = true;	
	return true;
}
*/


//Undo
//ドキュメントに変更があったときtrueを返す
bool SXBSchDoc::undo()
{
//printf("SXBSchDoc::undo() %d %d\n",m_nOpCount, m_nOldestOpCount);
	bool bResult = 	resetSelect();
	if(m_nOpCount <= m_nOldestOpCount){
		return bResult;
	}

	int nCount = m_listMain.size();
	if(nCount==0) return bResult;

	XBSchObjListIterator pObjIte =m_listMain.begin();
	while(nCount && pObjIte!=m_listMain.end()){
		if((*pObjIte)->placeNum() == m_nOpCount){ //Undo対象のオブジェクトなら...
			if((*pObjIte)->id() == SXBSchObj::ID_DELETEOBJ){		//   もしそれが、削除オブジェクトなら
				(*pObjIte)->refObj()->setRefObj(NULL);				//		削除オブジェクトの参照しているオブジェクトが削除前のもの
			}														//   なので、それを復活させる
			delete (*pObjIte);										//Undo対象のオブジェクトを消去
			pObjIte = m_listMain.erase(pObjIte);					//erase()でカレントノードは一つ進む
		}else{
			pObjIte++;
		}
		nCount--;
	}

	m_nOpCount--;
	return true;
}

//Attribute変更が可能か?
bool SXBSchDoc::canAttribute()
{
	int n=m_listTemp.size();
	if(n!=1) return false;
	XBSchObjListIterator pObjIte =m_listTemp.begin();
	return (*pObjIte)->canAttribute();
}

//コピー可能か？
bool SXBSchDoc::canCopy()
{
	return (m_listTemp.size()>0);
}

//削除可能か？
bool SXBSchDoc::canDel()
{
	return (m_listTemp.size()>0);
}

//切り取り可能か？
bool SXBSchDoc::canCut()
{
	return (m_listTemp.size()>0);
}

/*
//貼り付け可能か？
bool SXBSchDoc::canPaste()
{
	QString strID("+BSCH3_DATA_V.1.0");
	QClipboard *cb = QApplication::clipboard();
    QString str = cb->text().left(strID.length());	
	return (str == strID);
}
*/

//Undo可能か？
bool SXBSchDoc::canUndo()
{
	return (m_nOpCount > m_nOldestOpCount);
}



//Rotate
bool SXBSchDoc::rotate()
{
	if(!canRotate()) return false;

	setupRotateRect();

	int n = m_listTemp.size();
	XBSchObjListIterator pObjIte = m_listTemp.begin();
	if(n == 1){
		(*pObjIte)->rotate();
	}else{
		SPoint ptCenter = m_rcRotateRect.center();
		ptCenter.setX(((ptCenter.x()+5)/10)*10);
		ptCenter.setY(((ptCenter.y()+5)/10)*10);
		//int x=(m_rcRotateRect.l()+m_rcRotateRect.r())/2;
		//int y=(m_rcRotateRect.t()+m_rcRotateRect.bottom())/2;
		//SPoint ptCenter(x,y);
		while(pObjIte != m_listTemp.end()){
			(*pObjIte)->rotate(ptCenter);
			pObjIte++;
		}
	}
	setupRcTempArea();

	m_bDirty = true;	
	return true;
}

//レイヤーの移動
bool SXBSchDoc::moveToLayer(int nLayer)
{
	XBSchObjListIterator pObjIte = m_listTemp.begin();
	while(pObjIte != m_listTemp.end()){
		(*pObjIte)->setLayer(nLayer);
		pObjIte++;
	}
	m_bDirty = true;	
	return true;
}


//Mirror
bool SXBSchDoc::mirror()
{
	if(!canMirror()) return false;

	setupRotateRect();

	int n = m_listTemp.size();
	XBSchObjListIterator pObjIte = m_listTemp.begin();
	if(n == 1){
		(*pObjIte)->mirrorH();
	}else{
		int nCenter = (m_rcRotateRect.l()+m_rcRotateRect.r())/2;
		while(pObjIte != m_listTemp.end()){
			(*pObjIte)->mirrorH(nCenter);
			pObjIte++;
		}
	}
	setupRcTempArea();
	m_bDirty = true;	
	return true;
}


//Rotate可能か？
bool SXBSchDoc::canRotate()
{
	if(m_listTemp.size() == 0) return false;
	if(m_bSelectForDrag) return false;
	return true;
}

//Mirror可能か？
bool SXBSchDoc::canMirror()
{
	if(m_listTemp.size() == 0) return false;
	if(m_bSelectForDrag) return false;
	return true;
}



//古い無効データの削除
//   メインのリストの先頭からスキャンして、指定した値の操作カウント値
// より小さな値での削除を行ったオブジェクトを消去する。
//   削除したオブジェクトとそれに対応する削除オブジェクトのリストでの
// 位置に着目すると、かならず、削除したオブジェクトの方が先頭に近いと
// ころに存在する。
void SXBSchDoc::delOldData(int nOpCount)
{
	bool bDel;
	resetSelect();
	if(nOpCount <= m_nOldestOpCount) return;
	
	int nDelObj=0;
	int nDelDelObj=0;

	XBSchObjListIterator pObjIte = m_listMain.begin();
	while(pObjIte!=m_listMain.end()){
		bDel = false;
		if((*pObjIte)->id() != SXBSchObj::ID_DELETEOBJ){
			if((*pObjIte)->refObj()!=NULL){
				SXBSchObj* pDelObj = (*pObjIte)->refObj();
				if(pDelObj->placeNum() < nOpCount){
					bDel = true;
					nDelObj++;
				}
			}
		}else{
			if((*pObjIte)->placeNum() < nOpCount){
				bDel = true;
				nDelDelObj++;
			}
		}
		if(bDel){
			SXBSchObj* pDelObj = *pObjIte;
			pObjIte = m_listMain.erase(pObjIte);	//erase()でカレントノードは一つ進む
			delete pDelObj;
			//delete (*pObjIte);	//Undo対象のオブジェクトを消去
		}else{
			pObjIte++;
		}
	}
	if(nOpCount > m_nOpCount) nOpCount = m_nOpCount;
	m_nOldestOpCount = nOpCount;

	//qDebug("delOldData(%d)   %d   %d",nOpCount,nDelObj,nDelDelObj);

	return;
}

//指定した座標が一時データの上かどうか
int SXBSchDoc::isOnTempData(const SPoint& pt)
{
	if(m_listTemp.size() == 0) return 0;
	if(m_listTemp.size() == 1){
		XBSchObjListIterator pObjIte = m_listTemp.begin();
		int val= (*pObjIte)->qSelectWeight(pt) & (ON_OBJ|ON_NAME|ON_NUM);
		if(val)return val;
	}
	if(m_rcTempArea.intersect(pt)) 	return ON_OBJ;
	return 0;
}

//一時データを移動する
void SXBSchDoc::moveTempData(int offsetx,int offsety)
{
	XBSchObjListIterator pObjIte = m_listTemp.begin();
	while(pObjIte != m_listTemp.end()){
		(*pObjIte)->move(offsetx,offsety);
		pObjIte++;
	}
	m_rcTempArea+=SPoint(offsetx,offsety);
	m_ptTempSnapPoint.setX(m_ptTempSnapPoint.x()+offsetx);
	m_ptTempSnapPoint.setY(m_ptTempSnapPoint.y()+offsety);
	m_bDirty = true;	
}

//範囲内の接合点の選択。すべての接合点を m_listWorkJunc に抽出する。
//
void SXBSchDoc::selectForDragJunction(int scanMax,const SRect& rc)
{
	XBSchObjListIterator pObjIte = m_listMain.begin();
	while(scanMax>0 && pObjIte != m_listMain.end()){
		if(   (*pObjIte)->layer() == m_editLayer
		   && (*pObjIte)->id()==SXBSchObj::ID_JUNCTION 
		   && (*pObjIte)->refObj()==NULL){	//参照オブジェクトが存在するものは無視できる
			unsigned nSelStatus = (*pObjIte)->testSelection(rc);
			if(nSelStatus){
				SXBSchObj* pNewObj = (*pObjIte)->duplicate();	//選択オブジェクトの複製
				(*pObjIte)->setRefObj(pNewObj);					//旧オブジェクトに複製オブジェクトを設定
				pNewObj->setRefObj(*pObjIte);					//新オブジェクトに元オブジェクトを設定
				pNewObj->setSelection(nSelStatus);				//新オブジェクトに選択フラグを設定
				m_listTemp.push_back(pNewObj);					//新オブジェクトを一時データに保管
			}
		}
		*pObjIte++;
		scanMax--;
	}
}

//すべての接合点を m_listWorkJunc に抽出する。
void SXBSchDoc::pickupJunction(const SRect& )
{
	XBSchObjListIterator pObjIte = m_listMain.begin();
	while(pObjIte != m_listMain.end()){
		if(    (*pObjIte)->layer() == m_editLayer  
			&& (*pObjIte)->id()==SXBSchObj::ID_JUNCTION 
			&& (*pObjIte)->refObj()==NULL){	//参照オブジェクトが存在するものは無視できる
			m_listWorkJunc.push_back(*pObjIte);					//選択外の接合点を作業エリアに保管
		}
		*pObjIte++;
	}
}



//ドラッグのための接合点ワイヤー以外の選択
void SXBSchDoc::selectForDragExceptJuncWire(int scanMax,const SRect& rc)
{
	int scan=0;
	XBSchObjListIterator pObjIte = m_listMain.begin();
	while(scan<scanMax && pObjIte != m_listMain.end()){
		if( (*pObjIte)->layer() == m_editLayer
		  &&(*pObjIte)->id()!=SXBSchObj::ID_JUNCTION 
		  &&(*pObjIte)->id()!=SXBSchObj::ID_WIRE
		  &&(*pObjIte)->refObj()==NULL){
			unsigned nSelStatus = (*pObjIte)->testSelectionForDrag(rc);
			if(nSelStatus){
				SXBSchObj* pNewObj = (*pObjIte)->duplicate();	//選択オブジェクトの複製
				(*pObjIte)->setRefObj(pNewObj);					//旧オブジェクトに複製オブジェクトを設定
				pNewObj->setRefObj(*pObjIte);					//新オブジェクトに元オブジェクトを設定
				pNewObj->setSelection(nSelStatus);				//新オブジェクトに選択フラグを設定
				m_listTemp.push_back(pNewObj);					//新オブジェクトを一時データに保管
				int pincount=pNewObj->pinCount();
				//qDebug("SXBSchDoc::selectForDragExceptJuncWire()  PINCOUNT=%d",pincount);
				for(int n=0;n<pincount;n++){
					SPoint pt=pNewObj->pinPosition(n);
					if(!rc.intersect(pt)){
						SRect rcPin(pt,pt);
						selectForDragJunction(scanMax,rcPin);
						selectForDragWire(scanMax,rcPin);
					}
				}
			}
		}
		*pObjIte++;
		scan++;
	}
}


//ドラッグのためのワイヤー選択。
void SXBSchDoc::selectForDragWire(int scanMax,const SRect& rc)
{
	SXBSchWire* pNewWire[5];

	int workWireCount= m_listWorkWire.size();	//m_listWorkWireは増えるが、同じ rcで追加したものは再スキャンしない。
												//selectForDragで最初の範囲指定でのワイヤー選択ではworkWireCountは0になる。

	XBSchObjListIterator pObjIte = m_listMain.begin();
	while(scanMax>0 && pObjIte != m_listMain.end()){
		if( (*pObjIte)->layer() == m_editLayer
		  &&(*pObjIte)->id()==SXBSchObj::ID_WIRE 
		  &&(*pObjIte)->refObj()==NULL){	//参照オブジェクトが存在するものは無視できる
			SXBSchWire* pObj= (SXBSchWire*)(*pObjIte);
			int divcount = divideWire(pObj,rc,pNewWire);
			if(divcount>0){
				SXBSchDelObj* pDelObj = new SXBSchDelObj;		//旧オブジェクトを消去するための消去オブジェクトを作成
				(*pObjIte)->setRefObj(pDelObj);					//旧オブジェクトに消去オブジェクトを設定
				pDelObj->setRefObj(*pObjIte);					//消去オブジェクトに旧オブジェクトを設定
				pDelObj->setPlaceNum(m_nOpCount+1);				
				m_listMain.push_back(pDelObj);					//消去オブジェクトをメインデータに保管
				for(int n=0;n<divcount;n++){
					m_listWorkWire.push_back(pNewWire[n]);
				}
			}
		}
		*pObjIte++;
		scanMax--;
	}

	pObjIte = m_listWorkWire.begin();
	while(workWireCount>0 && pObjIte != m_listWorkWire.end()){
		if((*pObjIte)->refObj()==NULL){	//参照オブジェクトが存在するものは無視できる
			SXBSchWire* pObj= (SXBSchWire*)(*pObjIte);
			int divcount = divideWire(pObj,rc,pNewWire);
			if(divcount>0){
				(*pObjIte)->setRefObj(pNewWire[0]);
				for(int n=0;n<divcount;n++){
					m_listWorkWire.push_back(pNewWire[n]);
				}
			}
		}
		*pObjIte++;
		workWireCount--;
	}

}


//ドラッグ選択ではワイヤーの複製はduplicate()ではなく、divideWire()で行う。
// 複製されたオブジェクトには選択フラグだけが設定される。
//ワイヤーがドラッグ対象でない場合は複製しない。
//ドラッグ対象と非ドラッグ対象に分割する場合はドラッグ対象のものを先頭に格納する。
int SXBSchDoc::divideWire(SXBSchWire* swire,const SRect& rc,SXBSchWire** dwire)
{
	int retval=0;
	bool vFlag;
	int fixval,jfix,jp;
	int p0,p1,p2,p3,p4,p5;
	int	boundaryMin,boundaryMax;
	unsigned int nSelStatus;
	unsigned int nOldSelStatus;

	if(!swire->qValid()) return 0;

	nOldSelStatus = (swire->selection() & (SXBSchObj::SELECT_P1|SXBSchObj::SELECT_P2));
	nSelStatus = swire->testSelectionForDrag(rc);

	//一度選択したものを再スキャンするする場合のチェック
	if(nOldSelStatus){
		swire->setSelection(nOldSelStatus | nSelStatus);
		return 0;
	}

	nSelStatus = swire->testSelectionForDrag(rc);
	if((nSelStatus&(SXBSchObj::SELECT_P1|SXBSchObj::SELECT_P2)) ==SXBSchObj::SELECT_ALL){	//両端点が範囲内のとき
		dwire[0] = (SXBSchWire*)(swire->duplicate());
		dwire[0]->setSelection(nSelStatus);
		return 1;
	}
	if(swire->x1()==swire->x2()){	//垂直線のとき
		if(swire->x1()<rc.l() || swire->x1()>rc.r()) return 0;	//左右範囲外
		if(swire->y1()<rc.t() && swire->y2()<rc.t()) return 0;	//上に範囲外
		if(swire->y1()>rc.b() && swire->y2()>rc.b()) return 0;	//下に範囲外
		vFlag = true;
		fixval = swire->x1();
		p0 = swire->y1();
		p5 = swire->y2();
		boundaryMin = rc.t();
		boundaryMax = rc.b();
	}else if(swire->y1()==swire->y2()){	//水平線のとき
		if(swire->y1()<rc.t() || swire->y1()>rc.b()) return 0;	//上下範囲外
		if(swire->x1()<rc.l() && swire->x2()<rc.l()) return 0;	//左に範囲外
		if(swire->x1()>rc.r() && swire->x2()>rc.r()) return 0;	//右に範囲外
		vFlag = false;
		fixval = swire->y1();
		p0 = swire->x1();
		p5 = swire->x2();
		boundaryMin = rc.l();
		boundaryMax = rc.r();
	}else{	//斜め線のときは分割しない。
		if(nSelStatus!=0){
			dwire[0]=(SXBSchWire*)(swire->duplicate());
			dwire[0]->setSelection(nSelStatus);
			return 1;
		}else{
			return 0;
		}
	}
	if(p0>p5){ int i=p5; p5=p0; p0=i; }

	p3=p1=p0;
	p2=p4=p5;

	if(p0>=boundaryMin) p2=p0;
	if(p5<=boundaryMax) p3=p5;
	XBSchObjListIterator pObjIte = m_listWorkJunc.begin();
	while(pObjIte != m_listWorkJunc.end()){
		SXBSchJunc* pJunc = (SXBSchJunc*)(*pObjIte);
		if(vFlag){  jfix = pJunc->x1();	jp	 = pJunc->y1();	}
		else{		jfix = pJunc->y1();	jp	 = pJunc->x1();	}
		if(jfix==fixval && jp>p1 && jp<p4){
			if(jp<boundaryMin){			//p1更新チェック(boundaryMinより小さくて最大)
				p1=jp;
			}else if(jp>boundaryMax){	//p4更新チェック(boundaryMaxより大きくて最小)
				p4=jp;
			}else{	//接合点は範囲内
				if(jp>p3) p3=jp;
				if(jp<p2) p2=jp;
			}
		}
		pObjIte++;
	}
	if(p2>p3){//範囲内に端点も接合点もない場合は分割しない
		return 0;
	}
	if(p2<p3){//範囲内に両端点を持つワイヤーがある
		dwire[retval]=new SXBSchWire;
		if(vFlag){
			dwire[retval]->setP1(SPoint(fixval,p2));
			dwire[retval]->setP2(SPoint(fixval,p3));
		}else{
			dwire[retval]->setP1(SPoint(p2,fixval));
			dwire[retval]->setP2(SPoint(p3,fixval));
		}
		dwire[retval]->setSelection(SXBSchObj::SELECT_ALL);
		retval++;
	}
	if(p1<p2){//範囲内にP2端点を持つワイヤーがある
		dwire[retval]=new SXBSchWire;
		if(vFlag){
			dwire[retval]->setP1(SPoint(fixval,p1));
			dwire[retval]->setP2(SPoint(fixval,p2));
		}else{
			dwire[retval]->setP1(SPoint(p1,fixval));
			dwire[retval]->setP2(SPoint(p2,fixval));
		}
		dwire[retval]->setSelection(SXBSchObj::SELECT_P2);
		retval++;
	}
	if(p3<p4){//範囲内にP1端点を持つワイヤーがある
		dwire[retval]=new SXBSchWire;
		if(vFlag){
			dwire[retval]->setP1(SPoint(fixval,p3));
			dwire[retval]->setP2(SPoint(fixval,p4));
		}else{
			dwire[retval]->setP1(SPoint(p3,fixval));
			dwire[retval]->setP2(SPoint(p4,fixval));
		}
		dwire[retval]->setSelection(SXBSchObj::SELECT_P1);
		retval++;
	}
	if(p0<p1){//範囲外にワイヤーがある
		dwire[retval]=new SXBSchWire;
		if(vFlag){
			dwire[retval]->setP1(SPoint(fixval,p0));
			dwire[retval]->setP2(SPoint(fixval,p1));
		}else{
			dwire[retval]->setP1(SPoint(p0,fixval));
			dwire[retval]->setP2(SPoint(p1,fixval));
		}
		retval++;
	}
	if(p4<p5){//範囲外にワイヤーがある
		dwire[retval]=new SXBSchWire;
		if(vFlag){
			dwire[retval]->setP1(SPoint(fixval,p4));
			dwire[retval]->setP2(SPoint(fixval,p5));
		}else{
			dwire[retval]->setP1(SPoint(p4,fixval));
			dwire[retval]->setP2(SPoint(p5,fixval));
		}
		retval++;
	}
	return retval;
}


int SXBSchDoc::selectForDrag(const SRect& rc)
{
	//一時データの解放
	resetSelect();
	m_listWorkJunc.clear();
	m_listWorkWire.clear();

	int scanMax = m_listMain.size();

	//接合点の選択と抽出
	pickupJunction(rc);
	selectForDragJunction(scanMax,rc);

	//ワイヤーの選択
	//範囲内外にまたがるものはジャンクションによる分解を行う
	selectForDragWire(scanMax,rc);

	//接合点、ワイヤー以外の選択
	selectForDragExceptJuncWire(scanMax,rc);

	XBSchObjListIterator pObjIte = m_listWorkWire.begin();
	while(pObjIte != m_listWorkWire.end()){
		if((*pObjIte)->refObj()==NULL){	//参照オブジェクトがないものだけが有効
			if((*pObjIte)->selection()!=0){	//選択フラグが付いているものは一時データにコピー
				m_listTemp.push_back(*pObjIte);
			}else{
				(*pObjIte)->setPlaceNum(m_nOpCount+1);
				m_listMain.push_back(*pObjIte);
			}
		}else{
			delete(*pObjIte);
		}
		pObjIte++;
	}

	
	setupRcTempArea();
	setupRotateRect();
	m_listWorkJunc.clear();
	m_listWorkWire.clear();
	m_bSelectForDrag = true;
	return m_listTemp.size();
}
