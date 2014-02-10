/****************************************************************************
    Qt-LCoV Library Editor for Qt-BSch3V
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

#include <qapplication.h>
#include <qdir.h>
#include <qfont.h>
#include "application.h"
#include "cfgdata.h"
#include "xbschfont.h"

class SCmdlineOption
{
public:
    SCmdlineOption();
    string openFileName;
    string configFileName;
    void setupOption(QApplication& a);
};

SCmdlineOption::SCmdlineOption()
{
    openFileName    ="";
    configFileName    ="";
}

void SCmdlineOption::setupOption(QApplication& a)
{
//    int i;
//    string var="";
//    string val="";
//    for(i = 1; i < a.argc(); i++) {
//        val = string(a.argv()[i]);
//        if(val[0] == '-') {
//            var = val;
//        } else {
//            if(var.length() == 0) {
//                openFileName = val;
//            } else if(var == "-conf") {
//                configFileName = val;
//            }
//            var = "";
//        }
//    }
}

static string g_strCfgFileName;

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );

    //#ifdef Q_WS_X11
    //    QApplication::setStyle("windows");
    //#endif

    SCmdlineOption cmdoption;
    cmdoption.setupOption(a);

    g_strCfgFileName = cmdoption.configFileName;

    if(g_strCfgFileName.length() == 0) {
        //コマンドラインでの指定が無かった場合のコンフィギュレーションファイルの名前決定
        //#ifdef _WINDOWS
#ifdef Q_WS_WIN
        //Windows版では、カレントディレクトリに qlcov.cfg
        g_strCfgFileName = QDir::currentPath().toStdString();
        g_strCfgFileName = g_strCfgFileName + "/qlcov.cfg";
#else
        //LINUX版では、ホームディレクトリに .qlcovrc
        g_strCfgFileName = QDir::homePath().toStdString();
        g_strCfgFileName = g_strCfgFileName + "/.qlcovrc";
#endif
    }

    g_cfg.load(g_strCfgFileName.c_str());

    QFont font;
    string strname;
    if(g_cfg.getString("SmallFont","Name",strname)) {
        font = QFont(QString::fromLocal8Bit(strname.c_str()));
        int n;
        if(g_cfg.getInt("SmallFont","Size",n)) font.setPixelSize(n);
        if(g_cfg.getInt("SmallFont","Bold",n)) font.setBold(n != 0);
        if(g_cfg.getInt("SmallFont","Italic",n)) font.setItalic(n != 0);
        if(g_cfg.getInt("SmallFont","Underline",n)) font.setStrikeOut(n != 0);
        if(g_cfg.getInt("SmallFont","StrikeOut",n)) font.setUnderline(n != 0);
        g_SmallFont.SetFont(font);
    }
    ApplicationWindow *mw = new ApplicationWindow();
    //mw->setCaption( "Qt Example - Application" );
    mw->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    a.installEventFilter(mw);
    int result= a.exec();
    g_cfg.save(g_strCfgFileName.c_str());
    return result;
}
