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

#ifndef XBSCHPATTERN_H
#define XBSCHPATTERN_H

#include "coord.h"
void g_put_bmp(
    QPainter* pPainter,            //出力先
    int x,
    int y,                        //出力位置
    const unsigned char* bitmap,            //出力するビットマップ(ワードアラインメント モノクロ)
    int w,                        //ピクセル幅
    int h,                        //ピクセル高
    int dir,                    //向き
    int scale,                    //スケール 1,2,5のどれか
    int scaleMul,                //
    const QColor& color,        //出力色
    unsigned  nMode                //描画モード
);


#endif


/*
  Local Variables:
  mode: c++
  End:
 */
