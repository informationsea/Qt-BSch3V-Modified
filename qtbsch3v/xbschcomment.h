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
 ** XBSch図面要素ラベルクラス
 ****************************************************************************/
#ifndef XBSCHCOMMENT_H
#define XBSCHCOMMENT_H

#include "xbsch.h"
#include "xbschobj.h"

#define COMMENT_Y_OFFSET 2


class SXBSchComment:public SXBSchObj
{
public:
    //コンストラクタ
    SXBSchComment();
    //デストラクタ
    virtual ~SXBSchComment() {}

    bool readCe3(SReadCE3&,const SPoint* ,SXBSchDoc*);
    bool writeCe3(SWriteCE3& ,const SPoint* );

    //IDの取得
    unsigned id() {
        return ID_COMMENT;
    }
protected:
    string m_strText;
    bool   m_drawSizeIsValid;
    //    int        m_drawAreaVOffset;
    SSize  m_drawSize;
    string m_fontName;
    int    m_fontSize;
    bool   m_fontBold;
    bool   m_fontItalic;
    bool   m_fontUnderline;
    bool   m_fontStrikeOut;


public:
    //複製を行う
    SXBSchObj* duplicate();
    //座標の設定  m_p2への設定を無効にしている
    void setX2(int) {}
    void setY2(int) {}
    void setP2(const SPoint&) {}
    //左右反転を行う
    void mirrorH() {}    //何もしない
    //回転を行う
    void rotate() {}
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


public:
    //有効なオブジェクトならtrueを返す
    bool qValid() {
        return (m_strText.length()>0);
    }
    void setText(const char* psz);
    const char* text() {
        return m_strText.c_str();
    }
    //アトリビュートの変更が可能な場合はtrueを返す。
    bool canAttribute() {
        return true;
    }
    bool drawSizeIsValid() {
        return m_drawSizeIsValid;
    }
    //    void setDrawSize(int vOffset,const SSize& size);    //描画サイズの設定。原点より、vOffset上からのサイズ
    void setDrawSize(const SSize& size);    //描画サイズの設定
    void setFontName(const char* psz);    //フォント名の設定
    const char* fontName()const {
        return m_fontName.c_str();   //フォント名の取得
    }
    void setFontSize(int fontSize);    //フォントサイズの設定
    int    fontSize()const {
        return m_fontSize;   //フォントサイズの取得
    }
    void setFontBold(bool istrue);    //ボールド設定
    bool fontBold()const {
        return m_fontBold;   //ボールド設定取得
    }
    void setFontItalic(bool istrue);//イタリック設定
    bool fontItalic()const {
        return m_fontItalic;   //イタリック設定取得
    }
    void setFontUnderline(bool istrue);//アンダーライン設定
    bool fontUnderline()const {
        return m_fontUnderline;   //アンダーライン設定取得
    }
    void setFontStrikeOut(bool istrue);//取り消し線設定
    bool fontStrikeOut()const {
        return m_fontStrikeOut;   //取り消し線設定取得
    }

};

#endif


/*
  Local Variables:
  mode: c++
  End:
 */
