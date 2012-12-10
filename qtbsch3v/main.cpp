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



#include <qapplication.h>
#include <qwindowsstyle.h>
#include <qdir.h>
#include <stdio.h>

#include "application.h"
#include "xbschglobal.h"


//class OldApplicationWindow;

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
    openFileName   = "";
    configFileName = "";
}

void SCmdlineOption::setupOption(QApplication& a)
{
    int i;
    string var = "";
    string val = "";
    for(i = 1; i < a.argc(); i++) {
        val = string(a.argv()[i]);
        if(val[0] == '-') {
            var = val;
        } else {
            if(var.length() == 0) {
                openFileName = val;
            } else if(var == "-conf") {
                configFileName = val;
            }
            var = "";
        }
    }
}

int main( int argc, char ** argv )
{

    //QApplication::setColorSpec(QApplication::ManyColor);
    QApplication a( argc, argv );
    a.setApplicationName("QtBsch3V");
    a.setApplicationVersion("0.47");
    a.setOrganizationDomain("storklab.cyber-ninja.jp");
    a.setOrganizationName("StorkLab");


    SCmdlineOption cmdoption;
    cmdoption.setupOption(a);

    g_strCfgFileName = cmdoption.configFileName;

    if(g_strCfgFileName.length() == 0) {
        //コマンドラインでの指定が無かった場合のコンフィギュレーションファイルの名前決定
        //#ifdef _WINDOWS
#ifdef Q_WS_WIN
        //Windows版では、カレントディレクトリに qbsch.cfg
        g_strCfgFileName = string(QDir::currentPath().toLocal8Bit());
        g_strCfgFileName = g_strCfgFileName + "/qbsch.cfg";
#else
        //LINUX版では、ホームディレクトリに .qbschrc
        g_strCfgFileName = string(QDir::homePath().toLocal8Bit());
        g_strCfgFileName = g_strCfgFileName + "/.qbschrc";
#endif
    }



    g_cfg.load(g_strCfgFileName.c_str());
    g_ReadLibrary();


    ApplicationWindow *mw2 = new ApplicationWindow;
    mw2->show();

    //OldApplicationWindow * mw = new OldApplicationWindow;

    //mw->show();

    if(cmdoption.openFileName.length()>0) {
        mw2->openFile(QString::fromLocal8Bit(cmdoption.openFileName.c_str()));
    }

    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    a.installEventFilter(mw2);

    int result = a.exec();
    //    g_cfg.dump();
    g_cfg.save(g_strCfgFileName.c_str());

    g_FreeLibrary();

    return result;
}
