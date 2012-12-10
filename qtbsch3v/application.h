#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include <QList>

class QLabel;

namespace Ui
{
class ApplicationWindow;
}

class ApplicationWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ApplicationWindow(QWidget *parent = 0);
    virtual ~ApplicationWindow();

    virtual bool eventFilter(QObject *, QEvent *);
    virtual void closeEvent( QCloseEvent* ce );

    QString fileName() {
        return m_filename;
    }
    bool isEmptyDocWindow();
    static ApplicationWindow* firstEmptyDocWindow();

private:
    static QList<ApplicationWindow *> applicationWindows;

    QString m_filename;

    QLabel *m_plabelX;
    QLabel *m_plabelY;

    void SetToolButtonState();     //ツールボタンの状態を設定する
    void setAppCaption();       //アプリケーションのキャプションの設定

public slots:
    void openFile(QString file);

private slots:
    void toolSelectionChanged();
    void cursorMove(int x,int y);

    bool saveFile();
    bool saveFileAs();

    void updateWindowMenu();
    void updateEditMenu();
    void selectActiveWindow(QAction*);
    void selectedLayerChanged();
    void enableLayerChanged();
    void moveToLayer();


private slots:
    void on_actionVisit_StorkLab_webpage_triggered();

    void on_actionAbout_Qt_triggered();

    void on_actionAbout_QtBSch3V_Modified_triggered();

    void on_action_Open_triggered();

    void on_action_New_triggered();

    void on_action_Print_triggered();

    void on_actionP_rint_Option_triggered();

    void on_actionLibrary_triggered();

    void on_action_Export_image_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_action_Grid_triggered();

    void on_actionS_nap_triggered();

    void on_actionL_abel_Font_triggered();

    void on_action_Default_comment_font_triggered();

    void on_actionDarken_non_Active_Layer_triggered(bool);

protected:
    void keyPressEvent ( QKeyEvent * event );

private:
    Ui::ApplicationWindow *ui;
};

#endif // APPLICATION_H
