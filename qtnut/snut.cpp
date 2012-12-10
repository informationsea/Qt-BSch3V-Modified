/****************************************************************************
    NUT3W Numbering software for BSch3V
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

#include "snut.h"

SNutDataFile::~SNutDataFile()
{
    delete m_pDoc;
}

SNutComponent::SNutComponent()
{
    m_pObj=NULL;
    m_set=false;
    m_powComp=false;
    m_prefix="";
    m_suffix=0;
}

void SNutComponent::loadRef()
{
    string refnum = m_pObj->refnum();
    int len = refnum.length();
    int pos = len-1;
    while(pos>=0){
        if(!isdigit(refnum[pos])) break;
        pos--;
    }
    pos++;	//プレフィックスの文字数が得られる
    m_prefix = refnum.substr(0,pos);
    m_suffix = atoi(refnum.substr(pos,len-pos).c_str());
}

void SNutComponent::saveRef()
{
    char strSuffix[32];
    sprintf(strSuffix,"%d",m_suffix);

    string refnum=m_prefix+strSuffix;
    m_pObj->setRefnum(refnum.c_str());
}



//bool SNutComponent::operator<(const SNutComponent &a)const
//{
//	if(m_index != a.m_index){
//		return m_index < a.m_index;
//	}
//	SPoint pa = m_pObj->p1();
//	SPoint pb = a.m_pObj->p1();
//	if(pa.x() != pb.x()){
//		return pa.x() < pb.x();
//	}
//	return pa.y() < pb.y();
//}



string createBackUpPath(const char* strPath)
{
    string str=strPath;
    return str+"$";
}

SNut::SNut()
{
    m_dataFileCount=0;
    m_compArray = NULL;
    //	m_powcompArray = NULL;
    m_compArrayCount = 0;
    //	m_powcompArrayCount = 0;
}

SNut::~SNut()
{
    clearCompArray();
    clearSchFiles();
}

void SNut::clearSchFiles()
{
    int n;
    for(n = 0;n<m_dataFileCount;n++){
        m_schFiles[n].m_filename ="";
        SXBSchDoc* pDoc = m_schFiles[n].m_pDoc;
        delete pDoc;
        m_schFiles[n].m_pDoc = NULL;
    }
    m_dataFileCount=0;
}


void SNut::clearCompArray()
{
    int n;
    SNutComponent* pNutComp;
    if(m_compArrayCount>0){
        for(n=0;n<m_compArrayCount;n++){
            pNutComp = m_compArray[n];
            delete pNutComp;
        }
    }
    delete[]m_compArray;
    m_compArray = NULL;
    m_compArrayCount = 0;

    //	if(m_powcompArrayCount>0){
    //		for(n=0;n<m_powcompArrayCount;n++){
    //			pNutComp = m_powcompArray[n];
    //			delete pNutComp;
    //		}
    //	}
    //	delete[]m_powcompArray;
    //	m_powcompArray = NULL;
    //	m_powcompArrayCount = 0;
}


//保存に成功したら0以外を返す
int SNut::saveFiles()
{
    int dataIndex;
    int rval;
    for(dataIndex=0;dataIndex<m_dataFileCount;dataIndex++){
        rval=m_schFiles[dataIndex].m_pDoc->writeFile(m_schFiles[dataIndex].m_filename.c_str());
        if(rval) return 0;
    }
    return 1;
}


//読み込みに成功したら0以外を返す
int SNut::readFile(const char* path)
{
    //int m_dataFileCount;
    //SNutDataFile m_schFiles[MAX_SCH_FILES];
    int n;
    //既にオープンしたデータに同名のファイルがないかのチェック
    for(n=0;n<m_dataFileCount;n++){
        if(path == m_schFiles[n].m_filename) return 0;
    }

    SXBSchDoc* pDoc = new SXBSchDoc;
    if(pDoc->readFile(path) != 0){ // readFile()は0で正常終了
        delete pDoc;
        return 0;
    }

    string strBak=createBackUpPath(path);//バックアップファイル名を作成する。
    remove(strBak.c_str());							//すでに同名のバックアップファイルがある場合に、それを消去
    rename(path,strBak.c_str());					//元のファイルをバックアップファイルに改名

    m_schFiles[n].m_filename = path;
    m_schFiles[n].m_pDoc = pDoc;
    m_dataFileCount++;

    return 1;
}




//Prefix -> powCompでない -> ページ -> X ->Yで比較する。
int ComparePSNutCompPos( const void *p1, const void *p2 )
{
    PSNutComponent pElem1 = *(PSNutComponent*)(p1);
    PSNutComponent pElem2 = *(PSNutComponent*)(p2);

    int n= strcmp(pElem1->m_prefix.c_str(),pElem2->m_prefix.c_str());
    if(n!=0) return n;

    if((pElem1->m_powComp ==false) && (pElem2->m_powComp != false)) return -1;
    if((pElem1->m_powComp !=false) && (pElem2->m_powComp == false)) return 1;


    if(pElem1->m_index != pElem2->m_index){
        return pElem1->m_index - pElem2->m_index;
    }
    SPoint pa = pElem1->m_pObj->p1();
    SPoint pb = pElem2->m_pObj->p1();
    if(pa.x() != pb.x()){
        return pa.x()-pb.x();
    }
    return pa.y()-pb.y();
}

//Prefix -> Suffixで比較する。
int ComparePSNutCompRef( const void *p1, const void *p2 )
{
    PSNutComponent pElem1 = *(PSNutComponent*)(p1);
    PSNutComponent pElem2 = *(PSNutComponent*)(p2);

    int n= strcmp(pElem1->m_prefix.c_str(),pElem2->m_prefix.c_str());
    if(n!=0) return n;
    return pElem1->m_suffix - pElem2->m_suffix;
}



void SNut::pickupComponent(bool renumMode)
{

    NutCompList listCompP;
    NutCompList listPowCompP;
    PSNutComponent pNutComp;
    int n;

    int dataIndex;
    for(dataIndex=0;dataIndex<m_dataFileCount;dataIndex++){
        XBSchObjList* plistSrc = &(m_schFiles[dataIndex].m_pDoc->m_listMain);
        XBSchObjListIterator ite=plistSrc->begin();
        while(ite!=plistSrc->end()){
            SXBSchObj* pObj = (*ite++);
            if(pObj->id() == SXBSchObj::ID_COMPONENT){
                SXBSchComponent* pComp=(SXBSchComponent*)pObj;
                //部品番号が欠けている場合は処理対象外
                if(strlen(pComp->refnum())==0) continue;

                pNutComp = new SNutComponent;
                pNutComp->m_index = dataIndex;
                pNutComp->m_pObj = pComp;
                pNutComp->loadRef();	//レファレンスをprefixとsuffixに分ける。
                //部品番号が数字だけの場合も処理対象外
                if(pNutComp->m_prefix.length()==0){
                    delete pNutComp;
                    continue;
                }

                //電源部品かどうかの判断
                //orgnameが4文字以上で、先頭が"VCC","VDD","GND","VEE","VSS"のとき電源部品とする。
                if((strlen(pComp->orgname())>3)
                        &&(  (strncmp(pComp->orgname(),"VCC",3)==0)
                             ||(strncmp(pComp->orgname(),"VDD",3)==0)
                             ||(strncmp(pComp->orgname(),"GND",3)==0)
                             ||(strncmp(pComp->orgname(),"VEE",3)==0)
                             ||(strncmp(pComp->orgname(),"VSS",3)==0)
                             )
                        ){
                    pNutComp->m_powComp=true;
                }

                listCompP.push_back(pNutComp);

                //				if(renumMode){
                //					listCompP.push_back(pNutComp);
                //				}else{
                //				//電源部品かどうかの判断
                //				//orgnameが4文字以上で、先頭が"VCC","VDD","GND","VEE","VSS"のとき電源部品とする。
                //				if((strlen(pComp->orgname())>3)
                //					&&(  (strncmp(pComp->orgname(),"VCC",3)==0)
                //					   ||(strncmp(pComp->orgname(),"VDD",3)==0)
                //					   ||(strncmp(pComp->orgname(),"GND",3)==0)
                //					   ||(strncmp(pComp->orgname(),"VEE",3)==0)
                //					   ||(strncmp(pComp->orgname(),"VSS",3)==0)
                //					   )
                //					   ){
                //							listPowCompP.push_back(pNutComp);
                //					   }else{
                //							listCompP.push_back(pNutComp);
                //					   }
                //				}
            }
        }

    }

    //リストに格納したデータをメンバー変数の配列に置き換え。
    NutCompListIterator iteComp;
    m_compArrayCount = listCompP.size();
    m_compArray = new PSNutComponent[m_compArrayCount];
    iteComp = listCompP.begin();
    n=0;
    while(iteComp != listCompP.end() && n<m_compArrayCount){
        m_compArray[n++] = (*iteComp++);
    }
    //	if(!renumMode){
    //		m_powcompArrayCount = listPowCompP.size();
    //		m_powcompArray = new PSNutComponent[m_powcompArrayCount];
    //		iteComp = listPowCompP.begin();
    //		n=0;
    //		while(iteComp != listPowCompP.end() && n<m_powcompArrayCount){
    //			m_powcompArray[n++] = (*iteComp++);
    //		}
    //	}

    if(renumMode){
        sortREF();
    }else{
        sortPOS();
    }
}


void SNut::sortPOS()
{
    qsort(m_compArray,m_compArrayCount, sizeof(PSNutComponent), ComparePSNutCompPos );
    //qsort(m_powcompArray,m_powcompArrayCount, sizeof(PSNutComponent), ComparePSNutCompPos );
}

void SNut::sortREF()
{
    qsort(m_compArray,m_compArrayCount, sizeof(PSNutComponent), ComparePSNutCompRef );
}

int SNut::anum(int init,int step)
{
    pickupComponent(false);
    string currentPrefix;
    //	int oldSuffix;
    int suffix;
    int blocks;	//処理中の部品のブロックの個数
    int block;	//割り振ったブロック番号
    SXBSchComponent* pComp;
    SXBSchComponent* pComp_m;

    int n,m;
    for(n=0;n<m_compArrayCount;n++){
        if(m_compArray[n]->m_set) continue;
        if(m_compArray[n]->m_prefix != currentPrefix){
            currentPrefix = m_compArray[n]->m_prefix;
            suffix = init;
        }else{
            suffix += step;
        }
        pComp=m_compArray[n]->m_pObj;
        blocks = pComp->compInfoIndex()->block();
        //if(blocks>1){
        block=0;
        pComp->setBlock(block++);
        m_compArray[n]->m_suffix = suffix;
        m_compArray[n]->saveRef();
        m_compArray[n]->m_set = true;

        if(!m_compArray[n]->m_powComp){
            //複数ブロックを埋める
            for(m=n+1;m<m_compArrayCount;m++){
                if(block>=blocks) break; //ブロック数を埋めてしまえばブレーク
                if(m_compArray[m]->m_prefix != currentPrefix) break; //プレフィックスが変わったらブレーク
                if(m_compArray[m]->m_powComp) break; //電源部品に出会ったらブレーク
                if(m_compArray[m]->m_set) continue;	//処理済みのものはスキップ
                pComp_m=m_compArray[m]->m_pObj;
                if(strcmp(pComp->name(),pComp_m->name())) continue; //同じ名前でなければスキップ
                if(blocks != pComp_m->compInfoIndex()->block()) continue; //ブロックの数が違えばスキップ
                pComp_m->setBlock(block++);
                m_compArray[m]->m_suffix = suffix;
                m_compArray[m]->saveRef();
                m_compArray[m]->m_set = true;
            }
            //電源部品を検索する
            for(;m<m_compArrayCount;m++){
                if(m_compArray[m]->m_prefix != currentPrefix) break; //プレフィックスが変わったらブレーク
                if(m_compArray[m]->m_set) continue;	//処理済みのものはスキップ
                if(!m_compArray[m]->m_powComp) continue; //電源部品でなければスキップ
                pComp_m=m_compArray[m]->m_pObj;
                if(strcmp(pComp->name(),pComp_m->name())) continue; //同じ名前でなければスキップ
                pComp_m->setBlock(0);
                m_compArray[m]->m_suffix = suffix;
                m_compArray[m]->saveRef();
                m_compArray[m]->m_set = true;
                break;
            }
        }
    }
    return 1;
}


int SNut::renum(int init,int step)
{	
    //string prefix="";
    string oldPrefix;
    int oldSuffix;
    int suffix;
    int n;
    pickupComponent(true);
    for(n=0;n<m_compArrayCount;n++){
        if(m_compArray[n]->m_prefix != oldPrefix){
            oldPrefix = m_compArray[n]->m_prefix;
            oldSuffix = m_compArray[n]->m_suffix;
            suffix = init;
        }else{
            if(m_compArray[n]->m_suffix != oldSuffix){
                oldSuffix = m_compArray[n]->m_suffix;
                suffix += step;
            }
        }
        m_compArray[n]->m_suffix = suffix;
        m_compArray[n]->saveRef();
    }
    return 1;
}
