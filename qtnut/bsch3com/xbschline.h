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
** XBSch図面要素線クラス
****************************************************************************/
#ifndef XBSCHLINE_H
#define XBSCHLINE_H

#include "xbsch.h"
#include "xbschobj.h"
//#include <SPoint.h>
//#include <SRect.h>

class SXBSchLine:public SXBSchObj
{
public:
	//コンストラクタ
	SXBSchLine();
	//デストラクタ
	virtual ~SXBSchLine(){}
	//選択状態のときのスナップポイントを取得する
	SPoint snapPoint();

public:
	//点を指定して選択重要度を返す
	int qSelectWeight(const SPoint&);
	//範囲を指定して選択を行う
	unsigned testSelection(const SRect& );
	//範囲を選択してドラッグ選択を行う
	unsigned testSelectionForDrag(const SRect& );
	//描画が必要かどうかを返す
	bool qRedraw(const SRect& );
	//ピン数を返す
	//int pinCount(){return 2;}
	//n番目のピンの位置を得る。
	//SPoint pinPosition(int);
	SRect area();
	SRect rotateRect();
	//有効なオブジェクトならtrueを返す。長さのない線分は有効ではない。
	bool qValid(){return(m_p1 != m_p2);}
protected:
	//ストリームからの読み込み
	//bool readStreamEx(SCsvIStream* pStrm,const SPoint* pptOrigin,const char* pTag);
	//ストリームへの書き込み
	//bool writeStreamEx(SCsvOStream* pStrm,const SPoint* pptOrigin,const char* pTag);

	//ストリームからの読み込み
	virtual bool readCe3Ex(SReadCE3& rce3,const SPoint* pptOrigin,const char* pTag);
	//ストリームへの書き込み
	virtual bool writeCe3Ex(SWriteCE3& wce3,const SPoint* pptOrigin,const char* pTag);
};

//ワイヤークラス
class SXBSchWire:public SXBSchLine
{
public:
	//コンストラクタ
	SXBSchWire(){}
	//デストラクタ
	~SXBSchWire(){}
/*
	//ストリームからの読み込み ~ -WIRE
	bool readStream(SCsvIStream* pStrm,const SPoint* pptOrigin);
	//ストリームへの書き込み　+WIRE ~ -WIRE
	bool writeStream(SCsvOStream* pStrm,const SPoint* pptOrigin);
*/
	bool readCe3(SReadCE3&,const SPoint* ,SXBSchDoc*);
	bool writeCe3(SWriteCE3& ,const SPoint* );

	//IDの取得
	unsigned id(){return ID_WIRE;}
	//複製を行う
	SXBSchObj* duplicate();
};

//バスクラス
class SXBSchBus:public SXBSchLine
{
public:
	//コンストラクタ
	SXBSchBus(){}
	//デストラクタ
	~SXBSchBus(){}
	//IDの取得
	unsigned id(){return ID_BUS;}
/*
	//ストリームからの読み込み ~ -BUS
	bool readStream(SCsvIStream* pStrm,const SPoint* pptOrigin);
	//ストリームへの書き込み　+BUS ~ -BUS
	bool writeStream(SCsvOStream* pStrm,const SPoint* pptOrigin);
*/
	bool readCe3(SReadCE3&,const SPoint* ,SXBSchDoc*);
	bool writeCe3(SWriteCE3& ,const SPoint* );

	//複製を行う
	SXBSchObj* duplicate();
};

//破線クラス
class SXBSchDash:public SXBSchLine
{
public:
	//コンストラクタ
	SXBSchDash(){}
	//デストラクタ
	~SXBSchDash(){}
/*
	//ストリームからの読み込み ~ -DASH
	bool readStream(SCsvIStream* pStrm,const SPoint* pptOrigin);
	//ストリームへの書き込み　+DASH ~ -DASH
	bool writeStream(SCsvOStream* pStrm,const SPoint* pptOrigin);
*/
	bool readCe3(SReadCE3&,const SPoint* ,SXBSchDoc*);
	bool writeCe3(SWriteCE3& ,const SPoint* );

	
	//IDの取得
	unsigned id(){return ID_DASH;}
	//複製を行う
	SXBSchObj* duplicate();
};


#endif
