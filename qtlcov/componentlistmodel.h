#ifndef COMPONENTLISTMODEL_H
#define COMPONENTLISTMODEL_H

#include <QAbstractListModel>
#include <QItemSelectionModel>
#include <QList>

#include "complib.h"

class ApplicationWindow;

class ComponentListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ComponentListModel(QObject *parent = 0);
    virtual ~ComponentListModel() {}

    virtual int    columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    void setApplicationWindow(ApplicationWindow *app) {
        _app = app;
    }
    ApplicationWindow* applicationWindow() {
        return _app;
    }

    void onButtonUp(QItemSelectionModel *selectionModel);         //上移動ボタンが押されたときのハンドラ
    void onButtonDown(QItemSelectionModel *selectionModel);        //下移動ボタンが押されたときのハンドラ

    void clearComponent();
    void addComponent(SCompIndex *newcomp);
    void insertComponent(int i,SCompIndex *newcomp);
    void insertComponent(SCompIndex *before,SCompIndex *newcomp);
    void removeComponentAll(SCompIndex *delcomp);
    void removeComponentAt(int index);
    void setAlias(int index,bool aliased) {
        _aliasList[index] = aliased;
    }
    const QList<SCompIndex *> componentList() const;
    QList<SCompIndex *> selectedComponentList(QItemSelectionModel *selectionModel) const;
    void selectComponent(SCompIndex *comp,QItemSelectionModel *selectionModel) const;

private:
    QList<SCompIndex *> _componentList;
    QList<bool> _aliasList;
    ApplicationWindow *_app;

signals:

public slots:

};

#endif // COMPONENTLISTMODEL_H
