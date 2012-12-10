/****************************************************************************
    Qt-LCoV Library Editor for Qt-BSch3V
    Copyright (C) 2002-2006 H.Okada

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

#include "application.h"

#include <QtGui>

#include <qimage.h>
#include <qpixmap.h>
#include <qtoolbutton.h>
//#include <qbutton.h>
#include <qmenubar.h>
#include <qfile.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
//#include <qprinter.h>
#include <QApplication>
#include <qtextstream.h>
#include <qpainter.h>
//#include <qwhatsthis.h>
#include <qlayout.h>
#include <qclipboard.h>
#include <qfontdialog.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QAction>
#include <QToolBar>

#include "sheetmessagebox.h"

#include "componentview.h"
#include "lcolistboxitem.h"
#include "ce3io.h"
#include "xbschfont.h"
#include "cfgdata.h"
//#include "prop_comp.h"
#include "propcomponentdlg.h"
#include "add_alias.h"
#include "componentlistmodel.h"


//Static メンバー変数
QWidgetList ApplicationWindow::m_appWindowList;

bool ApplicationWindow::isEmptyDocWindow()
{
    return  m_filename.isEmpty() && m_LCoVDoc.m_CompLib.count() == 0;
}


ApplicationWindow* ApplicationWindow::firstEmptyDocWindow()
{
    int windowCount = m_appWindowList.count();
    int n;
    for(n = 0; n < windowCount; n++) {
        ApplicationWindow * w = (ApplicationWindow*)m_appWindowList.at(n);
        if(w->isEmptyDocWindow())return w;
    }
    return NULL;
}

ApplicationWindow::ApplicationWindow()
    : QMainWindow( NULL , Qt::Window )
{
    //printer = new QPrinter( QPrinter::HighResolution );
    //    QPixmap openIcon, saveIcon, printIcon;
    QPixmap toolSelectIcon, toolPinIcon, toolLineIcon,
            toolEllipseIcon, toolPolygonIcon, toolArcIcon, toolHandleIcon,toolTextIcon,
            upIcon, downIcon;

    setWindowIcon(QPixmap(":/images/lcoicon.png"));

    toolSelectIcon = QPixmap( ":/images/tool_select.png" );
    toolPinIcon = QPixmap( ":/images/tool_pin.png" );
    toolLineIcon = QPixmap( ":/images/tool_line.png" );
    toolEllipseIcon = QPixmap( ":/images/tool_ellipse.png" );
    toolPolygonIcon = QPixmap( ":/images/tool_polygon.png" );
    toolArcIcon = QPixmap( ":/images/tool_arc.png" );
    toolTextIcon = QPixmap( ":/images/tool_text.png" );
    toolHandleIcon = QPixmap( ":/images/tool_handle.png" );
    upIcon = QPixmap( ":/images/up_image.png" );
    downIcon = QPixmap( ":/images/down_image.png" );


    // --- Make Actions --------------------------------------------------------------

    // File Menu -----------
    m_menuFileNew = new QAction("&New",this);
    m_menuFileNew->setShortcuts(QKeySequence::New);
    connect(m_menuFileNew, SIGNAL(triggered()), this, SLOT(newDoc()));

    m_menuFileOpen = new QAction("&Open",this);
    m_menuFileOpen->setShortcut(QKeySequence::Open);
    connect(m_menuFileOpen,SIGNAL(triggered()),this,SLOT(choose()));

    m_menuFileSave = new QAction("&Save",this);
    m_menuFileSave->setShortcut(QKeySequence::Save);
    connect(m_menuFileSave,SIGNAL(triggered()),this,SLOT(save()));

    m_menuFileSaveAs = new QAction("Save &As",this);
    m_menuFileSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(m_menuFileSaveAs,SIGNAL(triggered()),this,SLOT(saveAs()));

    m_menuFileClose = new QAction("&Close",this);
    m_menuFileClose->setShortcut(QKeySequence::Close);
    connect(m_menuFileClose,SIGNAL(triggered()),this,SLOT(close()));

    m_menuFileQuit = new QAction("&Quit",this);
    m_menuFileQuit->setShortcut(QKeySequence::Quit);
    connect(m_menuFileQuit,SIGNAL(triggered()),qApp,SLOT(closeAllWindows()));

    // Edit Menu -----------
    m_menuEditUndo = new QAction("&Undo",this);
    m_menuEditUndo->setShortcut(QKeySequence::Undo);
    connect(m_menuEditUndo,SIGNAL(triggered()),this,SLOT(editUndo()));

    m_menuEditCut = new QAction("Cu&t",this);
    m_menuEditCut->setShortcut(QKeySequence::Cut);
    connect(m_menuEditCut,SIGNAL(triggered()),this,SLOT(editCut()));

    m_menuEditCopy = new QAction("&Copy",this);
    m_menuEditCopy->setShortcut(QKeySequence::Copy);
    connect(m_menuEditCopy,SIGNAL(triggered()),this,SLOT(editCopy()));

    m_menuEditPaste = new QAction("&Paste",this);
    m_menuEditPaste->setShortcut(QKeySequence::Paste);
    connect(m_menuEditPaste,SIGNAL(triggered()),this,SLOT(editPaste()));

    m_menuEditDelete = new QAction("&Delete",this);
    m_menuEditDelete->setShortcut(Qt::Key_Backspace);
    connect(m_menuEditDelete,SIGNAL(triggered()),this,SLOT(editDelete()));

    m_menuEditMirror = new QAction("&Mirror",this);
    m_menuEditMirror->setShortcut(tr("Ctrl+m"));
    connect(m_menuEditMirror,SIGNAL(triggered()),this,SLOT(editMirror()));

    m_menuEditRotate = new QAction("&Rotate",this);
    m_menuEditRotate->setShortcut(tr("Ctrl+r"));
    connect(m_menuEditRotate,SIGNAL(triggered()),this,SLOT(editRotate()));

    m_menuEditProperty = new QAction("Property",this);
    m_menuEditProperty->setShortcut(tr("Ctrl+p"));
    connect(m_menuEditProperty,SIGNAL(triggered()),this,SLOT(editProperty()));

    m_menuEditCutComponent = new QAction("Cut Component",this);
    m_menuEditCutComponent->setShortcut(tr("Ctrl+Shift+x"));
    connect(m_menuEditCutComponent,SIGNAL(triggered()),this,SLOT(editCompCut()));

    m_menuEditCopyComponent = new QAction("Copy Component",this);
    m_menuEditCopyComponent->setShortcut(tr("Ctrl+Shift+c"));
    connect(m_menuEditCopyComponent,SIGNAL(triggered()),this,SLOT(editCompCopy()));

    m_menuEditPasteComponent = new QAction("Paste Component",this);
    m_menuEditPasteComponent->setShortcut(tr("Ctrl+Shift+v"));
    connect(m_menuEditPasteComponent,SIGNAL(triggered()),this,SLOT(editCompPaste()));

    m_menuEditNewComponent = new QAction("New Component",this);
    m_menuEditNewComponent->setShortcut(tr("Ctrl+Shift+n"));
    connect(m_menuEditNewComponent,SIGNAL(triggered()),this,SLOT(editCompNew()));

    m_menuEditComponentProperty = new QAction("Component Property",this);
    connect(m_menuEditComponentProperty,SIGNAL(triggered()),this,SLOT(editCompProperty()));

    m_menuEditAddAlias = new QAction("Add Alias",this);
    connect(m_menuEditAddAlias,SIGNAL(triggered()),this,SLOT(editAddAlias()));

    // View Menu -----------
    m_menuViewZoomIn = new QAction("Zoom &In",this);
    m_menuViewZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(m_menuViewZoomIn,SIGNAL(triggered()),this,SLOT(viewZoomIn()));

    m_menuViewZoomOut = new QAction("Zoom &Out",this);
    m_menuViewZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(m_menuViewZoomOut,SIGNAL(triggered()),this,SLOT(viewZoomOut()));

    // Tool Menu -----------
    m_menuToolSelect = new QAction("&Select",this);
    m_menuToolSelect->setShortcut(Qt::Key_Escape);
    m_menuToolSelect->setCheckable(true);
    m_menuToolSelect->setIcon(toolSelectIcon);
    m_menuToolSelect->setIconVisibleInMenu(false);
    connect(m_menuToolSelect,SIGNAL(triggered()),this,SLOT(toolSelect()));

    m_menuToolPin = new QAction("&Pin",this);
    m_menuToolPin->setShortcut(Qt::Key_P);
    m_menuToolPin->setCheckable(true);
    m_menuToolPin->setIcon(toolPinIcon);
    m_menuToolPin->setIconVisibleInMenu(false);
    connect(m_menuToolPin,SIGNAL(triggered()),this,SLOT(toolPin()));

    m_menuToolLine = new QAction("&Line",this);
    m_menuToolLine->setShortcut(Qt::Key_L);
    m_menuToolLine->setCheckable(true);
    m_menuToolLine->setIcon(toolLineIcon);
    m_menuToolLine->setIconVisibleInMenu(false);
    connect(m_menuToolLine,SIGNAL(triggered()),this,SLOT(toolLine()));

    m_menuToolEllipse = new QAction("&Ellipse",this);
    m_menuToolEllipse->setShortcut(Qt::Key_E);
    m_menuToolEllipse->setCheckable(true);
    m_menuToolEllipse->setIcon(toolEllipseIcon);
    m_menuToolEllipse->setIconVisibleInMenu(false);
    connect(m_menuToolEllipse,SIGNAL(triggered()),this,SLOT(toolEllipse()));

    m_menuToolPolygon = new QAction("Poly&gon",this);
    m_menuToolPolygon->setShortcut(Qt::Key_G);
    m_menuToolPolygon->setCheckable(true);
    m_menuToolPolygon->setIcon(toolPolygonIcon);
    m_menuToolPolygon->setIconVisibleInMenu(false);
    connect(m_menuToolPolygon,SIGNAL(triggered()),this,SLOT(toolPolygon()));

    m_menuToolArc = new QAction("&Arc",this);
    m_menuToolArc->setShortcut(Qt::Key_A);
    m_menuToolArc->setCheckable(true);
    m_menuToolArc->setIcon(toolArcIcon);
    m_menuToolArc->setIconVisibleInMenu(false);
    connect(m_menuToolArc,SIGNAL(triggered()),this,SLOT(toolArc()));

    m_menuToolText = new QAction("&Text",this);
    m_menuToolText->setShortcut(Qt::Key_T);
    m_menuToolText->setCheckable(true);
    m_menuToolText->setIcon(toolTextIcon);
    m_menuToolText->setIconVisibleInMenu(false);
    connect(m_menuToolText,SIGNAL(triggered()),this,SLOT(toolText()));

    m_menuToolMoveNode = new QAction("Move &Node",this);
    m_menuToolMoveNode->setCheckable(true);
    m_menuToolMoveNode->setIcon(toolHandleIcon);
    m_menuToolMoveNode->setIconVisibleInMenu(false);
    connect(m_menuToolMoveNode,SIGNAL(triggered()),this,SLOT(toolHandle()));

    m_menuToolCreateBitPattern = new QAction("&Create Bit Pattern",this);
    connect(m_menuToolCreateBitPattern,SIGNAL(triggered()),this,SLOT(toolCreateBitPattern()));

    // Set Menu -----------
    m_menuSetFont = new QAction("&Font...",this);
    connect(m_menuSetFont,SIGNAL(triggered()),this,SLOT(setFont()));

    // Set Menu -----------
    m_menuHelpAbout = new QAction("&About",this);
    connect(m_menuHelpAbout,SIGNAL(triggered()),this,SLOT(about()));
    m_menuHelpAboutQt = new QAction("About &Qt",this);
    connect(m_menuHelpAboutQt,SIGNAL(triggered()),this,SLOT(aboutQt()));


    // ------ Tool Bar -----------------------------------------------------------------------

    QToolBar * pToolBar = new QToolBar( this);
    pToolBar->setObjectName("main toolbar" );
    setUnifiedTitleAndToolBarOnMac(true);
    pToolBar->setIconSize(QSize(16,16));
    addToolBar(pToolBar);
    pToolBar->addAction(m_menuToolSelect);
    pToolBar->addAction(m_menuToolPin);
    pToolBar->addAction(m_menuToolLine);
    pToolBar->addAction(m_menuToolEllipse);
    pToolBar->addAction(m_menuToolPolygon);
    pToolBar->addAction(m_menuToolArc);
    pToolBar->addAction(m_menuToolText);
    pToolBar->addSeparator();
    pToolBar->addAction(m_menuToolMoveNode);

    //FILEメニュー
    QMenu * file = new QMenu("&File", this );
    menuBar()->addMenu(file);
    file->addAction(m_menuFileNew);
    file->addAction(m_menuFileOpen);
    file->addSeparator();
    file->addAction(m_menuFileSave);
    file->addAction(m_menuFileSaveAs);
    file->addSeparator();
    file->addAction(m_menuFileClose);
    file->addAction(m_menuFileQuit);

    //EDITメニュー
    QMenu * edit = new QMenu("&Edit", this );
    menuBar()->addMenu(edit);
    edit->addAction(m_menuEditUndo);
    edit->addAction(m_menuEditCut);
    edit->addAction(m_menuEditCopy);
    edit->addAction(m_menuEditPaste);
    edit->addAction(m_menuEditDelete);
    edit->addAction(m_menuEditMirror);
    edit->addAction(m_menuEditRotate);
    edit->addAction(m_menuEditProperty);
    edit->addSeparator();
    edit->addAction(m_menuEditCutComponent);
    edit->addAction(m_menuEditCopyComponent);
    edit->addAction(m_menuEditPasteComponent);
    edit->addSeparator();
    edit->addAction(m_menuEditNewComponent);
    edit->addAction(m_menuEditComponentProperty);
    edit->addSeparator();
    edit->addAction(m_menuEditAddAlias);

    //VIEWメニュー
    QMenu * viewMenu = new QMenu("&View", this );
    menuBar()->addMenu(viewMenu);
    viewMenu->addAction(m_menuViewZoomIn);
    viewMenu->addAction(m_menuViewZoomOut);

    //TOOLメニュー
    QMenu * toolMenu = new QMenu("&Tool", this );
    menuBar()->addMenu(toolMenu);
    toolMenu->addAction(m_menuToolSelect);
    toolMenu->addAction(m_menuToolPin);
    toolMenu->addAction(m_menuToolLine);
    toolMenu->addAction(m_menuToolEllipse);
    toolMenu->addAction(m_menuToolPolygon);
    toolMenu->addAction(m_menuToolArc);
    toolMenu->addAction(m_menuToolText);
    toolMenu->addSeparator();
    toolMenu->addAction(m_menuToolMoveNode);
    toolMenu->addSeparator();
    toolMenu->addAction(m_menuToolCreateBitPattern);

    //SETメニュー
    QMenu * setMenu = new QMenu("&Set", this );
    menuBar()->addMenu(setMenu);
    setMenu->addAction(m_menuSetFont);


    //WINDOWメニュー
    m_menuWindow = new QMenu("&Window", this );
    menuBar()->addMenu(m_menuWindow);
    QObject::connect(m_menuWindow,SIGNAL(aboutToShow ()),
                     this,SLOT(setupWindowMenu()));
    QObject::connect(m_menuWindow,SIGNAL(triggered ( QAction *)),
                     this,SLOT(selectActiveDocument( QAction *)));


    menuBar()->addSeparator();

    QMenu *help = new QMenu("&Help", this );
    menuBar()->addMenu(help);
    help->addAction(m_menuHelpAbout);
    help->addAction(m_menuHelpAboutQt);

    m_mainWidget = new QWidget(this);
    m_mainWidget->setObjectName("mainWidget");
    setCentralWidget( m_mainWidget  );
    m_gridLayout= new QGridLayout( m_mainWidget);



    m_gridLayout->setSpacing( 4 );
    m_gridLayout->setMargin( 3 );

    m_buttonUp = new QToolButton( m_mainWidget );
    m_buttonUp->setObjectName("button_up");
    m_buttonUp->setIcon(upIcon);
    m_gridLayout->addWidget(m_buttonUp, 0, 0);
    m_buttonUp->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);

    m_buttonDown = new QToolButton( m_mainWidget);
    m_buttonDown->setObjectName("button_down" );
    m_buttonDown->setIcon(downIcon );
    m_gridLayout->addWidget(m_buttonDown, 0, 1);
    m_buttonDown->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);

    m_componentListBox = new QListView(  m_mainWidget );
    m_componentListBox->setObjectName("ListBox1");
    m_componentListBox->setMinimumSize(180, 300);
    _componentListModel = new ComponentListModel(this);
    m_componentListBox->setModel(_componentListModel);

    //m_componentListBox->setVScrollBarMode(Q3ScrollView::AlwaysOn);
    //m_gridLayout->addMultiCellWidget ( m_componentListBox, 1, 1, 0, 1);
    m_gridLayout->addWidget(m_componentListBox,1,0,1,2);

    m_componentView = new SComponentView(m_mainWidget);
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(m_componentView);
    scrollArea->setWidgetResizable(true);
    m_gridLayout->addWidget(scrollArea,0,2,2,1);
    //m_gridLayout->addMultiCellWidget(scrollArea,0,1,2,2);


    // signals and slots connections
    //connect(m_componentListBox , SIGNAL( selectionChanged () ),this,SLOT( listboxSelectionChanged()    ));
    //connect(m_componentListBox,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),SLOT(listboxSelectionChanged()));
    connect(m_componentListBox->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(listboxSelectionChanged()));
    connect(m_buttonUp , SIGNAL( clicked () ),this,SLOT( onButtonUp() ));
    connect(m_buttonDown , SIGNAL( clicked () ),this,SLOT( onButtonDown() ));

    resize(600 , 400 );
    setWindowTitle("[*] Qt-LCoV");
    setToolButtonState();
    m_appWindowList.append(this);
}


ApplicationWindow::~ApplicationWindow()
{
    int windowCount = m_appWindowList.count();
    int n;
    for(n = 0; n < windowCount; n++) {
        QWidget * w = m_appWindowList.at(n);
        if(w == this) {
            m_appWindowList.removeAll(w);
            break;
        }
    }

    //delete printer;
}


bool ApplicationWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (qApp == obj) {
        if (event->type() == QEvent::FileOpen) {
            QFileOpenEvent *openevent = static_cast < QFileOpenEvent *>(event);
            ApplicationWindow * aw =ApplicationWindow::firstEmptyDocWindow();
            if(aw == NULL) aw = new ApplicationWindow();
            aw->show();
            aw->load(openevent->file());
            aw->activateWindow();
            event->accept();
            return true;
        } else {
            return false;
        }
    } else {
        return QMainWindow::eventFilter(obj,event);
    }
}


QString ApplicationWindow::fileName()
{
    return m_filename;
}

void ApplicationWindow::newDoc()
{
    ApplicationWindow *ed = new ApplicationWindow;
    ed->setWindowTitle("[*] Qt-LCoV");
    ed->show();
}

void ApplicationWindow::choose()
{
    QString fn = QFileDialog::getOpenFileName( this,tr("Open library file"),g_cfg.getSettings()->value("FILEPATH",QDir::homePath()).toString(),"*.lb3");
    //QString fn = Q3FileDialog::getOpenFileName( QString::null, "*.lb3",
    //    	    	       this);
    if ( !fn.isEmpty() ) {
        g_cfg.getSettings()->setValue("FILEPATH",fn);
        if(m_LCoVDoc.m_CompLib.count()>0) {
            ApplicationWindow *ed = new ApplicationWindow;
            ed->setWindowTitle("[*] Qt-LCoV");
            ed->show();
            ed->load( fn );
        } else {
            load( fn );
        }
    }
    //    else
    //    statusBar()->message( "Loading aborted", 2000 );
}

void ApplicationWindow::keyPressEvent ( QKeyEvent * e )
{
    switch(e->key()) {
    case Qt::Key_Escape :
        if(!m_componentView->onKeyEscape())
            toolSelect();
        break;
    default:
        ;
    }
}

//上移動ボタンが押されたときのハンドラ
void  ApplicationWindow::onButtonUp()
{
#if 0
    int nSelected = m_componentListBox->currentRow();
    if(nSelected < 1) return;
    SLCoListBoxItem* itemSelected = (SLCoListBoxItem*)m_componentListBox->currentItem();
    //SLCoListBoxItem* listItem = new SLCoListBoxItem;
    //listItem->m_pCompIndex = itemSelected->m_pCompIndex;
    //listItem->setText(itemSelected->m_pCompIndex->name());
    //m_componentListBox->insertItem(nSelected-1,listItem);
    //m_componentListBox->setCurrentRow(nSelected-1);
    m_componentListBox->takeItem(nSelected);
    m_componentListBox->insertItem(nSelected-1,itemSelected);
    m_componentListBox->setCurrentRow(nSelected-1);
#endif
    _componentListModel->onButtonUp(m_componentListBox->selectionModel());
    m_LCoVDoc.m_bDirty = true;
    this->setWindowModified(true);
}

//下移動ボタンが押されたときのハンドラ
void  ApplicationWindow::onButtonDown()
{
#if 0
    int nSelected = m_componentListBox->currentRow();
    if(nSelected < 0) return;
    int nCount = m_componentListBox->count();
    if(nSelected > nCount -2) return;

    SLCoListBoxItem* itemSelected = (SLCoListBoxItem*)m_componentListBox->item(nSelected);
    //SLCoListBoxItem* listItem = new SLCoListBoxItem;
    //listItem->m_pCompIndex = itemSelected->m_pCompIndex;
    //m_componentListBox->insertItem(nSelected+2,listItem);
    //m_componentListBox->setCurrentRow(nSelected+2);
    m_componentListBox->takeItem(nSelected);
    m_componentListBox->insertItem(nSelected+1,itemSelected);
    m_componentListBox->setCurrentRow(nSelected+1);
#endif
    _componentListModel->onButtonDown(m_componentListBox->selectionModel());
    m_LCoVDoc.m_bDirty = true;
    this->setWindowModified(true);
}

//アプリケーションのキャプションのセット
void ApplicationWindow::setAppCaption(const QString &fileName )
{
    QString title = "[*] Qt-LCoV";
    if(!fileName.isEmpty()) {
        title += " - ";
        title += fileName;
    }
    setWindowTitle(title);
}

//部品インデックスをリストボックスにセット
void ApplicationWindow::setCompIndexToListBox()
{
    //m_componentListBox->clear();
    _componentListModel->clearComponent();
    SCompIndexListIterator iteIndex =m_LCoVDoc.m_CompLib.m_listCompIndex.begin();
    while(iteIndex != m_LCoVDoc.m_CompLib.m_listCompIndex.end()) {
        _componentListModel->addComponent(*iteIndex);
        iteIndex++;
    }
}

//リストボックスの選択が変更されたときに呼ばれる
void ApplicationWindow::listboxSelectionChanged()
{
    //qDebug("listboxSelectionChanged()");
    // TODO: rewrite here

    QList < SCompIndex *> selectedComp = _componentListModel->selectedComponentList(m_componentListBox->selectionModel());
    if(selectedComp.size() > 0) {

        if(m_componentView->m_pCompIndex != NULL) {
            m_componentView->releaseTemp();
            if(m_componentView->m_doc.m_dirtyFlag) {
                m_componentView->saveCompInfo();
                m_componentView->m_doc.m_dirtyFlag = false;
                m_LCoVDoc.m_bDirty = true;
                this->setWindowModified(true);
            }
        }
        SCompIndex *pIndex = selectedComp[0];

        int i;
        QList < SCompIndex *>::const_iterator it;
        for(it = _componentListModel->componentList().begin(),i = 0;
                it != _componentListModel->componentList().end();
                ++it,++i) {
            if(pIndex != *it) {
                _componentListModel->setAlias(i,pIndex->compInfo() == (*it)->compInfo());
            } else {
                _componentListModel->setAlias(i,false);
            }
        }
        //qDebug() << pIndex->name() << selectedComp;
        m_componentView->setCompIndex(pIndex);
    } else {
        m_componentView->setCompIndex(NULL);
    }
#if 0
    int nSelected = m_componentListBox->currentRow();
    if(nSelected >= 0) {

        if(m_componentView->m_pCompIndex != NULL) {
            m_componentView->releaseTemp();
            if(m_componentView->m_doc.m_dirtyFlag) {
                m_componentView->saveCompInfo();
                m_componentView->m_doc.m_dirtyFlag = false;
                m_LCoVDoc.m_bDirty = true;
                this->setWindowModified(true);
            }
        }

        //qDebug("lcoDialog::listboxSelectionChanged()  %d",nSelected);
        SLCoListBoxItem* itemSelected = (SLCoListBoxItem*)m_componentListBox->item(nSelected);
        int nCount = m_componentListBox->count();
        int n;
        for(n = 0; n < nCount; n++) {
            SLCoListBoxItem* listItem = (SLCoListBoxItem*)m_componentListBox->item(n);
            if(n == nSelected) {
                listItem->m_bAlias = false;
            } else {
                listItem->m_bAlias = (listItem->m_pCompIndex->compInfo() == itemSelected->m_pCompIndex->compInfo());
            }
        }
        m_componentView->setCompIndex(itemSelected->m_pCompIndex);
    } else {
        m_componentView->setCompIndex(NULL);
    }
#endif
    m_componentListBox->viewport()->update();
    //    if(CompEdit->modified()) m_bDirty = true;
    toolSelect();
}


void ApplicationWindow::load( const QString &fileName )
{
    m_componentView->setCompIndex(NULL);
    //m_componentListBox->clear();    //先にリストボックスを空にしておかないとセグメンテーションエラーになる。
    _componentListModel->clearComponent();
    m_LCoVDoc.m_CompLib.deleteContent();
    m_LCoVDoc.m_CompLib.readLibraryFile(fileName.toLocal8Bit());
    m_LCoVDoc.m_bDirty = false;
    this->setWindowModified(false);
    setAppCaption(fileName.section("/",-1));
    m_filename = fileName;
    setCompIndexToListBox();

    int nCount = _componentListModel->componentList().size();
    if(nCount > 0) {
        toolSelect();
        //m_componentListBox->setCurrentRow(0);
        m_componentListBox->selectionModel()->select(_componentListModel->index(0),QItemSelectionModel::Select);
        if(_componentListModel->componentList().size() > 0) {
            m_componentView->setCompIndex(_componentListModel->componentList()[0]);
        }
    }

}


void ApplicationWindow::save()
{
    if (m_filename.isEmpty() ) {
        saveAs();
        return;
    }
    doSave();
}


void ApplicationWindow::saveAs()
{

    QSettings preference;
    QString fn = QFileDialog::getSaveFileName( this,tr("Save library file"),
                 g_cfg.getSettings()->value("FILEPATH",QDir::homePath()).toString(),
                 "*.lb3");

    if ( fn.isEmpty() ) return;
    g_cfg.getSettings()->setValue("FILEPATH",fn);
    QFileInfo finfo(fn);
    if(finfo.suffix().isEmpty()) {    //「xxxx.」と「xxxx」の両方を拡張子なしと判定してしまう
        if(fn.right(1) != ".") fn += ".";    	// 最後の文字が「.」でなければ「.」を追加。
        fn += "lb3";    	    	    // 拡張子を追加。
    }
    if(QFile::exists (fn)) {
        QString msg = fn;
        msg += " already exists. Overwrite?";
        switch( QMessageBox::information( this, "Save as",msg,
                                          QMessageBox::Yes|QMessageBox::Cancel,QMessageBox::Cancel) ) {

        case QMessageBox::Yes:
            break;
        case QMessageBox::Cancel:
        default:
            return;
        }
    }

    m_filename = fn;
    setAppCaption(fn);
    doSave();
}

//ファイル保存の実際
void ApplicationWindow::doSave()
{
    if(m_componentView->m_pCompIndex != NULL) {
        m_componentView->releaseTemp();
        if(m_componentView->m_doc.m_dirtyFlag) {
            m_componentView->saveCompInfo();
            m_componentView->m_doc.m_dirtyFlag = false;
            m_LCoVDoc.m_bDirty = true;
            this->setWindowModified(true);
        }
    }


    int nCount = _componentListModel->componentList().size();
    int n,m;


    FILE* fp;
    if((fp = fopen(m_filename.toLocal8Bit(),"w")) == NULL) return ;
    SWriteCE3 wce3;
    wce3.Attach(fp);
    wce3.WriteRecord("+BSCH3_LIB_V.1.0");
    wce3.WriteEOL();

    wce3.WriteRecordString("PROP",m_LCoVDoc.m_CompLib.m_prop_string);
    wce3.WriteEOL();

    for(n = 0; n < nCount; n++) {
        SCompIndex* pIndex = _componentListModel->componentList()[n];
        if(!pIndex)continue;

        string name = pIndex->name();
        if(name.empty())continue;
        SCompInfo* pInfo = pIndex->compInfo();
        if(!pInfo)continue;
        bool bAlias = false;
        string aliasFor;
        for(m = 0; m < n && m < nCount; m++) {
            //SLCoListBoxItem* listItem2 = (SLCoListBoxItem*)m_componentListBox->item(m);
            SCompIndex* pIndex2 = _componentListModel->componentList()[m];
            if(!pIndex2)continue;
            if(pIndex2->compInfo() == pInfo) {
                aliasFor = pIndex2->name();    //書き込み済みの名前
                if(aliasFor.empty())continue;
                bAlias = true;
                break;
            }
        }
        if(bAlias) {
            SCompLib::WriteAlias(wce3,name.c_str(),aliasFor.c_str());
        } else {
            pInfo->writeCe3(wce3,name.c_str());
        }
    }
    wce3.WriteRecord("-BSCH3_LIB_V.1.0");
    wce3.WriteEOL();

    fclose(fp);
    m_LCoVDoc.m_bDirty = false;
    this->setWindowModified(false);

    //    m_wndView.m_doc.m_dirtyFlag = false;
    return ;
}


//EDIT-Undo  ハンドラ
void ApplicationWindow::editUndo()
{
    m_componentView->doUndo();
}

//EDIT-Cut  ハンドラ
void ApplicationWindow::editCut()
{
    m_componentView->doCopy();
    m_componentView->doDelete();
}

//EDIT-Copy  ハンドラ
void ApplicationWindow::editCopy()
{
    m_componentView->doCopy();
}

//EDIT-Paste  ハンドラ
void ApplicationWindow::editPaste()
{
    if(!m_componentView->canPaste()) return;
    toolSelect();
    m_componentView->doPaste();

}

//EDIT-Delete  ハンドラ
void ApplicationWindow::editDelete()
{
    m_componentView->doDelete();
}

//EDIT- Mirror ハンドラ
void ApplicationWindow::editMirror()
{
    m_componentView->doMirror();
}

//EDIT-Rotate  ハンドラ
void ApplicationWindow::editRotate()
{
    m_componentView->doRotate();
}

//EDIT-Property  ハンドラ
void ApplicationWindow::editProperty()
{
    m_componentView->editProperty();
}

void ApplicationWindow::editCompCut()
//EDIT-ComponentCut  ハンドラ
{
    doComponentCopy();
    doComponentDelete();
}

//EDIT-ComponentCopy  ハンドラ
void ApplicationWindow::editCompCopy()
{
    doComponentCopy();
}

//EDIT-ComponentPaste  ハンドラ
void ApplicationWindow::editCompPaste()
{
    doComponentPaste();
}

//EDIT-ComponentNew  ハンドラ
void ApplicationWindow::editCompNew()
{
    prop_componentdlg* dlg = new prop_componentdlg(this);
    //    prop_comp* dlg = new prop_comp(this,0,true);
    //    // 新規部品のデフォルト設定
    //    dlg->editName->setText("NAME");
    //    dlg->editRef->setText("U");
    //    dlg->editWidth->setText("2");
    //    dlg->editHeight->setText("2");
    //    dlg->editBlock->setText("1");
    //    dlg->checkUsePtn->setChecked(false);

    if(dlg->exec()) {
        // 部品情報クラスの生成
        SCompInfo* pComp = new SCompInfo;

        SSize sizeComp = SSize(dlg->width(),dlg->height());
        bool usePtn = dlg->usePattern();
        bool noBitPtn = !(dlg->useBitPattern());
        pComp->setSize(sizeComp,usePtn,noBitPtn);
        pComp->setUsePattern(usePtn,noBitPtn);
        pComp->setRef(dlg->ref().toLatin1());
        pComp->setBlock(dlg->block());
        string name = dlg->name().toStdString();

        //    	SSize sizeComp = SSize(    dlg->editWidth->text().toInt(),
        //    	    	    	         dlg->editHeight->text().toInt());
        //    	bool usePtn = dlg->checkUsePtn->isChecked();
        //    	pComp->setSize(sizeComp,usePtn);
        //    	pComp->setUsePattern(usePtn);
        //    	pComp->setRef(dlg->editRef->text().latin1());
        //    	pComp->setBlock(dlg->editBlock->text().toInt());
        //    	string name = dlg->editName->text().latin1();


        string uniqname = m_LCoVDoc.m_CompLib.uniqueName(name);

        // 部品インデックスクラスの生成
        SCompIndex* pIndex = new SCompIndex;
        pIndex->setName(uniqname.c_str());
        pIndex->setCompInfo(pComp);

        // ドキュメントクラスに、部品情報と、インデックスを追加
        m_LCoVDoc.m_CompLib.m_listCompIndex.push_back(pIndex);
        m_LCoVDoc.m_CompLib.m_listCompInfo.push_back(pComp);

        // 部品編集ウィンドウで編集中だったら…
        if(m_componentView->m_pCompIndex != NULL) {
            m_componentView->releaseTemp();    // 一時データの解放
            if(m_componentView->m_doc.m_dirtyFlag) {     // 改変があったら
                m_componentView->saveCompInfo();    // 部品情報を保管
                m_componentView->m_doc.m_dirtyFlag = false;
                m_LCoVDoc.m_bDirty = true;
                this->setWindowModified(true);
            }
        }

        /*
        int nSelected = m_componentListBox->currentRow();

        SLCoListBoxItem* listItem = new SLCoListBoxItem;
        listItem->m_pCompIndex = pIndex;
        listItem->setText(pIndex->name());
        m_componentListBox->insertItem(nSelected,listItem);
        m_componentListBox->setCurrentRow((nSelected < 0 ? 0 : nSelected));
        */
        _componentListModel->addComponent(pIndex);
        _componentListModel->selectComponent(pIndex,m_componentListBox->selectionModel());
        m_componentView->setCompIndex(pIndex);
        m_componentView->setDirty();
        toolPin();
        m_LCoVDoc.m_bDirty = true;
        this->setWindowModified(true);
    }

    delete dlg;
}

//EDIT-ComponentProperty  ハンドラ
void ApplicationWindow::editCompProperty()
{
    //int nSelected = m_componentListBox->currentRow();
    //if(nSelected < 0) return;

    QList < SCompIndex *> selectedComp = _componentListModel->selectedComponentList(m_componentListBox->selectionModel());
    if(selectedComp.size() == 0) return;

    if(m_componentView->m_pCompIndex != NULL) {
        m_componentView->releaseTemp();
        if(m_componentView->m_doc.m_dirtyFlag) {
            m_componentView->saveCompInfo();
            m_componentView->m_doc.m_dirtyFlag = false;
            m_LCoVDoc.m_bDirty = true;
            this->setWindowModified(true);
        }
    }
    SCompIndex* pIndex = selectedComp[0];

    if(!pIndex)return;
    string oldName = pIndex->name();
    SCompInfo* pComp = pIndex->compInfo();
    if(!pComp)return;

    prop_componentdlg* dlg = new prop_componentdlg(this);
    dlg->setName(QString::fromLatin1(oldName.c_str()));
    dlg->setRef(QString::fromLatin1(pComp->m_pszRef));
    dlg->setWidth(pComp->m_size.w());
    dlg->setHeight(pComp->m_size.h());
    dlg->setBlock(pComp->m_nBlock);
    dlg->setUsePattern(pComp->m_pPtn != NULL);

    if(pComp->m_pPtn != NULL) {
        dlg->setUseBitPattern(pComp->m_pPtn->m_pBuff != NULL);
    } else {
        dlg->setUseBitPattern(false);
    }

    //    prop_comp* dlg = new prop_comp(this,0,true);
    //    dlg->editName->setText(QString::fromLatin1(oldName.c_str()));
    //    dlg->editRef->setText(QString::fromLatin1(pComp->m_pszRef));
    //    dlg->editWidth->setText(QString::number(pComp->m_size.w()));
    //    dlg->editHeight->setText(QString::number(pComp->m_size.h()));
    //    dlg->editBlock->setText(QString::number(pComp->m_nBlock));
    //    dlg->checkUsePtn->setChecked(pComp->m_pPtn != NULL);

    if(dlg->exec()) {
        SSize sizeComp = SSize(dlg->width(),dlg->height());
        bool usePtn = dlg->usePattern();
        bool noBitPtn = !(dlg->useBitPattern());
        pComp->setSize(sizeComp,usePtn,noBitPtn);
        pComp->setUsePattern(usePtn,noBitPtn);
        pComp->setRef(dlg->ref().toLatin1());
        pComp->setBlock(dlg->block());
        string name = dlg->name().toStdString();

        //    	SSize sizeComp = SSize(    dlg->editWidth->text().toInt(),
        //    	    	    	         dlg->editHeight->text().toInt());
        //    	bool usePtn = dlg->checkUsePtn->isChecked();
        //    	pComp->setSize(sizeComp,usePtn);
        //    	pComp->setUsePattern(usePtn);
        //    	pComp->setRef(dlg->editRef->text().latin1());
        //    	pComp->setBlock(dlg->editBlock->text().toInt());
        //
        //    	string name = dlg->editName->text().latin1();

        if(name != oldName) {
            string uniqname = m_LCoVDoc.m_CompLib.uniqueName(name);
            pIndex->setName(uniqname.c_str());
            m_componentListBox->repaint();
        }
        m_componentView->setCompIndex(pIndex);
        m_componentView->setDirty();
        m_LCoVDoc.m_bDirty = true;
        this->setWindowModified(true);
    }

    delete dlg;
}

//EDIT-AddAlias  ハンドラ
void ApplicationWindow::editAddAlias()
{
    //int nSelected = m_componentListBox->currentRow();
    //if(nSelected < 0) return;

    QList < SCompIndex *> selectedComp = _componentListModel->selectedComponentList(m_componentListBox->selectionModel());
    if(selectedComp.size() == 0) return;

    if(m_componentView->m_pCompIndex != NULL) {
        m_componentView->releaseTemp();
        if(m_componentView->m_doc.m_dirtyFlag) {
            m_componentView->saveCompInfo();
            m_componentView->m_doc.m_dirtyFlag = false;
            m_LCoVDoc.m_bDirty = true;
            this->setWindowModified(true);
        }
    }

    //SLCoListBoxItem* listItem = (SLCoListBoxItem*)m_componentListBox->item(nSelected);
    SCompIndex* pIndex = selectedComp[0];


    if(!pIndex)return;
    string strName = pIndex->name();
    SCompInfo* pCompInfo = pIndex->compInfo();
    if(!pCompInfo)return;

    add_alias* dlg = new add_alias(this,0,true);
    dlg->labelAliasOf->setText(QString::fromLatin1(strName.c_str()));
    dlg->editAlias->setText("NAME");
    if(dlg->exec()) {
        string strAlias = dlg->editAlias->text().toStdString();

        string uniqname = m_LCoVDoc.m_CompLib.uniqueName(strAlias);
        SCompIndex* pIndex = new SCompIndex;
        pIndex->setName(uniqname.c_str());
        pIndex->setCompInfo(pCompInfo);

        m_LCoVDoc.m_CompLib.m_listCompIndex.push_back(pIndex);

#if 0
        SLCoListBoxItem* listItem = new SLCoListBoxItem;
        listItem->setText(pIndex->name());
        listItem->m_pCompIndex = pIndex;
        m_componentListBox->insertItem(nSelected,listItem);
        m_componentListBox->setCurrentRow(nSelected);
#endif
        _componentListModel->insertComponent(selectedComp[0],pIndex);
        _componentListModel->selectComponent(pIndex,m_componentListBox->selectionModel());
        m_componentView->setCompIndex(pIndex);
        m_componentView->setDirty();
        m_LCoVDoc.m_bDirty = true;
        this->setWindowModified(true);
    }
    delete dlg;
}

void  ApplicationWindow::toolSelect()
{
    m_componentView->setTool(SComponentView::TOOL_SELECT);
    setToolButtonState();
}

void  ApplicationWindow::toolPin()
{
    m_componentView->setTool(SComponentView::TOOL_PIN);
    setToolButtonState();
}

void  ApplicationWindow::toolLine()
{
    m_componentView->setTool(SComponentView::TOOL_LINE);
    setToolButtonState();
}

void  ApplicationWindow::toolEllipse()
{
    m_componentView->setTool(SComponentView::TOOL_CIRCLE);
    setToolButtonState();
}

void  ApplicationWindow::toolPolygon()
{
    m_componentView->setTool(SComponentView::TOOL_POLYGON);
    setToolButtonState();
}

void  ApplicationWindow::toolArc()
{
    m_componentView->setTool(SComponentView::TOOL_ARC);
    setToolButtonState();
}

void  ApplicationWindow::toolText()
{
    m_componentView->setTool(SComponentView::TOOL_TEXT);
    setToolButtonState();
}

void  ApplicationWindow::toolHandle()
{
    bool currentMode= m_componentView->editNodeMode();
    m_componentView->setEditNodeMode(!currentMode);
    setToolButtonState();
}

//ボタンの状態の設定
void ApplicationWindow::setToolButtonState()
{
    int toolmode = m_componentView->tool();
    //メニューのチェック状態
#if 1
    m_menuToolSelect->setChecked(toolmode == SComponentView::TOOL_SELECT);
    m_menuToolPin->setChecked(toolmode == SComponentView::TOOL_PIN);
    m_menuToolLine->setChecked(toolmode == SComponentView::TOOL_LINE);
    m_menuToolEllipse->setChecked(toolmode == SComponentView::TOOL_CIRCLE);
    m_menuToolPolygon->setChecked(toolmode == SComponentView::TOOL_POLYGON);
    m_menuToolText->setChecked(toolmode == SComponentView::TOOL_TEXT);
    m_menuToolArc->setChecked(toolmode == SComponentView::TOOL_ARC);
#endif

    m_menuToolMoveNode->setChecked(m_componentView->editNodeMode());
}


/*
void ApplicationWindow::print()
{
    printer->setFullPage( TRUE );
    if ( printer->setup(this) ) {    	// printer dialog
 statusBar()->message( "Printing..." );
 QPainter p;
 if( !p.begin( printer ) ) {               // paint on printer
     statusBar()->message( "Printing aborted", 2000 );
     return;
 }

 QPaintDeviceMetrics metrics( p.device() );
 int dpiy = metrics.logicalDpiY();
 int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
 QRect view( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
 QSimpleRichText richText( QStyleSheet::convertFromPlainText(e->text()),
      QFont(),
      e->context(),
      e->styleSheet(),
      e->mimeSourceFactory(),
      view.height() );
 richText.setWidth( &p, view.width() );
 int page = 1;
 do {
     richText.draw( &p, margin, margin, view, colorGroup() );
     view.moveBy( 0, view.height() );
     p.translate( 0 , -view.height() );
     p.drawText( view.right() - p.fontMetrics().width( QString::number( page ) ),
   view.bottom() + p.fontMetrics().ascent() + 5, QString::number( page ) );
     if ( view.top() - margin >= richText.height() )
  break;
     printer->newPage();
     page++;
 } while (TRUE);

 statusBar()->message( "Printing completed", 2000 );
    } else {
 statusBar()->message( "Printing aborted", 2000 );
    }
}
*/
void ApplicationWindow::closeEvent( QCloseEvent* ce )
{

    if(m_componentView->m_pCompIndex != NULL) {
        m_componentView->releaseTemp();
        if(m_componentView->m_doc.m_dirtyFlag) {
            m_componentView->saveCompInfo();
            m_componentView->m_doc.m_dirtyFlag = false;
            m_LCoVDoc.m_bDirty = true;
            this->setWindowModified(true);
        }
    }


    if(!m_LCoVDoc.m_bDirty) {
        m_componentView->setCompIndex(NULL);
        ce->accept();
        return;
    }

    QMessageBox::StandardButton ret =
        SheetMessageBox::information(this,"Qt-LCov Modified",
                                     "Do you want to save the changes to the document?",
                                     QMessageBox::Discard | QMessageBox::Save | QMessageBox::Cancel, QMessageBox::Save);

    switch(ret) {
    case QMessageBox::Save:
        save();
        m_componentView->setCompIndex(NULL);
        ce->accept();
        break;
    case QMessageBox::Discard:
        m_componentView->setCompIndex(NULL);
        ce->accept();
        break;
    case QMessageBox::Cancel:
    default:
        ce->ignore();
        break;

    }
}


//Windowメニュー更新のタイミングを受け取る
void ApplicationWindow::setupWindowMenu()
{
    //printf("setupWindowMenu \n");
    QString fn;
    m_menuWindow->clear();
    int windowCount = m_appWindowList.count();
    int n;
    for(n = 0; n < windowCount; n++) {
        ApplicationWindow * w = (ApplicationWindow*)m_appWindowList.at(n);
        fn = w->fileName();
        if(fn.isEmpty()) fn="Untitled";
        QAction* action = m_menuWindow->addAction(fn);
        action->setData(QVariant(n));
        action->setCheckable(true);
        if(w == this) {
            action->setChecked(true);
        }
        //m_menuWindow->insertItem(fn,n);
        //if(w == this){
        //    m_menuWindow->setItemChecked(n,true);
        //}
    }
}

//Windowメニューのアイテムが選択されたときのSLOT
void ApplicationWindow::selectActiveDocument(QAction* action)
{
    int id = action->data().toInt();
    int windowCount = m_appWindowList.count();
    if(id < 0 || windowCount <= id)return;
    ApplicationWindow * w = (ApplicationWindow*)m_appWindowList.at(id);
    w->activateWindow();
    w->raise();    	//2005/06/11
}

//void ApplicationWindow::selectActiveDocument(int id)
//{
//    int windowCount = m_appWindowList.count();
//    if(id < 0 || windowCount <= id)return;
//    ApplicationWindow * w = (ApplicationWindow*)m_appWindowList.at(id);
//    w->setActiveWindow();
//    w->raise();
//}



void ApplicationWindow::about()
{
    QPixmap icon( ":/images/lcoicon.png" );

    QMessageBox mb(this);

    mb.setMinimumWidth(400);
    mb.setWindowTitle("About  Qt-LCoV");
    mb.setTextFormat(Qt::RichText);
    mb.setText(
        "<center><b> Qt-LCoV - Modified Version 0.43.008</b><br><br>"
        "Copyright(C) 2010-2012 Stork Lab < br>"
        "Lisenced by GPL3 or later < br>"
        "URL  <a href=\"http://storklab.cyber-ninja.jp\">http://storklab.cyber-ninja.jp</a><br>"
        "E-Mail  storklab@gmail.com</center><hr>"
        "<center><b > Qt-LCoV  Version 0.43</b><br><br>"
        "Copyright(C) 2002-2006 H.Okada(Suigyodo)<br>"
        "Qt-LCoV comes with absolutely no warranty.<br>"
        "URL  http://www.suigyodo.com/online < br>"
        "E-Mail  hitoshi@suigyodo.com</center>"
    );
    mb.setIconPixmap(icon);
    //mb.adjustSize();
    mb.exec();

}


void ApplicationWindow::aboutQt()
{
    QMessageBox::aboutQt( this, "About Qt" );
}

//ComponentCopyの実際
void ApplicationWindow::doComponentCopy()
{
    //    編集中の部品データの保存
    //    if(m_wndView.m_pCompIndex != NULL){
    //    	m_wndView.releaseTemp();
    //    	if(m_wndView.m_doc.m_dirtyFlag){
    //    	    m_wndView.saveCompInfo();
    //    	}
    //    }

    // 部品編集ウィンドウで編集中だったら…
    if(m_componentView->m_pCompIndex != NULL) {
        m_componentView->releaseTemp();    // 一時データの解放
        if(m_componentView->m_doc.m_dirtyFlag) {     // 改変があったら
            m_componentView->saveCompInfo();    // 部品情報を保管
            m_componentView->m_doc.m_dirtyFlag = false;
            m_LCoVDoc.m_bDirty = true;
            this->setWindowModified(true);
        }
    }

    QList < SCompIndex *> selectedComp = _componentListModel->selectedComponentList(m_componentListBox->selectionModel());
    if(selectedComp.size() == 0) return;

    SCompIndex* pIndex = selectedComp[0];

    if(!pIndex)return;
    string name = pIndex->name();
    if(name.empty())return;
    SCompInfo* pInfo = pIndex->compInfo();
    if(!pInfo)return;
    string str;
    SWriteCE3 wce3;
    wce3.Attach(&str);

    wce3.WriteRecord("+BSCH3_DATA_V.1.0");
    wce3.WriteEOL();
    wce3.WriteRecord("+COMPONENT");
    wce3.WriteEOL();
    wce3.WriteRecord("+BSCH3_LIB_V.1.0");
    wce3.WriteEOL();
    pInfo->writeCe3(wce3,name.c_str());
    wce3.WriteRecord("-BSCH3_LIB_V.1.0");
    wce3.WriteEOL();
    wce3.WriteRecordInt("X",pInfo->m_size.w()*10);
    wce3.WriteRecordInt("Y",pInfo->m_size.h()*10);
    wce3.WriteRecordString("LIB",name);
    wce3.WriteRecordString("N",  name);
    wce3.WriteRecordString("R",  pInfo->m_pszRef);
    wce3.WriteEOL();
    wce3.WriteRecord("-COMPONENT");
    wce3.WriteEOL();
    wce3.WriteRecord("-BSCH3_DATA_V.1.0");
    wce3.WriteEOL();

    QClipboard *cb = QApplication::clipboard();
    cb->setText(QString::fromLocal8Bit(str.c_str()));
}

//ComponentDeleteの削除
void ApplicationWindow::doComponentDelete()
{
    QList < SCompIndex *> selectedComp = _componentListModel->selectedComponentList(m_componentListBox->selectionModel());
    if(selectedComp.size() == 0) return;

    SCompIndex* pIndex = selectedComp[0];
    if(!pIndex)return;


    //    SLBCompName* pLB =(SLBCompName*)m_wndDlgBar.GetDlgItem(IDC_LIST_NAME);
    //    int nIndex = pLB->GetCurSel();
    //    if(nIndex < 0)return;


    //    編集中の部品データの破棄
    //    m_wndView.setCompIndex(NULL);

    //リストボックスからの削除
    //m_componentListBox->takeItem(nSelected);
    _componentListModel->removeComponentAll(pIndex);


    //m_pCompInfoを共有していないかどうかのチェック
    bool shareConpInfo =false;

    SCompIndexListIterator iteIndex =m_LCoVDoc.m_CompLib.m_listCompIndex.begin();
    while(iteIndex != m_LCoVDoc.m_CompLib.m_listCompIndex.end()) {
        if((*iteIndex) == pIndex) {
            SCompIndexListIterator iteIndex2 =m_LCoVDoc.m_CompLib.m_listCompIndex.begin();
            while(iteIndex2 != m_LCoVDoc.m_CompLib.m_listCompIndex.end()) {
                if(iteIndex != iteIndex2) {
                    if((*iteIndex)->m_pCompInfo ==(*iteIndex2)->m_pCompInfo) {
                        shareConpInfo = true;
                        break;
                    }
                }
                iteIndex2++;
            }
            if(!shareConpInfo) { ///m_pCompInfoを共有していない
                SCompInfoListIterator iteInfo = m_LCoVDoc.m_CompLib.m_listCompInfo.begin();
                while(iteInfo != m_LCoVDoc.m_CompLib.m_listCompInfo.end()) {
                    if((*iteIndex)->m_pCompInfo == (*iteInfo)) {
                        delete (*iteInfo);
                        m_LCoVDoc.m_CompLib.m_listCompInfo.erase(iteInfo);
                        break;
                    }
                    iteInfo++;
                }
            }
            delete (*iteIndex);
            m_LCoVDoc.m_CompLib.m_listCompIndex.erase(iteIndex);
            break;
        }
        iteIndex++;
    }

    if(_componentListModel->componentList().size()>0) {
        //nSelected = m_componentListBox->currentRow();
        listboxSelectionChanged();
        //部品編集ウィンドウに新しい選択を反映
        //itemSelected = (SLCoListBoxItem*)m_componentListBox->item(nSelected);
        //CompEdit->setCompIndex(itemSelected->m_pCompIndex);
    } else {
        m_componentView->setCompIndex(NULL);
    }
    m_LCoVDoc.m_bDirty =true;
    this->setWindowModified(true);


}

void ApplicationWindow::redrawAll()
{
    int windowCount = m_appWindowList.count();
    int n;
    for(n = 0; n < windowCount; n++) {
        ApplicationWindow * w = (ApplicationWindow*)m_appWindowList.at(n);
        w->m_componentView->RefreshView();
        //w->m_pXBSchView->erase();
        //qDebug("redrawAll()\n");
    }
}

//View Menu ハンドラ
void ApplicationWindow::viewZoomIn()
{
    m_componentView->OnViewZoomin();
}
void ApplicationWindow::viewZoomOut()
{
    m_componentView->OnViewZoomout();
}


void ApplicationWindow::setFont()
{
    QFont font;
    int dpi,point,pixel;
    QDesktopWidget *desktop = QApplication::desktop();
    dpi = desktop->logicalDpiX();

    string strname;
    if(g_cfg.getString("SmallFont","Name",strname)) {
        font = QFont(QString::fromLocal8Bit(strname.c_str()));
        int n;
        if(g_cfg.getInt("SmallFont","Size",pixel)) {
            point=(pixel*72+dpi-1)/dpi;
            font.setPointSize(point);
        }
        if(g_cfg.getInt("SmallFont","Size",n)) font.setPixelSize(n);
        if(g_cfg.getInt("SmallFont","Bold",n)) font.setBold(n != 0);
        if(g_cfg.getInt("SmallFont","Italic",n)) font.setItalic(n != 0);
        if(g_cfg.getInt("SmallFont","Underline",n)) font.setStrikeOut(n != 0);
        if(g_cfg.getInt("SmallFont","StrikeOut",n)) font.setUnderline(n != 0);
    }

    bool ok;
    font = QFontDialog::getFont(&ok,font,this);
    if(ok) {
        g_cfg.setString("SmallFont","Name",string(font.family().toLocal8Bit()));
        point = font.pointSize();
        pixel = point*dpi/72;
        //QFontInfo fontInfo(font);
        font.setPixelSize(pixel);
        g_cfg.setInt("SmallFont","Size",pixel);
        g_cfg.setInt("SmallFont","Bold",font.bold()?1:0);
        g_cfg.setInt("SmallFont","Italic",font.italic()?1:0);
        g_cfg.setInt("SmallFont","Underline",font.underline()?1:0);
        g_cfg.setInt("SmallFont","StrikeOut",font.strikeOut()?1:0);
        g_SmallFont.SetFont(font);
        redrawAll();
        //m_pXBSchView->setSmallFont();
    }
}


//ComponentPasteの実際
void  ApplicationWindow::doComponentPaste()
{
    QClipboard *cb = QApplication::clipboard();
    string cbbuff = string(cb->text().toLocal8Bit());
    SReadCE3 rce3;
    rce3.Attach(cbbuff.c_str());

    string str("");

    rce3.ReadRecord(str);
    if(str != "+BSCH3_DATA_V.1.0") return;
    if(rce3.SkipTo("+BSCH3_LIB_V.1.0") == EOF) return;

    if(!m_LCoVDoc.m_CompLib.ReadLb3(rce3,true))return;

    SCompIndex* pIndex = m_LCoVDoc.m_CompLib.m_listCompIndex.back();
    if(pIndex) {
        // 部品編集ウィンドウで編集中だったら…
        if(m_componentView->m_pCompIndex != NULL) {
            m_componentView->releaseTemp();    // 一時データの解放
            if(m_componentView->m_doc.m_dirtyFlag) {     // 改変があったら
                m_componentView->saveCompInfo();    // 部品情報を保管
                m_componentView->m_doc.m_dirtyFlag = false;
                m_LCoVDoc.m_bDirty = true;
                this->setWindowModified(true);
            }
        }

        QList < SCompIndex *> selectedComp = _componentListModel->selectedComponentList(m_componentListBox->selectionModel());
        if(selectedComp.size()) {
            _componentListModel->insertComponent(selectedComp[0],pIndex);
        } else {
            _componentListModel->addComponent(pIndex);
        }
        _componentListModel->selectComponent(pIndex,m_componentListBox->selectionModel());

        m_componentView->setCompIndex(pIndex);

        m_LCoVDoc.m_bDirty = true;
        this->setWindowModified(true);
    }
}

void ApplicationWindow::toolCreateBitPattern()
{
    if(!m_componentView->canCreateBitPtn()) {
        SheetMessageBox::information( this, "Create Bit Pattern",
                                      "No Bit Pattern",
                                      QMessageBox::Ok);
        return;
    }
    if(SheetMessageBox::warning( this, "Create Bit Pattern",
                                 "Overwrite existing bit pattern?",
                                 QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok) == QMessageBox::Cancel) return;
    m_componentView->doCreateBitPtn();

}
