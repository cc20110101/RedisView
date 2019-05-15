/**
* @file      ValueTableItem.cpp
* @brief     表项
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "Model/ValueTableItem.h"

int globalColumn = 0;

bool compareTableItemAsc(ValueTableItem * &item1, ValueTableItem * &item2)
{
    if (item1->text(globalColumn) < item2->text(globalColumn))
    {
        return true;
    }
    return false;
}

bool compareTableItemDesc(ValueTableItem * &item1, ValueTableItem * &item2)
{
    if (item1->text(globalColumn) < item2->text(globalColumn))
    {
        return false;
    }
    return true;
}

ValueTableItem::ValueTableItem() {
}

ValueTableItem::ValueTableItem(const QString &text) {
    if(itemData.size() > 0)
        itemData[0] = QVariant(text);
    else
        itemData.push_back(QVariant(text));
}

ValueTableItem::ValueTableItem(const QVector<QVariant> &data)
{
    itemData = data;
}

ValueTableItem::~ValueTableItem()
{
    qDeleteAll(childItems);
    childItems.clear();
}

void ValueTableItem::setData(const QVector<QVariant> &data) {
    itemData.clear();
    itemData = data;
}

ValueTableItem *ValueTableItem::child(int number)
{
    if(number >= childItems.count())
        return nullptr;
    return childItems.value(number);
}

int ValueTableItem::childCount() const
{
    return childItems.count();
}

int ValueTableItem::columnCount() const
{
    return itemData.count();
}

QVariant ValueTableItem::data(int column) const
{
    if(column < 0 || column >= itemData.count())
        return QVariant();
    return itemData.value(column);
}

bool ValueTableItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}

bool ValueTableItem::insertChildren(int position, ValueTableItem * &item)
{
    if (position < 0 || position > childItems.size())
        return false;

    childItems.insert(position, item);
    return true;
}

bool ValueTableItem::removeChildren(int position)
{
    if (position < 0 || position >= childItems.size())
        return false;

    delete childItems.takeAt(position);

    return true;
}

bool ValueTableItem::removeItem(ValueTableItem *item) {
    if(childItems.removeOne(item)) {
        if(item) {
            delete item;
            item = nullptr;
        }
    } else
        return false;
    return true;
}

int ValueTableItem::childNumber(ValueTableItem * item) const
{
    return childItems.indexOf(item);
}

QString ValueTableItem::text(int column) {
    if (column < 0 || column >= itemData.size())
        return "";
    else
        return itemData[column].toString();
}

void ValueTableItem::clear() {
    qDeleteAll(childItems);
    childItems.clear();
}

void ValueTableItem::sort(int column, Qt::SortOrder order) {
    globalColumn = column;
    if(order == Qt::AscendingOrder)
        qSort(childItems.begin(),childItems.end(),compareTableItemAsc);
    else
        qSort(childItems.begin(),childItems.end(),compareTableItemDesc);
}
