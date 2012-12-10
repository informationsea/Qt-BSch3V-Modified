/****************************************************************************
    BSch3V schematic capture
    Copyright (C) 1997-2005 H.Okada

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
** XBSch図面情報クラスの実装
****************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>
using namespace std;
#include "ce3io.h"

#include "xbschobj.h"
#include "SheetSizeInfo.h"
#include "xbschsheetinfo.h"


bool SXBSchSheetInfo::readCe3(SReadCE3& rce3,const SPoint*,SXBSchDoc*)
{
    string str("");
    bool bInitW = false;
    bool bInitH = false;
    while(1) {
        if(rce3.ReadRecord(str) == EOF) return false;
        if(str[0] == '-') {
            if((str != "-SHEETINFO") || !bInitW || !bInitH) {
                return false;
            } else {
                break;
            }
        } else if(str[0] == '+') {
            str[0]='-';
            if(rce3.SkipTo(str) == EOF) return false;
        } else {
            int n;
            int l = str.length();
            for(n = 0; n < l; n++) {
                if(str[n] == ':') break;
            }
            if(0 < n && n < l) {    // : で分けられたレコードである
                string var = str.substr(0,n);    	    	//先頭から:の手前まで
                int nParam = atoi(str.substr(n+1).c_str()); //:から終端までを数値に変換
                // #ifdef _MFCBSCH    //Qt-BSch doesn't support layer.
                if(var == "EL") {
                    if(isValidLayer(nParam)) {
                        m_Layer    = nParam;
                    }
                } else if(var == "VL") {
                    nParam &=0xFF;
                    if(nParam == 0) nParam = 0xFF;
                    m_Selection = nParam;
                } else
                    // #endif
                    if(var == "W") {
                        if(nParam >= SHEETSIZE_X_MIN && nParam <=SHEETSIZE_X_MAX) m_p1.setX(nParam);
                        bInitW = true;
                    } else if(var == "H") {
                        if(nParam >= SHEETSIZE_Y_MIN && nParam <=SHEETSIZE_Y_MAX) m_p1.setY(nParam);
                        bInitH = true;
                    }
            }
        }
    }
    return true;
}
/*
//ストリームからの読み込み ~ -SHEETINFO
bool SXBSchSheetInfo::readStream(SCsvIStream* pStrm,const SPoint*)
{
 if(!pStrm) return false;
 QString str;
 int nResult;
 int nParam;
 bool bInitW = false;
 bool bInitH = false;
 while(1){
  nResult = pStrm->getRecord(str);
  if(nResult == SCsvIStream::GETRECORD_EOF || nResult == SCsvIStream::GETRECORD_ERROR){
   return false;
  }
  //str = deleteLeftSpace(str);
  if(str[0] == '-'){
   if((str != "-SHEETINFO") || !bInitW || !bInitH){
    return false;
   }else{
    break;
   }
  }else if(str[0] == '+'){
   if(pStrm->skipTo(str.replace(1,1,"-")) != SCsvIStream::GETRECORD_NORMAL)return false;
  }else{
   QChar cc = str[0];
   switch(cc){
   case 'W':
    if(str[1] != ':') break;
    nParam = (str.mid(2)).toInt();
    m_p1.setX(nParam);
    bInitW = true;
    break;
   case 'H':
    if(str[1] != ':') break;
    nParam = (str.mid(2)).toInt();
    m_p1.setX(nParam);
    bInitH = true;
    break;
   default:
    break;
   }
  }
 }
 return true;
}
*/

bool SXBSchSheetInfo::writeCe3(SWriteCE3& wce3,const SPoint* )
{
    //char sz[32];
    wce3.WriteRecord("+SHEETINFO");
    wce3.WriteRecordInt("EL",m_Layer);
    wce3.WriteRecordInt("VL",m_Selection);
    wce3.WriteRecordInt("W",m_p1.x());
    //sprintf(sz,"W:%d",m_p1.x());
    //wce3.WriteRecord(sz);
    wce3.WriteRecordInt("H",m_p1.y());
    //sprintf(sz,"H:%d",m_p1.y());
    //wce3.WriteRecord(sz);
    wce3.WriteRecord("-SHEETINFO");
    wce3.WriteEOL();
    return true;
}

/*
//ストリームへの書き込み　+SHEETINFO ~ -SHEETINFO
bool SXBSchSheetInfo::writeStream(SCsvOStream* pStrm,const SPoint*)
{
 if(!pStrm) return false;
 char sz[32];
 pStrm->writeRecord("+SHEETINFO");
 sprintf(sz,"W:%d",m_p1.x());
 pStrm->writeRecord(sz);
 sprintf(sz,"H:%d",m_p1.y());
 pStrm->writeRecord(sz);
 pStrm->writeRecord("-SHEETINFO");
 pStrm->writeEOL();
 return true;
}
*/
