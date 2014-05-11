#include "application.h"
#include "ui_application.h"

#include <QUrl>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileInfo>
#include <QLabel>
#include <QFileDialog>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPrinter>
#include <QFontDialog>

#include <QDebug>

#include "cfgdata.h"
#include "sheetmessagebox.h"
#include "xbschglobal.h"

#include "printOptionDlg.h"
#include "setlibrarydialog.h"

QList < ApplicationWindow *> ApplicationWindow::applicationWindows;

ApplicationWindow::ApplicationWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ApplicationWindow)
{
    ui->setupUi(this);

    // Add components

    QFont tipfont( "helvetica", 10 ) ;
    m_plabelX = new QLabel(" X:0000 ",statusBar());
    m_plabelY = new QLabel(" Y:0000 ",statusBar());
    m_plabelX->setFont( tipfont );
    m_plabelY->setFont( tipfont );
    statusBar()->addPermanentWidget(m_plabelX);
    statusBar()->addPermanentWidget(m_plabelY);

    // Action settings

    ui->action_New->setShortcuts(QKeySequence::New);
    ui->action_Open->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSave_as->setShortcuts(QKeySequence::SaveAs);
    ui->action_Print->setShortcuts(QKeySequence::Print);
    ui->action_Close->setShortcut(QKeySequence::Close);
    ui->action_Quit->setShortcuts(QKeySequence::Quit);

    ui->action_Undo->setShortcuts(QKeySequence::Undo);
    ui->action_Copy->setShortcuts(QKeySequence::Copy);
    ui->action_Cut->setShortcuts(QKeySequence::Cut);
    ui->action_Paste->setShortcuts(QKeySequence::Paste);
    //ui->action_Delete->setShortcuts(QKeySequence::Delete);

    ui->actionZoom_In->setShortcuts(QKeySequence::ZoomIn);
    ui->actionZoom_Out->setShortcuts(QKeySequence::ZoomOut);

    ui->action_Select->setData(TOOL_SELECTOR);
    ui->action_Drag->setData(TOOL_DRAG);
    ui->action_Component->setData(TOOL_COMPONENT);
    ui->action_Bus->setData(TOOL_BUS);
    ui->action_Wire->setData(TOOL_WIRE);
    ui->actionD_ash->setData(TOOL_DASH);
    ui->action_Junction->setData(TOOL_JUNCTION);
    ui->actionB_us_entry->setData(TOOL_BENTRY);
    ui->action_Entry->setData(TOOL_ENTRY);
    ui->action_Tag->setData(TOOL_TAG);
    ui->action_Label->setData(TOOL_LABEL);
    ui->actionC_omment->setData(TOOL_COMMENT);

    ui->actionMove_to_Layer_0->setData(0);
    ui->actionMove_to_Layer_1->setData(1);
    ui->actionMove_to_Layer_2->setData(2);
    ui->actionMove_to_Layer_3->setData(3);
    ui->actionMove_to_Layer_4->setData(4);
    ui->actionMove_to_Layer_5->setData(5);
    ui->actionMove_to_Layer_6->setData(6);
    ui->actionMove_to_Layer_7->setData(7);


    // Signal & Slot connect
    connect(ui->actionSave,SIGNAL(triggered()),SLOT(saveFile()));
    connect(ui->actionSave_as,SIGNAL(triggered()),SLOT(saveFileAs()));
    connect(ui->action_Quit,SIGNAL(triggered()),qApp,SLOT(closeAllWindows()));

    connect(ui->action_Undo,SIGNAL(triggered()),ui->xbschview,SLOT(undo()));
    connect(ui->action_Copy,SIGNAL(triggered()),ui->xbschview,SLOT(copy()));
    connect(ui->action_Cut,SIGNAL(triggered()),ui->xbschview,SLOT(cut()));
    connect(ui->action_Paste,SIGNAL(triggered()),ui->xbschview,SLOT(paste()));
    connect(ui->action_Delete,SIGNAL(triggered()),ui->xbschview,SLOT(del()));
    connect(ui->action_Attribute,SIGNAL(triggered()),ui->xbschview,SLOT(attribute()));
    connect(ui->action_Mirror,SIGNAL(triggered()),ui->xbschview,SLOT(mirror()));
    connect(ui->action_Rotate,SIGNAL(triggered()),ui->xbschview,SLOT(rotate()));
    connect(ui->action_Refresh,SIGNAL(triggered()),ui->xbschview,SLOT(RefreshView()));

    connect(ui->action_Select,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->action_Drag,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->action_Component,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->action_Bus,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->action_Wire,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->actionD_ash,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->action_Junction,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->actionB_us_entry,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->action_Entry,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->action_Tag,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->action_Label,SIGNAL(triggered()),SLOT(toolSelectionChanged()));
    connect(ui->actionC_omment,SIGNAL(triggered()),SLOT(toolSelectionChanged()));

    connect(ui->actionSheet_size,SIGNAL(triggered()),ui->xbschview,SLOT(resizeSheet()));

    connect(ui->menu_Window,SIGNAL(aboutToShow()),SLOT(updateWindowMenu()));
    connect(ui->menu_Window,SIGNAL(triggered(QAction*)),SLOT(selectActiveWindow(QAction*)));
    connect(ui->menu_Edit,SIGNAL(aboutToShow()),SLOT(updateEditMenu()));

    connect(ui->xbschview,SIGNAL(cursorMove(int,int)),SLOT(cursorMove(int,int)));
    connect(ui->xbschview,SIGNAL(toolChange()),SLOT(toolSelectionChanged()));

    connect(ui->layerEnable0,SIGNAL(clicked()),SLOT(enableLayerChanged()));
    connect(ui->layerEnable1,SIGNAL(clicked()),SLOT(enableLayerChanged()));
    connect(ui->layerEnable2,SIGNAL(clicked()),SLOT(enableLayerChanged()));
    connect(ui->layerEnable3,SIGNAL(clicked()),SLOT(enableLayerChanged()));
    connect(ui->layerEnable4,SIGNAL(clicked()),SLOT(enableLayerChanged()));
    connect(ui->layerEnable5,SIGNAL(clicked()),SLOT(enableLayerChanged()));
    connect(ui->layerEnable6,SIGNAL(clicked()),SLOT(enableLayerChanged()));
    connect(ui->layerEnable7,SIGNAL(clicked()),SLOT(enableLayerChanged()));

    connect(ui->layerSelect0,SIGNAL(clicked()),SLOT(selectedLayerChanged()));
    connect(ui->layerSelect1,SIGNAL(clicked()),SLOT(selectedLayerChanged()));
    connect(ui->layerSelect2,SIGNAL(clicked()),SLOT(selectedLayerChanged()));
    connect(ui->layerSelect3,SIGNAL(clicked()),SLOT(selectedLayerChanged()));
    connect(ui->layerSelect4,SIGNAL(clicked()),SLOT(selectedLayerChanged()));
    connect(ui->layerSelect5,SIGNAL(clicked()),SLOT(selectedLayerChanged()));
    connect(ui->layerSelect6,SIGNAL(clicked()),SLOT(selectedLayerChanged()));
    connect(ui->layerSelect7,SIGNAL(clicked()),SLOT(selectedLayerChanged()));

    connect(ui->actionMove_to_Layer_0,SIGNAL(triggered()),SLOT(moveToLayer()));
    connect(ui->actionMove_to_Layer_1,SIGNAL(triggered()),SLOT(moveToLayer()));
    connect(ui->actionMove_to_Layer_2,SIGNAL(triggered()),SLOT(moveToLayer()));
    connect(ui->actionMove_to_Layer_3,SIGNAL(triggered()),SLOT(moveToLayer()));
    connect(ui->actionMove_to_Layer_4,SIGNAL(triggered()),SLOT(moveToLayer()));
    connect(ui->actionMove_to_Layer_5,SIGNAL(triggered()),SLOT(moveToLayer()));
    connect(ui->actionMove_to_Layer_6,SIGNAL(triggered()),SLOT(moveToLayer()));
    connect(ui->actionMove_to_Layer_7,SIGNAL(triggered()),SLOT(moveToLayer()));

    // Read settings
    QSettings *settings = g_cfg.getSettings();
    settings->beginGroup("SET");
    //スナップの状態の設定
    bool nCfg1 = settings->value("SNAP",true).toBool();
    ui->xbschview->setSnapGrid(nCfg1);
    ui->actionS_nap->setChecked(nCfg1);
    settings->endGroup();

    settings->beginGroup("VIEW");
    // Grid
    nCfg1 = settings->value("GRID",true).toBool();
    ui->xbschview->setDisplayGrid(nCfg1);
    ui->action_Grid->setChecked(nCfg1);

    //非アクティブレイヤーの表示状態の設定
    nCfg1 = settings->value("DARKEN_NONACTIVE_LAYER",true).toBool();
    ui->actionDarken_non_Active_Layer->setChecked(nCfg1);
    ui->xbschview->setDarkenNonActiveLayer(nCfg1 != 0);

    settings->endGroup();

    //ツールモードの設定
    ui->xbschview->setToolMode(TOOL_SELECTOR);
    SetToolButtonState();

    g_SmallFont.SetFont(g_cfg.getSettings()->value("SmallFont",QFont("San-serif",8)).value < QFont>());

    applicationWindows.append(this);

    ui->xbschview->setApplicationWindow(this);

    QByteArray state = settings->value("MAIN_WINDOW_STATE").toByteArray();
    restoreState(state);
}

ApplicationWindow::~ApplicationWindow()
{
    applicationWindows.removeOne(this);

    delete m_plabelX;
    delete m_plabelY;

    delete ui;
}

void ApplicationWindow::closeEvent( QCloseEvent* ce )
{
    QSettings *settings = g_cfg.getSettings();
    QByteArray state;
    if(ui->xbschview->isDirty()) {
        QMessageBox::StandardButton ret =
            SheetMessageBox::information( this, "Close Qt-BSch3V Document" ,
                                          "The current document has been changed since the last save.",
                                          QMessageBox::Save|QMessageBox::Cancel|QMessageBox::Discard,QMessageBox::Save);
        switch( ret ) {
        case QMessageBox::Save:
            if(saveFile() == false) {
                settings->setValue("MAIN_WINDOW_STATE", saveState());
                ce->accept();
            } else {
                ce->ignore();
            }
            break;
        case QMessageBox::Discard:
            settings->setValue("MAIN_WINDOW_STATE", saveState());
            ce->accept();
            break;
        case QMessageBox::Cancel:
        default:
            ce->ignore();
            break;
        }
    } else {
        settings->setValue("MAIN_WINDOW_STATE", saveState());
        ce->accept();
    }
}

bool ApplicationWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (qApp == obj && event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openevent = static_cast < QFileOpenEvent *>(event);
        ApplicationWindow * aw = ApplicationWindow::firstEmptyDocWindow();
        if(aw == NULL) aw = new ApplicationWindow();
        aw->show();
        aw->openFile(openevent->file());
        aw->activateWindow();
        event->accept();
        return true;
    } else {
        return QMainWindow::eventFilter(obj,event);
    }
}



bool ApplicationWindow::isEmptyDocWindow()
{
    return  m_filename.isEmpty() && ui->xbschview->isEmpty();
}

ApplicationWindow* ApplicationWindow::firstEmptyDocWindow()
{
    foreach(ApplicationWindow *w,applicationWindows) {
        if(w->isEmptyDocWindow()) return w;
    }

    return NULL;
}

void ApplicationWindow::selectedLayerChanged()
{
    int selected = 1;
    if(ui->layerSelect0->isChecked()) selected = 0;
    if(ui->layerSelect1->isChecked()) selected = 1;
    if(ui->layerSelect2->isChecked()) selected = 2;
    if(ui->layerSelect3->isChecked()) selected = 3;
    if(ui->layerSelect4->isChecked()) selected = 4;
    if(ui->layerSelect5->isChecked()) selected = 5;
    if(ui->layerSelect6->isChecked()) selected = 6;
    if(ui->layerSelect7->isChecked()) selected = 7;
    ui->xbschview->setActiveLayer(selected);
    ui->xbschview->RefreshView();
}

void ApplicationWindow::enableLayerChanged()
{
    int enabled = 0;
    if(ui->layerEnable0->isChecked()) enabled |= 1 << 0;
    if(ui->layerEnable1->isChecked()) enabled |= 1 << 1;
    if(ui->layerEnable2->isChecked()) enabled |= 1 << 2;
    if(ui->layerEnable3->isChecked()) enabled |= 1 << 3;
    if(ui->layerEnable4->isChecked()) enabled |= 1 << 4;
    if(ui->layerEnable5->isChecked()) enabled |= 1 << 5;
    if(ui->layerEnable6->isChecked()) enabled |= 1 << 6;
    if(ui->layerEnable7->isChecked()) enabled |= 1 << 7;
    ui->xbschview->setVisibleLayer(enabled);
    ui->xbschview->RefreshView();
}

void ApplicationWindow::moveToLayer()
{
    QAction * act = (QAction *)QObject::sender();
    ui->xbschview->moveToLayer(act->data().toInt());
}

//アプリケーションのキャプションの設定
void ApplicationWindow::setAppCaption()
{
    QFileInfo finfo(m_filename);

    QString caption = "Qt-BSch3V Modified - [*] ";
    caption += finfo.fileName();
    setWindowTitle( caption );
}

void ApplicationWindow::on_actionVisit_StorkLab_webpage_triggered()
{
    QDesktopServices::openUrl(QUrl("http://storklab.cyber-ninja.jp"));
}

void ApplicationWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this,"About Qt");
}

void ApplicationWindow::on_actionAbout_QtBSch3V_Modified_triggered()
{

    QPixmap icon( ":/images/icon.png" );

    QMessageBox mb(this);

#ifndef VERSION
#define VERSION "Uknown version"
#endif

#ifndef _STR
#define _STR(x) #x
#define STR(x) _STR(x)
#endif
#define VERSION_STR _STR(VERSION)

    mb.setMinimumWidth(600);
    mb.setWindowTitle("About  Qt BSch3V Modified");
    mb.setTextFormat(Qt::RichText);
    mb.setText(
                "<center><b>Qt-BSch3V Modified Version 0.47.008</b><br><br>"
                "Copyright(C) 2010-2012 Stork < br>"
                "URL:<a href=\"http://storklab.cyber-ninja.jp/\">http://storklab.cyber-ninja.jp/</a><br>"
                "E-Mail  storklab@gmail.com"
                "</center>"
                "<hr>"
                "<center><b>Qt-BSch3V  Version 0.47</b><br><br>"
                "Copyright(C) 2002-2006 H.Okada(Suigyodo)<br>"
                "Qt-BSch3V comes with absolutely no warranty.<br>"
                "URL  <a href=\"http://www.suigyodo.com/online\">http://www.suigyodo.com/online</a><br>"
                "E-Mail  hitoshi@suigyodo.com"
                "</center>"
                );
    mb.setIconPixmap(icon);
    mb.exec();
}

void ApplicationWindow::SetToolButtonState()
{
    BSCH_TOOL_MODE toolmode = ui->xbschview->toolMode();

    //メニュー&ツールバーのチェック状態
    ui->action_Select->setChecked(toolmode == TOOL_SELECTOR);
    ui->action_Drag->setChecked(toolmode == TOOL_DRAG);
    ui->action_Component->setChecked(toolmode == TOOL_COMPONENT);
    ui->action_Bus->setChecked(toolmode == TOOL_BUS);
    ui->action_Wire->setChecked(toolmode == TOOL_WIRE);
    ui->actionD_ash->setChecked(toolmode == TOOL_DASH);
    ui->action_Junction->setChecked(toolmode == TOOL_JUNCTION);
    ui->actionB_us_entry->setChecked(toolmode == TOOL_BENTRY);
    ui->action_Entry->setChecked(toolmode == TOOL_ENTRY);
    ui->action_Tag->setChecked(toolmode == TOOL_TAG);
    ui->action_Label->setChecked(toolmode == TOOL_LABEL);
    ui->actionC_omment->setChecked(toolmode == TOOL_COMMENT);
}

void ApplicationWindow::toolSelectionChanged()
{
    QAction * act = (QAction *)QObject::sender();
    ui->xbschview->setToolMode((BSCH_TOOL_MODE)act->data().toInt());
    SetToolButtonState();
}

//ビューでのカーソル位置の移動情報を受け取る
void ApplicationWindow::cursorMove(int x,int y)
{
    char buff[16];
    sprintf(buff," X:%04d ",x);
    m_plabelX->setText(buff);
    sprintf(buff," Y:%04d ",y);
    m_plabelY->setText(buff);
}


void ApplicationWindow::openFile(QString file)
{
    QString msg;
    if(!QFile::exists(file)) return;
    switch (ui->xbschview->load(file)) {
    case 0:
        m_filename = file;
        setAppCaption();
        break;
    case 1:
        SheetMessageBox::critical(this, "Cannot open file",file + " read error");
        break;
    default:
        SheetMessageBox::critical( this, "Cannot open file",msg + " is not Qt-BSch3V document.");
        break;
    }

    setWindowFilePath(file);
}

void ApplicationWindow::on_action_Open_triggered()
{
    QString defaultPath = g_cfg.getSettings()->value("LAST_PATH",QDir::homePath()).toString();
    QString fn = QFileDialog::getOpenFileName( this,"Open schematic file",defaultPath,"*.ce3");
    if ( !fn.isEmpty() ) {
        g_cfg.getSettings()->setValue("LAST_PATH",QFileInfo(fn).absoluteDir().absolutePath());
        ApplicationWindow * aw = ApplicationWindow::firstEmptyDocWindow();
        if(aw == NULL) aw = new ApplicationWindow();
        aw->show();
        aw->openFile(fn);
        aw->activateWindow();
    } else {
        statusBar()->showMessage( "Loading aborted", 2000 );
    }
}

void ApplicationWindow::on_action_New_triggered()
{
    ApplicationWindow *ed = new ApplicationWindow;
    ed->show();
}

void ApplicationWindow::updateWindowMenu()
{
    ui->menu_Window->clear();
    QList < ApplicationWindow *>::iterator it;
    int i;
    for(it = applicationWindows.begin(),i = 0;
            it != applicationWindows.end(); ++it,++i) {

        QString fn =(*it)->fileName();
        if(fn.isEmpty()) fn="Untitled";
        QAction* action = ui->menu_Window->addAction(fn);
        action->setData(QVariant(i));
        action->setCheckable(true);
        if((*it) == this) {
            action->setChecked(true);
        }

    }
}

void ApplicationWindow::updateEditMenu()
{
    ui->action_Undo->setEnabled(ui->xbschview->canUndo());
    ui->action_Cut->setEnabled(ui->xbschview->canCut());
    ui->action_Copy->setEnabled(ui->xbschview->canCopy());
    ui->action_Paste->setEnabled(ui->xbschview->canPaste());
    ui->action_Delete->setEnabled(ui->xbschview->canDel());
    ui->action_Attribute->setEnabled(ui->xbschview->canAttribute());
    ui->action_Mirror->setEnabled(ui->xbschview->canMirror());
    ui->action_Rotate->setEnabled(ui->xbschview->canRotate());

    ui->actionSetup_Layer->setEnabled(!ui->dockWidgetContents->isVisible());

    ui->actionMove_to_Layer_0->setEnabled(ui->xbschview->canMoveToLayer(0));
    ui->actionMove_to_Layer_1->setEnabled(ui->xbschview->canMoveToLayer(1));
    ui->actionMove_to_Layer_2->setEnabled(ui->xbschview->canMoveToLayer(2));
    ui->actionMove_to_Layer_3->setEnabled(ui->xbschview->canMoveToLayer(3));
    ui->actionMove_to_Layer_4->setEnabled(ui->xbschview->canMoveToLayer(4));
    ui->actionMove_to_Layer_5->setEnabled(ui->xbschview->canMoveToLayer(5));
    ui->actionMove_to_Layer_6->setEnabled(ui->xbschview->canMoveToLayer(6));
    ui->actionMove_to_Layer_7->setEnabled(ui->xbschview->canMoveToLayer(7));
}

void ApplicationWindow::selectActiveWindow(QAction* action)
{
    int id = action->data().toInt();
    int windowCount = applicationWindows.count();
    if(id < 0 || windowCount <= id) return;
    ApplicationWindow * w = applicationWindows[id];
    w->activateWindow();
    w->raise();
}

void ApplicationWindow::on_action_Print_triggered()
{

    QPrinter *printer = new QPrinter( QPrinter::HighResolution );

    QFileInfo qfi(m_filename);
    printer->setDocName(qfi.fileName());
    printer->setOutputFileName(qfi.baseName());

    QPrintDialog pdialog(printer,this);
    if ( pdialog.exec()) {
        QPageSetupDialog psd(printer,this);
        if (psd.exec())
            ui->xbschview->print( printer );
    }
    delete printer;
}

void ApplicationWindow::on_actionP_rint_Option_triggered()
{

    printOptionDlg *dlg = new printOptionDlg(this);
    int mgn_left, mgn_right, mgn_top, mgn_bottom;
    bool color = true;

    if(!dlg) return;

    QSettings *settings = g_cfg.getSettings();

    settings->beginGroup("PrintMargin");
    mgn_top = settings->value("Top",15).toInt();
    mgn_bottom = settings->value("Bottom").toInt();
    mgn_left = settings->value("Left",15).toInt();
    mgn_right = settings->value("Right",15).toInt();
    settings->endGroup();

    settings->beginGroup("PrintOption");
    color = settings->value("Color",true).toBool();
    settings->endGroup();

    dlg->m_top = mgn_top;
    dlg->m_bottom = mgn_bottom;
    dlg->m_left = mgn_left;
    dlg->m_right = mgn_right;
    dlg->m_color = (bool)color;

    if(dlg->exec()) {
        settings->beginGroup("PrintMargin");
        settings->setValue("Top",dlg->m_top);
        settings->setValue("Bottom",dlg->m_bottom);
        settings->setValue("Left",dlg->m_left);
        settings->setValue("Right",dlg->m_right);
        settings->endGroup();

        settings->beginGroup("PrintOption");
        settings->setValue("Color",dlg->m_color);
        settings->endGroup();
    }
    delete dlg;
}

void ApplicationWindow::on_actionLibrary_triggered()
{
    QStringList list;
    for (int i = 0; i < g_LibraryCount(); i++) {
        list << g_LibraryInfo(i)->libraryName();
    }

    SetLibraryDialog *dlg = new SetLibraryDialog(this);
    dlg->setFiles(list);
    if(dlg->exec()) {
        list = dlg->selectedFiles();
        QStringList::Iterator it = list.begin();
        for(int n = 0; n < MAX_LIB; n++) {
            char szKey[32];
            sprintf(szKey,"LIB%d",n);
            if(it != list.end()) {
                if(!(*it).isEmpty()) {
                    string strData = string((*it).toLocal8Bit());
                    g_cfg.setString("Library",szKey,strData);
                } else {
                    g_cfg.setString("Library",szKey,"");
                }
                ++it;
            } else {
                g_cfg.setString("Library",szKey,"");
            }
        }
        delete dlg;
        g_FreeLibrary();
        g_ReadLibrary();
    } else {
        delete dlg;
    }
}

bool ApplicationWindow::saveFile()
{
    if ( m_filename.isEmpty() ) {
        return saveFileAs();
    } else {
        g_cfg.getSettings()->sync();
        return ui->xbschview->save(m_filename);
    }
}

bool ApplicationWindow::saveFileAs()
{
    QString defaultPath = g_cfg.getSettings()->value("LAST_PATH",QDir::homePath()).toString();
    QString fn = QFileDialog::getSaveFileName( this,QString::null,defaultPath,"*.ce3");
    if ( !fn.isEmpty() ) {
        m_filename = fn;
        if(!ui->xbschview->save(fn)) {
            m_filename = fn;
            setAppCaption();
            setWindowFilePath(m_filename);
            return false;
        }
        return true;
    }
    return true;
}

void ApplicationWindow::on_action_Export_image_triggered()
{
    QString imagefilename = m_filename;
    if ( imagefilename.isEmpty() ) {
        imagefilename="untitled";
    }
    QFileInfo finfo(imagefilename);
    QString path = finfo.absolutePath() + "/";
    if(path.isEmpty()) path = QDir::homePath()+"/";
    path += finfo.baseName() + ".png";
    QString fn = QFileDialog::getSaveFileName( this,tr("Export as image (PNG, XPM, JPEG, BMP, TIFF)"),path,tr("Image (*.png *.xpm *.jpg *.bmp *.tiff)"));
    if(!fn.isEmpty()) {
        ui->xbschview->exportImage(fn);
    }
}

void ApplicationWindow::on_actionZoom_In_triggered()
{
    ui->xbschview->ZoomIn(true);
    ui->actionZoom_In->setEnabled(ui->xbschview->CanZoomIn());
    ui->actionZoom_Out->setEnabled(ui->xbschview->CanZoomOut());
}

void ApplicationWindow::on_actionZoom_Out_triggered()
{
    ui->xbschview->ZoomOut(true);
    ui->actionZoom_In->setEnabled(ui->xbschview->CanZoomIn());
    ui->actionZoom_Out->setEnabled(ui->xbschview->CanZoomOut());
}

void ApplicationWindow::on_action_Grid_triggered()
{
    bool bGridOn = !ui->xbschview->displayGrid();
    ui->action_Grid->setChecked(bGridOn);
    ui->xbschview->setDisplayGrid(bGridOn);
    g_cfg.setInt("VIEW","GRID",bGridOn);
}

void ApplicationWindow::on_actionS_nap_triggered()
{
    bool bCheck = !ui->xbschview->snapGrid();
    ui->actionS_nap->setChecked(bCheck);
    ui->xbschview->setSnapGrid(bCheck);
    g_cfg.getSettings()->beginGroup("SET");
    g_cfg.getSettings()->setValue("SNAP", bCheck);
    g_cfg.getSettings()->endGroup();
}

void ApplicationWindow::on_actionL_abel_Font_triggered()
{
    QFont font = g_cfg.getSettings()->value("SmallFont",QFont("San-serif",8)).value < QFont>();

    bool ok;
    font = QFontDialog::getFont(&ok,font,this);
    if(ok) {
        g_cfg.getSettings()->setValue("SmallFont",font);
        g_SmallFont.SetFont(font);
        foreach(ApplicationWindow *aw,applicationWindows) {
            aw->ui->xbschview->RefreshView();
        }
    }
}

void ApplicationWindow::on_action_Default_comment_font_triggered()
{

    QFont font;

    font = g_cfg.getSettings()->value("Font",QFont("San-serif",14)).value < QFont>();

    bool ok;
    font = QFontDialog::getFont(&ok,font,this);
    if(ok) {
        g_cfg.setString("Font","Name",string(font.family().toLocal8Bit()));
        g_cfg.getSettings()->setValue("Font",font);
        ui->xbschview->setDefaultFont();
    }
}

void ApplicationWindow::on_actionDarken_non_Active_Layer_triggered(bool stat)
{
    qDebug() << stat;
    ui->xbschview->setDarkenNonActiveLayer(stat);
    g_cfg.setInt("VIEW","DARKEN_NONACTIVE_LAYER",(stat ? 1 : 0));
    ui->xbschview->RefreshView();
}


void ApplicationWindow::keyPressEvent ( QKeyEvent * event )
{
    //qDebug() << event->key();
    QMainWindow::keyPressEvent(event);
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        ui->xbschview->del();
    }
}

