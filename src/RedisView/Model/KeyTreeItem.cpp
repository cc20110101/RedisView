/**
* @file      KeyTreeItem.cpp
* @brief     树项
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "Model/KeyTreeItem.h"

bool compareTreeItemAsc(KeyTreeItem * &item1, KeyTreeItem * &item2)
{
    if (item1->text() < item2->text())
    {
        return true;
    }
    return false;
}

bool compareTreeItemDesc(KeyTreeItem * &item1, KeyTreeItem * &item2)
{
    if (item1->text() < item2->text())
    {
        return false;
    }
    return true;
}

KeyTreeItem::KeyTreeItem(const QString &data, KeyTreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

KeyTreeItem::~KeyTreeItem()
{
    qDeleteAll(childItems);
    childItems.clear();
}

void KeyTreeItem::clear() {
    qDeleteAll(childItems);
    childItems.clear();
}

bool KeyTreeItem::haveIcon() {
    return false;
}

void KeyTreeItem::setIconId(const qint8 &)
{
}

qint8 KeyTreeItem::getIconId() const
{
    return 0;
}

KeyTreeItem *KeyTreeItem::child(int number)
{
    return childItems.value(number);
}

int KeyTreeItem::childCount() const
{
    return childItems.count();
}

int KeyTreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<KeyTreeItem*>(this));

    return 0;
}

int KeyTreeItem::columnCount() const
{
    return 1;
}

QVariant KeyTreeItem::data(int column) const
{
    if(column == 0)
        return itemData;
    else
        return QVariant();
}

QString KeyTreeItem::text() {
    return itemData;
}

void KeyTreeItem::appendChildren(KeyTreeItem * item) {
    childItems.insert(childItems.size(), item);
}

KeyTreeItem *KeyTreeItem::parent()
{
    return parentItem;
}

bool KeyTreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool KeyTreeItem::setData(int column, const QVariant &value)
{
    if(column == 0)
        itemData = value.toString();
    else
        return false;

    return true;
}

void KeyTreeItem::sort(Qt::SortOrder order) {
    if(order == Qt::AscendingOrder)
        std::sort(childItems.begin(),childItems.end(),compareTreeItemAsc);
    else
        std::sort(childItems.begin(),childItems.end(),compareTreeItemDesc);
}

void KeyDbTreeItem::setIconId(const qint8 &value)
{
    iconId = value;
}

qint8 KeyDbTreeItem::getIconId() const
{
    return iconId;
}

bool KeyDbTreeItem::haveIcon() {
    return true;
}
