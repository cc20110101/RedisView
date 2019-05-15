/**
* @file      KeyTreeModel.cpp
* @brief     树模型
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "Model/KeyTreeModel.h"

KeyTreeModel::KeyTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    headerInfo = "KEY";
    conicon = new QIcon(ICON_LONGIN);
    dbicon  = new QIcon(ICON_DB);
    keyicon  = new QIcon(ICON_KEY);
    rootItem = new KeyTreeItem(headerInfo);
}

KeyTreeModel::~KeyTreeModel()
{
    if(rootItem) {
        delete rootItem;
        rootItem = nullptr;
    }
}

void KeyTreeModel::setRootItem(KeyTreeItem * item) {
    beginResetModel();
    if(rootItem)
        delete rootItem;
    rootItem = item;
    endResetModel();
}

void KeyTreeModel::clear() {
    beginResetModel();
    rootItem->clear();
    endResetModel();
}

int KeyTreeModel::columnCount(const QModelIndex &) const
{
    return rootItem->columnCount();
}

QVariant KeyTreeModel::data(KeyTreeItem * item) const
{
    if (!item)
        return QVariant();

    return item->data(0);
}

QVariant KeyTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    KeyTreeItem *item = getItem(index);
    switch(role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return item->data(index.column());
    case Qt::DecorationRole :
        if(item->haveIcon()) {
            if(item->getIconId() == 1)
                return *conicon;
            else if(item->getIconId() == 2) {
                return *dbicon;
            }
        } else {
            return *keyicon;
        }
    }
    return QVariant();
}

void KeyTreeModel::sort(int column, Qt::SortOrder order) {
    Q_UNUSED(column)
    recurSortItem(rootItem, order);
}

void KeyTreeModel::recurSortItem(KeyTreeItem *item, Qt::SortOrder order) {
    if(item == nullptr)
        return;

    if(item->childCount() == 0)
        return;

    for(int i = 0; i < item->childCount(); ++i) {
        sortItem(item->child(i),order);
        recurSortItem(item->child(i));
    }
}

void KeyTreeModel::sortItem(KeyTreeItem *item, Qt::SortOrder order) {

    if(item == nullptr)
        return;

    if(item->childCount() == 0)
        return;

    QModelIndex index = indexFromItem(item);
    if(!index.isValid())
        return;

    QList<QPersistentModelIndex> indexs;
    indexs << index;
    emit layoutAboutToBeChanged(indexs, QAbstractItemModel::VerticalSortHint);
    item->sort(order);
    emit layoutChanged(indexs, QAbstractItemModel::VerticalSortHint);
}

Qt::ItemFlags KeyTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

KeyTreeItem *KeyTreeModel::itemFromIndex(const QModelIndex &index) const {
    return index.isValid() ? getItem(index): nullptr;
}

QModelIndex KeyTreeModel::indexFromItem(const KeyTreeItem *item) const {
    if(!item || item == rootItem)
        return QModelIndex();

    return createIndex(item->childNumber(), 0, const_cast<KeyTreeItem *>(item));
}

KeyTreeItem *KeyTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        KeyTreeItem *item = static_cast<KeyTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

KeyTreeItem *KeyTreeModel::getRootItem() const
{
    return rootItem;
}

QVariant KeyTreeModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
    Q_UNUSED(section)
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return headerInfo;

    return QVariant();
}

QModelIndex KeyTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    KeyTreeItem *parentItem = getItem(parent);
    KeyTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool KeyTreeModel::removeChild(KeyTreeItem * item, int row) {
    if(item) {
        QModelIndex itemQModel = indexFromItem(item);
        return removeRows(row, 1, itemQModel);
    }
    return false;
}

bool KeyTreeModel::removeChild(KeyTreeItem * item) {
    if(item) {
        QModelIndex itemQModel = indexFromItem(item);
        return removeRows(0, item->childCount(),itemQModel);
    }
    return false;
}

bool KeyTreeModel::insertRow(KeyTreeItem *item) {
    if(item && item->parent()) {
        QModelIndex parentQModel = indexFromItem(item->parent());
        beginInsertRows(parentQModel, item->parent()->childCount(), item->parent()->childCount());
        item->parent()->appendChildren(item);
        endInsertRows();
        return true;
    }
    return false;
}

QModelIndex KeyTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    KeyTreeItem *childItem = getItem(index);
    if(childItem == nullptr)
        return QModelIndex();

    KeyTreeItem *parentItem = childItem->parent();

    if (parentItem == nullptr || parentItem == rootItem)
        return QModelIndex();

    if(parentItem->parent() == nullptr)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool KeyTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    if(rows <= 0)
        return false;

    KeyTreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int KeyTreeModel::rowCount(const QModelIndex &parent) const
{
    KeyTreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

bool KeyTreeModel::setText(KeyTreeItem * item, const QString &value) {
    return setData(indexFromItem(item),value,Qt::EditRole);
}

bool KeyTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    KeyTreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool KeyTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                                 const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    headerInfo = value.toString();
    emit headerDataChanged(orientation, section, section);
    return true;
}
