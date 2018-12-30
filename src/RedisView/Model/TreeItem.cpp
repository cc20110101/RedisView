#include "TreeItem.h"

bool compareItemAsc(TreeItem * &item1, TreeItem * &item2)
{
    if (item1->text() < item2->text())
    {
        return true;
    }
    return false;
}

bool compareItemDesc(TreeItem * &item1, TreeItem * &item2)
{
    if (item1->text() < item2->text())
    {
        return false;
    }
    return true;
}

TreeItem::TreeItem(const QString &data, TreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
    childItems.clear();
}

void TreeItem::clear() {
    qDeleteAll(childItems);
    childItems.clear();
}

void TreeItem::setIconId(const qint8 &value)
{
    iconId = value;
}

qint8 TreeItem::getIconId() const
{
    return iconId;
}

TreeItem *TreeItem::child(int number)
{
    return childItems.value(number);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int TreeItem::columnCount() const
{
    return 1;
}

QVariant TreeItem::data(int column) const
{
    if(column == 0)
        return itemData;
    else
        return QVariant();
}

QString TreeItem::text() {
    return itemData;
}

void TreeItem::appendChildren(TreeItem * item) {
    childItems.insert(childItems.size(), item);
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool TreeItem::setData(int column, const QVariant &value)
{
    if(column == 0)
        itemData = value.toString();
    else
        return false;

    return true;
}

void TreeItem::sort(Qt::SortOrder order) {
    if(order == Qt::AscendingOrder)
        qSort(childItems.begin(),childItems.end(),compareItemAsc);
    else
        qSort(childItems.begin(),childItems.end(),compareItemDesc);
}
