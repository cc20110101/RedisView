#include "TreeModel.h"

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    headerInfo = "KEY";
    conicon = new QIcon(ICON_LONGIN);
    dbicon  = new QIcon(ICON_DB);
    keyicon  = new QIcon(ICON_KEY);
    rootItem = new TreeItem(headerInfo);
}

TreeModel::~TreeModel()
{
    delete rootItem;
    rootItem = nullptr;
}

void TreeModel::setRootItem(TreeItem * item) {
    beginResetModel();
    if(rootItem)
        delete rootItem;
    rootItem = item;
    endResetModel();
}

void TreeModel::clear() {
    beginResetModel();
    rootItem->clear();
    endResetModel();
}

int TreeModel::columnCount(const QModelIndex &) const
{
    return rootItem->columnCount();
}

QVariant TreeModel::data(TreeItem * item) const
{
    if (!item)
        return QVariant();

    return item->data(0);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = getItem(index);
    switch(role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return item->data(index.column());
    case Qt::DecorationRole :
        if(item->getIconId() == 1)
            return *conicon;
        else if(item->getIconId() == 2) {
            return *dbicon;
        } else if(item->getIconId() == 3)
            return *keyicon;
    }
    return QVariant();
}

void TreeModel::sortItem(TreeItem *item, Qt::SortOrder order) {

    if(item == nullptr)
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

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

TreeItem *TreeModel::itemFromIndex(const QModelIndex &index) const {
    return index.isValid() ? getItem(index): nullptr;
}

QModelIndex TreeModel::indexFromItem(const TreeItem *item) const {
    if(!item || item == rootItem)
        return QModelIndex();

    return createIndex(item->childNumber(), 0, const_cast<TreeItem *>(item));
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

TreeItem *TreeModel::getRootItem() const
{
    return rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    Q_UNUSED(section)
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return headerInfo;

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem = getItem(parent);
    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool TreeModel::removeChild(TreeItem * item, int row) {
    if(item) {
        QModelIndex itemQModel = indexFromItem(item);
        return removeRows(row, 1, itemQModel);
    }
    return false;
}

bool TreeModel::removeChild(TreeItem * item) {
    if(item) {
        QModelIndex itemQModel = indexFromItem(item);
        return removeRows(0, item->childCount(),itemQModel);
    }
    return false;
}

bool TreeModel::insertRow(TreeItem *item) {
    if(item && item->parent()) {
        QModelIndex parentQModel = indexFromItem(item->parent());
        beginInsertRows(parentQModel, item->parent()->childCount(), item->parent()->childCount());
        item->parent()->appendChildren(item);
        endInsertRows();
        return true;
    }
    return false;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    if(childItem == nullptr)
        return QModelIndex();

    TreeItem *parentItem = childItem->parent();

    if (parentItem == nullptr || parentItem == rootItem)
        return QModelIndex();

    if(parentItem->parent() == nullptr)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    if(rows <= 0)
        return false;

    TreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

bool TreeModel::setText(TreeItem * item, const QString &value) {
    return setData(indexFromItem(item),value,Qt::EditRole);
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    TreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    headerInfo = value.toString();
    emit headerDataChanged(orientation, section, section);
    return true;
}
