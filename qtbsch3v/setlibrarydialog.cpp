#include "setlibrarydialog.h"
#include "ui_setlibrarydialog.h"

#include <QFileDialog>
#include <QDir>

SetLibraryDialog::SetLibraryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetLibraryDialog)
{
    ui->setupUi(this);
    ui->libraryList->setModel(&_model);
}


void SetLibraryDialog::setFiles(const QStringList& files)
{
    _model.setStringList(files);
}

QStringList SetLibraryDialog::selectedFiles()
{
    return _model.stringList();
}

SetLibraryDialog::~SetLibraryDialog()
{
    delete ui;
}

void SetLibraryDialog::on_addButton_clicked()
{
    QStringList list = QFileDialog::getOpenFileNames(this,"Select library files",QDir::homePath(),"Library (*.lb3)");
    QStringList current = selectedFiles();
    current << list;
    setFiles(current);
}

void SetLibraryDialog::on_removeButton_clicked()
{
    QModelIndexList selectedList = ui->libraryList->selectionModel()->selectedIndexes();
    if(selectedList.size() == 0)
        return;
    Q_ASSERT_X(selectedList.size() == 1,"library dialog","more than one selection");
    _model.removeRow(selectedList[0].row(),selectedList[0].parent());
}

void SetLibraryDialog::on_upButton_clicked()
{
    QModelIndexList selectedList = ui->libraryList->selectionModel()->selectedIndexes();
    if(selectedList.size() == 0)
        return;
    Q_ASSERT_X(selectedList.size() == 1,"library dialog","more than one selection");
    if(selectedList[0].row() == 0)
        return;
    QStringList current = selectedFiles();
    QString line = current[selectedList[0].row()];
    current.removeAt(selectedList[0].row());
    current.insert(selectedList[0].row()-1,line);
    setFiles(current);
    ui->libraryList->selectionModel()->select(_model.index(selectedList[0].row()-1),QItemSelectionModel::ClearAndSelect);
}

void SetLibraryDialog::on_downButton_clicked()
{
    QModelIndexList selectedList = ui->libraryList->selectionModel()->selectedIndexes();
    if(selectedList.size() == 0)
        return;
    Q_ASSERT_X(selectedList.size() == 1,"library dialog","more than one selection");
    if(selectedList[0].row() == _model.rowCount()-1)
        return;
    QStringList current = selectedFiles();
    QString line = current[selectedList[0].row()];
    current.removeAt(selectedList[0].row());
    current.insert(selectedList[0].row()+1,line);
    setFiles(current);
    ui->libraryList->selectionModel()->select(_model.index(selectedList[0].row()+1),QItemSelectionModel::ClearAndSelect);
}
