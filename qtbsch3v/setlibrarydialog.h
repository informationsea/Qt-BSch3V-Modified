#ifndef SETLIBRARYDIALOG_H
#define SETLIBRARYDIALOG_H

#include <QDialog>
#include <QStringListModel>

namespace Ui
{
class SetLibraryDialog;
}

class SetLibraryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetLibraryDialog(QWidget *parent = 0);
    ~SetLibraryDialog();

    void setFiles(const QStringList& files);
    QStringList selectedFiles();

private slots:
    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_upButton_clicked();

    void on_downButton_clicked();

private:
    Ui::SetLibraryDialog *ui;

    QStringListModel _model;
};

#endif // SETLIBRARYDIALOG_H
