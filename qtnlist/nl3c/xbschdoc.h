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
#ifndef XBSCHDOC_H
#define XBSCHDOC_H


#include <list>
using namespace std;

#include "xbschobj.h"
#include "ce3io.h"

//typedef list<string> stringList;
//typedef list<string>::iterator stringListIterator;

typedef list<SXBSchObj*> XBSchObjList;
typedef list<SXBSchObj*>::iterator XBSchObjListIterator;


class SXBSchSheetInfo;
class SXBSchWire;



class SXBSchDoc
{
public:
	SXBSchDoc();
	virtual ~SXBSchDoc();


protected:
	bool m_bDirty;		//ドキュメントに対して変更が発生したときtrueになる。
	SSize m_sizeSheet;	//シートのサイズを保存する
	int m_nOpCount;				//操作番号
	int m_nOldestOpCount;		//もっとも古い操作番号
	SRect m_rcTempArea;			//	
	SRect m_rcRotateRect;		//回転や反転で中心位置を求めるためのエリア
	SPoint m_ptTempSnapPoint;	//一時オブジェクトをスナップさせる場合はこの点がグリッドに乗るようにする
	bool m_bSelectForDrag;
	unsigned m_visibleLayer;
	int m_editLayer;



protected:
	//選択の解除 bDel が true のときは、削除を行う
	//public の resetSelect() と del() から呼ばれる。
	//ドキュメントに変更があったときtrueを返す
	bool resetSelect_1(bool bDel=false);

	void pickupJunction(const SRect& rc);//すべての接合点を m_listWorkJunc に抽出する。
	void selectForDragJunction(int scanMax,const SRect& rc);	//範囲内の接合点の選択。すべての接合点をm_listWorkJunc に抽出する。
	int divideWire(SXBSchWire* swire,const SRect& rc,SXBSchWire** dwire);
	void selectForDragWire(int scanMax,const SRect& rc);		//ドラッグのためのワイヤー選択。
	void selectForDragExceptJuncWire(int scanMax,const SRect& rc);//ドラッグのための接合点ワイヤー以外の選択。
	
	

public:  //ストリームI/O関連
	bool writeMinimumLb3(SWriteCE3& wce3,XBSchObjList* pList,bool bOwnOnly);
	bool writeCe3(SWriteCE3& wce3,XBSchObjList* pList,bool bAllObj,const SPoint* pptOrigin);
	bool readCe3(SReadCE3& rce3,XBSchObjList* pList,const SPoint* pptOrigin,SXBSchSheetInfo* pSheetInfo);



public:
	XBSchObjList m_listMain;
	XBSchObjList m_listTemp;
protected:
	XBSchObjList m_listWorkJunc;	//一時作業用のワークエリア ドラッグ選択で使用
	XBSchObjList m_listWorkWire;	//一時作業用のワークエリア ドラッグ選択で使用


public:
	bool isDirty(){return m_bDirty;}	//最後の保存のあとに変更が入ったかどうかを返す
	bool isEmpty();	//図面オブジェクトが一つでもあればfalseを返す
	void SetSheetSize(const SSize& size);
	SSize SheetSize(){return m_sizeSheet;}



public:
	//図面要素の追加 操作番号を返す
	int add(SXBSchObj* pObj);
	//選択状態で追加図面要素の追加 操作番号を返す
	int addSelect(SXBSchObj* pObj);
	//点を指定しての選択
	int select(const SPoint& pt);
	//範囲を指定しての選択
	int select(const SRect& rc);
	int selectForDrag(const SRect& rc);



	//m_rcTempArea と m_ptTempSnapPoint を一時オブジェクトのリストから設定する。
	void setupRcTempArea();
	void setupRotateRect();

	
	
	//選択の解除
	//ドキュメントに変更があったときtrueを返す
	bool resetSelect();
	//一時オブジェクトの範囲を取得する
	SRect tempArea(){return m_rcTempArea;}
	//一時オブジェクトのスナップポイントを取得する
	SPoint tempSnapPoint(){return m_ptTempSnapPoint;}
	//初期化
	void init();
	//ファイルからの読み込み
	int readFile(const char* fname);
	int readFile(FILE* fp);
	//ファイルへの書き込み
	int writeFile(const char* fname);
	int writeFile(FILE* fp);




	//***** 編集 *****

	//コピー
	//コピーが行われたときにtrueを返す
	virtual bool copy(){return false;}
	//貼り付け
	//ドキュメントに変更があったときtrueを返す
	virtual bool paste(const SPoint&){return false;}
	//貼り付け可能か？
	virtual bool canPaste(){return false;}

	//削除
	//ドキュメントに変更があったときtrueを返す
	bool del();

	//切り取り
	//ドキュメントに変更があったときtrueを返す
	bool cut();


	//Undo
	//ドキュメントに変更があったときtrueを返す
	bool undo();

	//コピー可能か？
	bool canCopy();
	//削除可能か？
	bool canDel();
	//切り取り可能か？
	bool canCut();
	//Undo可能か？
	bool canUndo();

	//Attribute変更が可能か?
	bool canAttribute();

	//Rotate
	//ドキュメントに変更があったときtrueを返す
	bool rotate();
	//Mirror
	//ドキュメントに変更があったときtrueを返す
	bool mirror();

	//Rotate可能か？
	bool canRotate();
	//Mirror可能か？
	bool canMirror();
	

	//レイヤーの移動
	bool moveToLayer(int nLayer);


	//操作番号を返す
	int opCount(){return m_nOpCount;}
	//古いデータの削除
	void delOldData(int nOpCount);
	//指定した座標が一時データの上かどうか
	int isOnTempData(const SPoint& pt);
	//一時データを移動する
	void moveTempData(int offsetx,int offsety);
};

#endif  //#ifndef XBSCHDOC_H
