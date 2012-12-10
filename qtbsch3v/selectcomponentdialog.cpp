#include "selectcomponentdialog.h"
#include "ui_selectcomponentdialog.h"

#include <QAbstractListModel>
#include <QFileInfo>
#include <QDebug>

#include "sheetmessagebox.h"
#include "xbsch.h"
#include "xbschglobal.h"
#include "complib.h"

class SelectComponentDialogLibraryListModel : public QAbstractListModel
{
public:
    SelectComponentDialogLibraryListModel() {

    }

    virtual int columnCount ( const QModelIndex & /* parent */ = QModelIndex() ) const {
        return 1;
    }
    virtual int rowCount ( const QModelIndex & /*parent*/ = QModelIndex() ) const {
        return g_LibraryCount();
    }
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const {
        switch(role) {
        case Qt::DisplayRole: {
            const SCompLib* pLib = g_LibraryInfo(index.row());
            QFileInfo finfo(QString::fromUtf8(pLib->libraryName())); // convert from path name
            return finfo.baseName();
        }
        default:
            return QVariant();
        }
    }
    const SCompLib* library(const QModelIndex &index ) const {
        return g_LibraryInfo(index.row());
    }
};

class SelectComponentDialogComponentListModel : public QAbstractListModel
{
private:
    const SCompLib *m_lib;
public:
    SelectComponentDialogComponentListModel() : m_lib(0) {

    }

    void setLibrary(const SCompLib *newlib) {
        beginResetModel();
        m_lib = newlib;
        endResetModel();
    }

    virtual int columnCount ( const QModelIndex & /* parent */ = QModelIndex() ) const {
        return 1;
    }
    virtual int rowCount ( const QModelIndex & /*parent*/ = QModelIndex() ) const {
        if(m_lib)
            return m_lib->count();
        else
            return 0;
    }
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const {
        if(!m_lib)
            return QVariant();
        switch(role) {
        case Qt::DisplayRole: {
            const SCompIndex* pComp = m_lib->compIndex(index.row());
            return QString::fromLocal8Bit(pComp->name());
        }
        default:
            return QVariant();
        }
    }

    const SCompIndex* componentIndex(const QModelIndex & index) const {
        if(!m_lib) return 0;
        return  m_lib->compIndex(index.row());
    }
};

SelectComponentDialog::SelectComponentDialog(QWidget *parent) :
    QDialog(parent),m_pCompIndex(0),
    ui(new Ui::SelectComponentDialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowModality(Qt::WindowModal);

    m_libraryListModel = new SelectComponentDialogLibraryListModel;
    m_componentListModel = new SelectComponentDialogComponentListModel;

    initializeComponents();
}

SelectComponentDialog::~SelectComponentDialog()
{
    delete m_libraryListModel;
    delete m_componentListModel;
    delete ui;
}


void SelectComponentDialog::initializeComponents()
{
    ui->libraryList->setModel(m_libraryListModel);
    connect(ui->libraryList->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(librarySelectChanged(QModelIndex,QModelIndex)));
    ui->componentList->setModel(m_componentListModel);
    connect(ui->componentList->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(componentSelectChanged(QModelIndex,QModelIndex)));

    QList < int> sizelist;
    sizelist << 400 << 300;
    ui->splitter_2->setSizes(sizelist);
}

void SelectComponentDialog::accept()
{
    if(m_pCompIndex)
        QDialog::accept();
    else
        SheetMessageBox::warning(NULL,tr("No component is selected"),tr("Please select a componenet."));
}

void SelectComponentDialog::librarySelectChanged(const QModelIndex &current,const QModelIndex &previous)
{
    if(current == previous)
        return;
    QModelIndexList modellist;
    modellist << current;
    ui->libraryList->selectionModel()->select(current,QItemSelectionModel::ClearAndSelect);

    m_currentLib = m_libraryListModel->library(current);
    m_componentListModel->setLibrary(m_currentLib);
    if(m_currentLib->count()) {
        componentSelectChanged(m_componentListModel->index(0),QModelIndex());
    }
}

void SelectComponentDialog::componentSelectChanged(const QModelIndex &current,const QModelIndex & previous)
{
    if(current == previous)
        return;
    ui->componentList->selectionModel()->select(current,QItemSelectionModel::ClearAndSelect);
    m_pCompIndex = m_componentListModel->componentIndex(current);
    ui->component->setComponentIndex(m_pCompIndex);
    ui->componentName->setText(QString::fromLocal8Bit(m_pCompIndex->name()));
}

void SelectComponentDialog::on_componentName_textChanged(const QString &arg1)
{
    if(m_pCompIndex && QString::fromLocal8Bit(m_pCompIndex->name()) == arg1)
        return;

    for(int i = 0; i < m_currentLib->count(); i++) {
        const SCompIndex *index = m_currentLib->compIndex(i);
        if(index && QString::fromLocal8Bit(index->name()) == arg1) {
            componentSelectChanged(m_componentListModel->index(i),QModelIndex());
            break;
        }
    }
    ui->component->setComponentIndex(NULL);
    m_pCompIndex = NULL;
}
