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
** XBSch図面要素ジャンクションクラス
****************************************************************************/
#ifndef XBSCHJUNC_H
#define XBSCHJUNC_H

#include "xbsch.h"
#include "xbschobj.h"

class SXBSchJunc:public SXBSchObj
{
public:
	//コンストラクタ
	SXBSchJunc();
	//デストラクタ
	virtual ~SXBSchJunc(){};

/*	//ストリームからの読み込み ~ -JUNCTION
	bool readStream(SCsvIStream* pStrm,const QPoint* pptOrigin);
	//ストリームへの書き込み　+JUNCTION ~ -JUNCTION
	bool writeStream(SCsvOStream* pStrm,const QPoint* pptOrigin);
*/
	bool readCe3(SReadCE3&,const SPoint* ,SXBSchDoc* );
	bool writeCe3(SWriteCE3& ,const SPoint* );

	
	
	//IDの取得
	unsigned id(){return ID_JUNCTION;}
public:
	//複製を行う
	SXBSchObj* duplicate();
	//座標の設定  m_p2への設定を無効にしている
	void setX2(int){}
	void setY2(int){}
	void setP2(const SPoint&){}
	//左右反転を行う
	void mirrorH(){}	//何もしない
	//回転を行う
	void rotate(){}		//何もしない
	//点を指定して選択重要度を返す
	int qSelectWeight(const SPoint& p);
	//範囲を指定して選択を行う
	unsigned testSelection(const SRect& rc);
	//範囲を選択してドラッグ選択を行う
	unsigned testSelectionForDrag(const SRect& rc);
	//描画が必要かどうかを返す
	bool qRedraw(const SRect& rc);
	//占有範囲を返す
	SRect area();
	SRect rotateRect();
};

#endif
