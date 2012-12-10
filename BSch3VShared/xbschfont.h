/*******************************************************************************
 *   This file is part of Qt-BSch3V Modified.
 *   Copyright (C) 2002-2005 H.Okada
 *   Copyright (C) 2010-2011 Stork Lab
 *
 *   Qt-BSch3V Modified is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Qt-BSch3V Modified is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#ifndef _SMALLFONT_H
#define _SMALLFONT_H

#include "coord.h"
///////////////////////////////////////////////////////////////////
// XBSchのスモールフォントクラス
// ＣＥのフォントと同一のものを使う
// このクラスはXBSchの起動時にグローバル変数で１つだけ作成して、終了時に破棄する。
// 通常運用にはDrawTextメソッドしか使わないため、グローバル関数として作成することも考えたが、
// フォント利用に初期化が必要なため、BSchと同様にクラスとする。
// 通常文字列の描画は、QtではXORが使えるようなので(本当かな?)、このクラスからは外す。
//   BSchでは、XOR描画を実現するために、通常文字列描画もこのクラスを通した。



class SSmallFont
{
private:
    const char*    m_pString;   //文字列へのポインタ
    int            m_nStrLen;   //文字列の長さ
    unsigned char* m_pBmpBit;   //展開用バッファへのポインタ。コンストラクタで4096バイト確保
    QFont          m_font;
    int            m_fontSize;

    int m_scale;
    int  m_scaleMul;
    int  m_upperPos;
    int  m_vOffset;
    bool m_updateFont;


private:


public:
    SSmallFont();               //コンストラクタ
    ~SSmallFont();              //デストラクタ。
    void SetFont(const QFont& font); //描画に使うフォントを設定する。

    SSize  DrawText(            //８ドットフォントの描画
        QPainter*     pPainter, //出力先
        int           x,
        int           y,        //出力位置
        const char*   str,      //出力文字列
        const QColor& color,    //出力色
        unsigned      nMode,    //= DRAW_ON,//描画モード
        bool          bVert,    //= false,  //垂直フラグ
        int           nPointPos, //指定した出力位置が 0:文字列の前 1:文字列の後ろ 2:文字列の中央
        //      0:文字列の下 4:文字列の上   8:文字列の中

        int nDiv,               //= 1,
        int nMul,               //= 1    //nMul/nDivに拡大縮小表示。
        bool bUseUpperLine   = true
    );
};

//////////////////////////////////////////////////////////////////////
//フォント関連 グローバル変数・関数
extern SSmallFont g_SmallFont;

SSize DrawString(
    QPainter*     p,            //出力先
    int           x,int y,      //出力位置
    const char*   pstr,         //出力文字列
    const QColor& color,        //出力色
    QFont&        rFont,
    int           fontSize,
    //unsigned nMode, //描画モード
    bool          bVert,        //垂直フラグ
    int           nPointPos,    //指定した出力位置が 0:文字列の前 1:文字列の後ろ 2:文字列の中央
    //      0:文字列の下 4:文字列の上   8:文字列の中
    int           nDiv,
    int           nMul);

#endif


/*
  Local Variables:
  mode: c++
  End:
 */
