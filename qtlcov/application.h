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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QGridLayout>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QMainWindow>
#include <QListWidget>

class ComponentListModel;
class QToolButton;
class QGridLayout;

class SComponentView;
class QAction;


#include <qwidget.h>

#include "lcovdoc.h"

class ApplicationWindow: public QMainWindow
{
    Q_OBJECT

public:
    ApplicationWindow();
    ~ApplicationWindow();

    QString fileName();
    bool isEmptyDocWindow();
    static  ApplicationWindow* firstEmptyDocWindow();
    virtual bool eventFilter(QObject *, QEvent *);

protected:
    void keyPressEvent ( QKeyEvent * e );
    void closeEvent( QCloseEvent* );
    void setAppCaption(const QString &fileName );    //アプリケーションのキャプションのセット
    void setCompIndexToListBox();        //部品インデックスをリストボックスにセット

    void doSave(); //ファイル保存の実際
    void doComponentCopy(); //ComponentCopyの実際
    void doComponentDelete(); //ComponentDeleteの削除
    void doComponentPaste(); //ComponentPasteの実際

    void redrawAll();

    void setToolButtonState();//ボタンの状態の設定

public slots:
    void load( const QString &fileName );

private slots:
    void newDoc();
    void choose();
    void save(); // FILE-SAVE ハンドラ
    void saveAs(); // FILE-SAVE AS ハンドラ
    //void print();

    void editUndo(); //EDIT-Undo  ハンドラ
    void editCut(); //EDIT-Cut  ハンドラ
    void editCopy(); //EDIT-Copy  ハンドラ
    void editPaste(); //EDIT-Paste  ハンドラ
    void editDelete(); //EDIT-Delete  ハンドラ
    void editMirror(); //EDIT- Mirror ハンドラ
    void editRotate(); //EDIT-Rotate  ハンドラ
    void editProperty(); //EDIT-Property  ハンドラ
    void editCompCut(); //EDIT-ComponentCut  ハンドラ
    void editCompCopy(); //EDIT-ComponentCopy  ハンドラ
    void editCompPaste(); //EDIT-ComponentPaste  ハンドラ
    void editCompNew(); //EDIT-ComponentNew  ハンドラ
    void editCompProperty(); //EDIT-ComponentProperty  ハンドラ
    void editAddAlias(); //EDIT-AddAlias  ハンドラ

    void toolSelect();
    void toolPin();
    void toolLine();
    void toolEllipse();
    void toolPolygon();
    void toolArc();
    void toolHandle();
    void toolText();
    void toolCreateBitPattern();

    void listboxSelectionChanged();    //部品名リストボックスの選択が変更されたときに呼ばれる
    void onButtonUp(); //上移動ボタンが押されたときのハンドラ
    void onButtonDown();    //下移動ボタンが押されたときのハンドラ

    //View Menu ハンドラ
    void viewZoomIn();
    void viewZoomOut();


    //Set Menu ハンドラ
    void setFont();

    //Window Menu ハンドラ
    void setupWindowMenu(); //Windowメニュー更新のタイミングを受け取る
    void selectActiveDocument(QAction* action); //Windowメニューのアイテムが選択されたときのSLOT


    void about();
    void aboutQt();

private:
    //QPrinter *printer;
    QString m_filename;

    QFont* fontMenuBar;

    QToolButton* m_ptbToolSelect;
    QToolButton* m_ptbToolPin;
    QToolButton* m_ptbToolLine;
    QToolButton* m_ptbToolEllipse;
    QToolButton* m_ptbToolPolygon;
    QToolButton* m_ptbToolArc;
    QToolButton* m_ptbToolHandle;
    QToolButton* m_ptbToolText;

    QWidget* m_mainWidget;

    QGridLayout* m_gridLayout;

    QToolButton* m_buttonUp;
    QToolButton* m_buttonDown;

    QMenu* m_menuWindow;    //Windowメニュー
    QListView* m_componentListBox;    //部品名リストボックス
    ComponentListModel* _componentListModel;

    SComponentView* m_componentView;

    QAction* m_menuActionToolSelect;
    QAction* m_menuActionToolPin;
    QAction* m_menuActionToolLine;
    QAction* m_menuActionToolEllipse;
    QAction* m_menuActionToolPolygon;
    QAction* m_menuActionToolArc;
    QAction* m_menuActionToolHandle;
    QAction* m_menuActionToolText;

    // -------------

    QAction* m_menuFileNew;
    QAction* m_menuFileOpen;
    QAction* m_menuFileSave;
    QAction* m_menuFileSaveAs;
    QAction* m_menuFileClose;
    QAction* m_menuFileQuit;

    QAction* m_menuEditUndo;
    QAction* m_menuEditCut;
    QAction* m_menuEditCopy;
    QAction* m_menuEditPaste;
    QAction* m_menuEditDelete;
    QAction* m_menuEditMirror;
    QAction* m_menuEditRotate;
    QAction* m_menuEditProperty;
    QAction* m_menuEditCutComponent;
    QAction* m_menuEditCopyComponent;
    QAction* m_menuEditPasteComponent;
    QAction* m_menuEditNewComponent;
    QAction* m_menuEditComponentProperty;
    QAction* m_menuEditAddAlias;

    QAction* m_menuViewZoomIn;
    QAction* m_menuViewZoomOut;

    QAction* m_menuToolSelect;
    QAction* m_menuToolPin;
    QAction* m_menuToolLine;
    QAction* m_menuToolEllipse;
    QAction* m_menuToolPolygon;
    QAction* m_menuToolArc;
    QAction* m_menuToolText;
    QAction* m_menuToolMoveNode;
    QAction* m_menuToolCreateBitPattern;

    QAction* m_menuSetFont;

    QAction* m_menuHelpAbout;
    QAction* m_menuHelpAboutQt;

    //Static メンバー変数
    static QWidgetList m_appWindowList;

    SLCoVDoc m_LCoVDoc;


};


#endif
