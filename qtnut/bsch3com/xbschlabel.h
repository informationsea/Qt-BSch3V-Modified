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
** XBSch図面要素ラベルクラス
****************************************************************************/
#ifndef XBSCHLABEL_H
#define XBSCHLABEL_H

#include "xbsch.h"
#include "xbschobj.h"

class SXBSchLabel:public SXBSchObj
{
public:
	//コンストラクタ
	SXBSchLabel();
	//デストラクタ
	virtual ~SXBSchLabel(){};

	bool readCe3(SReadCE3&,const SPoint* ,SXBSchDoc*);
	bool writeCe3(SWriteCE3& ,const SPoint* );
/*
	//ストリームからの読み込み ～ -LABEL
	bool readStream(SCsvIStream* pStrm,const SPoint* pptOrigin);
	//ストリームへの書き込み　+LABEL ～ -LABEL
	bool writeStream(SCsvOStream* pStrm,const SPoint* pptOrigin);
*/
    //IDの取得
	unsigned id(){return ID_LABEL;}
protected:
	string m_strText;
	bool m_bHoriz;
	bool	m_drawSizeIsValid;
	SSize	m_drawSize;


public:
	//複製を行う
	SXBSchObj* duplicate();
	//座標の設定  m_p2への設定を無効にしている
	void setX2(int){}
	void setY2(int){}
	void setP2(const SPoint&){}
	//左右反転を行う
	void mirrorH(){}	//何もしない
	//XY中心座標を指定して、回転を行う
	void rotate(const SPoint& p);
	//回転を行う
	void rotate();
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

	bool drawSizeIsValid(){return m_drawSizeIsValid;}
	void setDrawSize(const SSize& size);	//描画サイズの設定


public:
	//有効なオブジェクトならtrueを返す
	bool qValid(){return (m_strText.length()>0);}

	bool horizontal(){ return m_bHoriz; }
	void setHorizontal(bool horizFlag){m_bHoriz = horizFlag;}
	void setText(const char* psz);
	const char* text(){return m_strText.c_str();}
	//アトリビュートの変更が可能な場合はtrueを返す。
	bool canAttribute(){return true;}
	

};

#endif
