/****************************************************************************
    Qt-BSch3V schematic capture
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
 ** XBSch図面要素タグクラス
 ****************************************************************************/
#ifndef XBSCHTAG_H
#define XBSCHTAG_H

#include "xbsch.h"
#include "xbschobj.h"
#include <assert.h>

class SXBSchTag:public SXBSchObj
{

public:

    enum BSCHTAG_TYPE {
        TAG_SQ = 0,
        TAG_LU = 1,
        TAG_RD = 2,
        TAG_BI = 3
    };

    //コンストラクタ
    SXBSchTag();
    //デストラクタ
    virtual ~SXBSchTag() {}

    bool readCe3(SReadCE3&,const SPoint* ,SXBSchDoc*);
    bool writeCe3(SWriteCE3& ,const SPoint* );


    //IDの取得
    unsigned id() {
        return ID_TAG;
    }
protected:
    string m_strText;
    bool m_bHoriz;
    int m_nType;
public:
    int width();
    void setp2();
    // m_p1が左または下になるように調整する
    void normalize();
    //複製を行う
    SXBSchObj* duplicate();
    //座標の設定  m_p2への設定を無効にしている
    void setX2(int) {}
    void setY2(int) {}
    void setP2(const SPoint&) {}
    //X座標を指定して左右反転を行う
    void mirrorH(int x);
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
    //ピン数を返す
    int pinCount()const {
        return 2;
    }
    //n番目のピンの位置を得る。
    SPoint pinPosition(int n) const;

public:
    bool canAttribute() {
        return true;
    }
    //有効なオブジェクトならtrueを返す
    bool qValid() {
        return true;
    }

    bool horizontal() {
        return m_bHoriz;
    }
    void setHorizontal(bool horizFlag) {
        m_bHoriz = horizFlag;
    }
    int tagType() {
        return m_nType;
    }
    void setTagType(int nType) {
        assert(nType >= 0 && nType<=3);
        m_nType = nType ;
    }
    void setText(const char* psz);
    const char* text() {
        return m_strText.c_str();
    }

};

#endif


/*
  Local Variables:
  mode: c++
  End:
 */
