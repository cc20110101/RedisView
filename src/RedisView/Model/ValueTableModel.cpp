#include "Model/ValueTableModel.h"

ValueTableModel::ValueTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    rootItem = new ValueTableItem();
}

ValueTableModel::~ValueTableModel()
{
    if(rootItem) {
        delete rootItem;
        rootItem = nullptr;
    }
}

int ValueTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rootItem->childCount();
}

int ValueTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _column;
}

QVariant ValueTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        ValueTableItem *item =rootItem->child(index.row());
        if(!item)
            return QVariant();
        return item->data(index.column());
    } else {
        return QVariant();
    }
}

bool ValueTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole) {
        return false;
    }

    ValueTableItem *item = rootItem->child(index.row());
    if(!item)
        return false;

    if(item->setData(index.column(),value)) {
        emit dataChanged(index, index);
        emit itemChanged(item, index.column());
        return true;
    }
    return false;
}

QVariant ValueTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Vertical) {
        return section + 1;
    }

    if(section < 0 || section >= rootItem->columnCount())
        return QVariant();

    return rootItem->data(section);
}

bool ValueTableModel::setHeaderData(int section, Qt::Orientation orientation,
                                    const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    if(rootItem->setData(section,value)) {
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

void ValueTableModel::setColumnCount(int column) {
    _column = column;
    QVector<QVariant> data;
    flagsData.clear();
    for(int i = 0; i < column; ++i) {
        data.push_back(QVariant());
        flagsData.push_back(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    }
    rootItem->setData(data);
}

Qt::ItemFlags ValueTableModel::flags(int column) {
    if(column >= flagsData.size())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flagsData[column];
}

bool ValueTableModel::setFlags(int column, Qt::ItemFlags flags) {
    if(column >= flagsData.size())
        return false;
    flagsData[column] = flags;
    return true;
}

void ValueTableModel::setHeaderData(const QVector<QVariant> &data) {
    rootItem->setData(data);
}

bool ValueTableModel::insertRow(ValueTableItem *item) {
    beginInsertRows(QModelIndex(), rootItem->childCount(), rootItem->childCount());
    bool success = rootItem->insertChildren(rootItem->childCount(), item);
    endInsertRows();
    return success;
}

bool ValueTableModel::insertRow(int position, ValueTableItem * &item)
{
    beginInsertRows(QModelIndex(), position, position);
    bool success = rootItem->insertChildren(position, item);
    endInsertRows();
    return success;
}

bool ValueTableModel::removeRow(int position)
{
    beginRemoveRows(QModelIndex(), position, position);
    bool success = rootItem->removeChildren(position);
    endRemoveRows();
    return success;
}

bool ValueTableModel::removeItem(ValueTableItem *item)
{
    if(!item)
        return false;
    beginRemoveRows(QModelIndex(), rootItem->childNumber(item), rootItem->childNumber(item));
    bool success = rootItem->removeItem(item);
    endRemoveRows();
    return success;
}

int ValueTableModel::childNumber(ValueTableItem * item) const
{
    return rootItem->childNumber(item);
}

ValueTableItem * ValueTableModel::item(int row) {
    return rootItem->child(row);
}

void ValueTableModel::clear() {
    beginResetModel();
    rootItem->clear();
    _column = 0;
    flagsData.clear();
    endResetModel();
}

Qt::ItemFlags ValueTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if(index.column() >= flagsData.size())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flagsData[index.column()];
}

void ValueTableModel::sort(int column, Qt::SortOrder order) {
    QList<QPersistentModelIndex> indexs;
    indexs << QModelIndex();
    emit layoutAboutToBeChanged(indexs, QAbstractItemModel::VerticalSortHint);
    rootItem->sort(column, order);
    emit layoutChanged(indexs, QAbstractItemModel::VerticalSortHint);
}
