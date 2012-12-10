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
** XBSch図面要素クラス
****************************************************************************/
#ifndef XBSCHOBJ_H
#define XBSCHOBJ_H

#include "xbsch.h"
//#include <SPoint.h>
//#include <SRect.h>
#include "coord.h"
#include "ce3io.h"

class SXBSchDoc;

/*
#define		NOT_SELECT	0
#define		SELECT_P1	1
#define		SELECT_P2	2
#define		SELECT_ALL	3

#define		OUTCODE_LEFT		0x0001
#define		OUTCODE_UPPER		0x0002
#define		OUTCODE_RIGHT		0x0004
#define		OUTCODE_LOWER		0x0008
*/

enum BSCH_TOOL_MODE
{
	TOOL_SELECTOR,
	TOOL_DRAG,
	TOOL_COMPONENT,
	TOOL_BUS,
	TOOL_WIRE,
	TOOL_DASH,
	TOOL_JUNCTION,
	TOOL_BENTRY,
	TOOL_ENTRY,
	TOOL_TAG,
	TOOL_LABEL,
	TOOL_COMMENT
};

enum ON_OBJECT
{
	ON_OBJ		=0x10000,
	ON_NAME		=0x20000,
	ON_NUM		=0x40000
};

class SXBSchObj
{
public:
	enum BSCHOBJ_ID
	{
		ID_JUNCTION		=0,
		ID_ENTRY		=1,
		ID_BENTRY		=2,
		ID_BUS			=3,
		ID_WIRE			=4,
		ID_DASH			=5,
		ID_LABEL		=6,
		ID_TAG			=7,
		ID_COMPONENT	=8,
		ID_SHEETINFO	=9,
		ID_COMMENT		=10,
		ID_DELETEOBJ	=254,
		ID_ENDOFDATA	=255
	};

	enum SELECTION
	{
		NOT_SELECTED	=0,
		SELECT_P1		=1,
		SELECT_P2		=2,
		SELECT_ALL		=3
	};

	enum OUTCODE
	{
		OUTCODE_CENTER	=	0x0000,
		OUTCODE_LEFT	=	0x0001,
		OUTCODE_UPPER	=	0x0002,
		OUTCODE_RIGHT	=	0x0004,
		OUTCODE_LOWER	=	0x0008
	};

protected:
	//配置したときの操作番号
	int m_PlaceNum;
	//参照オブジェクトへのポインタ
	//	ID_DELETEOBJ では、その消去オブジェクトが無効化しているところの対象オブジェクトを指す
	//  それ以外のオブジェクトでは、消去オブジェクトを指す。NULLのときは、そのオブジェクトが有効で
	//  あることを表す。
	SXBSchObj* m_pRefObj;
	//配置レイヤー
	int m_Layer;
	//オブジェクトの座標
	SPoint m_p1;
	SPoint m_p2;
	//選択状態
	unsigned m_Selection;

	bool isValidLayer(int n){return (n>=0 && n<8);}

public:
	//コンストラクタ
	SXBSchObj();
	//デストラクタ
	virtual ~SXBSchObj(){};

//	virtual bool readStream(SCsvIStream* ,const SPoint* ){return false;}
//	virtual bool writeStream(SCsvOStream* ,const SPoint* ){return false;}

	virtual bool readCe3(SReadCE3&,const SPoint* ,SXBSchDoc* ){return false;}
//	virtual bool readCe3(SReadCE3&,const SPoint* ){return false;}
	virtual bool writeCe3(SWriteCE3& ,const SPoint* ){return false;}

	//IDの取得
	virtual unsigned id()=0;
	//配置したときの操作番号
	void setPlaceNum(int n){m_PlaceNum = n;}
	//配置したときの操作番号の取得
	int placeNum(){return m_PlaceNum;}
	//参照オブジェクトへのポインタの設定
	void setRefObj(SXBSchObj* pObj){m_pRefObj = pObj;}
	//参照オブジェクトへのポインタを取得
	SXBSchObj* refObj(){return m_pRefObj;}
	//レイヤーの設定
	void setLayer(int n){m_Layer=n;}
	//レイヤーの取得
	int layer(){return m_Layer;}
	//座標の設定
	virtual void setX1(int x1){m_p1.setX(x1);}
	virtual void setY1(int y1){m_p1.setY(y1);}
	virtual void setX2(int x2){m_p2.setX(x2);}
	virtual void setY2(int y2){m_p2.setX(y2);}
	virtual void setP1(const SPoint& p){m_p1 = p;}
	virtual void setP2(const SPoint& p){m_p2 = p;}
	//座標の取得
	int x1(){return m_p1.x();}
	int y1(){return m_p1.y();}
	virtual int x2(){return m_p2.x();}
	virtual int y2(){return m_p2.y();}
	SPoint p1(){return m_p1;}
	virtual SPoint p2(){return m_p2;}
	virtual SPoint snapPoint(){return m_p1;}
	//選択状態の取得
	unsigned selection(){return m_Selection;}
public:
	//複製を行う
	virtual SXBSchObj* duplicate(){return NULL;}
	//位置を動かす
	virtual void move(int offsetx,int offsety);
	//X座標を指定して左右反転を行う
	virtual void mirrorH(int x);
	//左右反転を行う
	virtual void mirrorH();
	//XY中心座標を指定して、回転を行う
	virtual void rotate(const SPoint& p);
	//回転を行う
	virtual void rotate();
	//点を指定して選択重要度を返す
	virtual int qSelectWeight(const SPoint&){return 0;}
	//選択フラグをセットする
	virtual void setSelection(unsigned sel);
	//選択を解除する
	virtual void resetSelection();
	//範囲を指定して選択を行う
	virtual unsigned testSelection(const SRect& ){return 0;}
	//範囲を選択してドラッグ選択を行う
	virtual unsigned testSelectionForDrag(const SRect& ){return 0;}
	//描画が必要かどうかを返す
	virtual bool qRedraw(const SRect& ){return 0;}
	//ピン数を返す
	virtual int pinCount()const {return 0;}
	//n番目のピンの位置を得る。
	virtual SPoint pinPosition(int) const {return SPoint(-1,-1);}
	virtual SRect area(){return SRect(-1,-1,0,0);}
	virtual SRect rotateRect(){return SRect(-1,-1,0,0);}
	//有効なオブジェクトならtrueを返す。文字列のないテキスト、長さのない線分は有効ではない。
	virtual bool qValid(){return true;}
	//アトリビュートの変更が可能な場合はtrueを返す。
	virtual bool canAttribute() {return false;}
public:
	//pとrcAreaの内外コードを返す
	static unsigned SetOutCode(const SPoint& p,const SRect& rcArea);
	//点と矩形の重なり情報を、重み情報付きで返す
	static int PointAndRectMag(const SPoint& p,const SRect& rcArea);
};

#endif
