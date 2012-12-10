#include "componentlistmodel.h"

#include <QDebug>
#include <assert.h>

ComponentListModel::ComponentListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int ComponentListModel::columnCount ( const QModelIndex & parent ) const
{
    return 1;
}

QVariant ComponentListModel::data ( const QModelIndex & index, int role ) const
{
    switch(role) {
    case Qt::DisplayRole:
        return _componentList[index.row()]->name();
    case Qt::ForegroundRole:
        if(_aliasList[index.row()]) {
            return Qt::red;
        } else {
            return Qt::black;
        }
    default:
        return QVariant();
    }
}

int ComponentListModel::rowCount ( const QModelIndex & parent ) const
{
    return _componentList.size();
}


void ComponentListModel::onButtonUp(QItemSelectionModel *selectionModel) //上移動ボタンが押されたときのハンドラ
{
    int selected = 0;
    QModelIndexList selectedRows = selectionModel->selectedRows();
    foreach(QModelIndex index,selectedRows) {
        selected = index.row();
        break;
    }
    if(selected == 0)
        return;
    beginResetModel();
    SCompIndex *temp = _componentList[selected-1];
    _componentList[selected-1] = _componentList[selected];
    _componentList[selected] = temp;
    endResetModel();
    selectComponent(_componentList[selected-1],selectionModel);
}

void ComponentListModel::onButtonDown(QItemSelectionModel *selectionModel)    //下移動ボタンが押されたときのハンドラ
{
    int selected = -1;
    QModelIndexList selectedRows = selectionModel->selectedRows();
    foreach(QModelIndex index,selectedRows) {
        selected = index.row();
        break;
    }
    if(selected < 0 || selected >= _componentList.size()-1)
        return;
    beginResetModel();
    SCompIndex *temp = _componentList[selected+1];
    _componentList[selected+1] = _componentList[selected];
    _componentList[selected] = temp;
    endResetModel();
    selectComponent(_componentList[selected+1],selectionModel);
}

void ComponentListModel::addComponent(SCompIndex *newcomp)
{
    Q_ASSERT_X(_componentList.indexOf(newcomp) < 0,"addComponent","duplicated index");
    beginInsertRows(QModelIndex(),_componentList.size(),_componentList.size());
    //beginResetModel();
    _componentList << newcomp;
    _aliasList << false;
    //endResetModel();
    endInsertRows();
}

void ComponentListModel::insertComponent(int i,SCompIndex *newcomp)
{
    Q_ASSERT_X(_componentList.indexOf(newcomp) < 0,"insertComponent","duplicated index");
    beginInsertRows(QModelIndex(),i,i);
    _componentList.insert(i,newcomp);
    _aliasList.insert(i,false);
    endInsertRows();
}

void ComponentListModel::insertComponent(SCompIndex *before,SCompIndex *newcomp)
{
    insertComponent(_componentList.indexOf(before),newcomp);
}

void ComponentListModel::clearComponent()
{
    beginResetModel();
    _componentList.clear();
    _aliasList.clear();
    endResetModel();
}

void ComponentListModel::removeComponentAll(SCompIndex *delcomp)
{
    QList < int> remlist;
    QList < SCompIndex *>::iterator it;
    int i;
    for(it = _componentList.begin(),i = 0; it != _componentList.end(); ++it,++i) {
        if(delcomp == *it) {
            remlist << i;
        }
    }
    QList < int>::iterator it2;
    for(it2 = remlist.end()-1; it2 >= remlist.begin(); --it2) {
        removeComponentAt(*it2);
    }
}


void ComponentListModel::removeComponentAt(int index)
{
    beginRemoveRows(QModelIndex(),index,index);
    _componentList.removeAt(index);
    _aliasList.removeAt(index);
    endRemoveRows();
}

const QList < SCompIndex *> ComponentListModel::componentList() const
{
    return _componentList;
}

QList < SCompIndex *> ComponentListModel::selectedComponentList(QItemSelectionModel *selectionModel) const
{
    QList < SCompIndex *> retvalues;
    QModelIndexList selectedRows = selectionModel->selectedRows();
    foreach(QModelIndex index,selectedRows) {
        retvalues << _componentList[index.row()];
    }
    return retvalues;
}

void ComponentListModel::selectComponent(SCompIndex *comp,QItemSelectionModel *selectionModel) const
{
    selectionModel->select(index(_componentList.indexOf(comp)),QItemSelectionModel::ClearAndSelect);
}
