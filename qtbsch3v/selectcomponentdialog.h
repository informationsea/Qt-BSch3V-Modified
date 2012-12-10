#ifndef SELECTCOMPONENTDIALOG_H
#define SELECTCOMPONENTDIALOG_H

#include <QDialog>
#include <QModelIndex>

#include "complib.h"

namespace Ui
{
class SelectComponentDialog;
}

class SelectComponentDialogLibraryListModel;
class SelectComponentDialogComponentListModel;

class SelectComponentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectComponentDialog(QWidget *parent = 0);
    virtual ~SelectComponentDialog();

    const SCompIndex* selectedCompIndex() const {
        return m_pCompIndex;
    }

public slots:
    virtual void accept();

private:
    SelectComponentDialogLibraryListModel *m_libraryListModel;
    SelectComponentDialogComponentListModel *m_componentListModel;

    const SCompIndex* m_pCompIndex;
    const SCompLib *m_currentLib;

    void initializeComponents();

private slots:
    void librarySelectChanged(const QModelIndex &current,const QModelIndex &previous);
    void componentSelectChanged(const QModelIndex &current,const QModelIndex &previous);

    void on_componentName_textChanged(const QString &arg1);

private:
    Ui::SelectComponentDialog *ui;
};

#endif // SELECTCOMPONENTDIALOG_H
