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
** XBSch図面情報クラス
****************************************************************************/
#ifndef XBSCHSHEETINFO_H
#define XBSCHSHEETINFO_H

#include "xbsch.h"
#include "xbschobj.h"

class SXBSchSheetInfo:public SXBSchObj
{
public:
	//コンストラクタ
	SXBSchSheetInfo(){m_p1.setX(1500);m_p1.setY(1000);m_Selection=0xFF;m_Layer=0;}
	//SXBSchJunc(const SXBSchJunc& junc);	//コピーコンストラクタ
	//デストラクタ
	virtual ~SXBSchSheetInfo(){};
/*
	//ストリームからの読み込み ~-SHEETINFO
	bool readStream(SCsvIStream* pStrm,const QPoint* pptOffset);
	//ストリームへの書き込み　+SHEETINFO~+SHEETINFO
	bool writeStream(SCsvOStream* pStrm,const QPoint* pptOffset);
*/
	bool readCe3(SReadCE3&,const SPoint* ,SXBSchDoc*);
	bool writeCe3(SWriteCE3& ,const SPoint* );



	//IDの取得
	unsigned id(){return ID_SHEETINFO;}
public:
	void setWidth(int w) {m_p1.setX(w);}
	void setHeight(int h){m_p1.setY(h);}
	int width(){return x1();}
	int height(){return y1();}

	void setEditLayer(int n){m_Layer=n;}	
	int editLayer(){return m_Layer;}

	void setVisbleLayer(unsigned n){m_Selection = n;}
	unsigned visbleLayer(){return m_Selection;}
	
	void setX1(int){}
	void setY1(int){}
	void setP1(const SPoint&){}
	void setX2(int){}
	void setY2(int){}
	void setP2(const SPoint&){}

	//位置を動かす
	void move(int,int){};
	//X座標を指定して左右反転を行う
	void mirrorH(int){};
	//左右反転を行う
	void mirrorH(){};
	//XY中心座標を指定して、回転を行う
	void rotate(const SPoint&){};
	//回転を行う
	void rotate(){};
	//選択フラグをセットする
	void setSelection(unsigned){};
	//選択を解除する
	void resetSelection(){};
	//占有範囲を返す
	SRect area(){return SRect(0,0,m_p1.x(),m_p1.y());}
};

#endif
