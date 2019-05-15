/**
* @file      ValueTableItem.h
* @brief     表项
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef VALUETABLEITEM_H
#define VALUETABLEITEM_H

#include "Public/Define.h"

class ValueTableItem
{
public:
    explicit ValueTableItem();
    explicit ValueTableItem(const QString &text);
    explicit ValueTableItem(const QVector<QVariant> &data);
    ~ValueTableItem();

    ValueTableItem *child(int number);
    int childCount() const;
    int columnCount() const;
    int childNumber(ValueTableItem *item) const;
    QVariant data(int column) const;
    bool setData(int column, const QVariant &value);
    void setData(const QVector<QVariant> &data);
    bool insertChildren(int position, ValueTableItem *&item);
    bool removeChildren(int position);
    bool removeItem(ValueTableItem *item);
    QString text(int column = 0);
    void clear();
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

private:
    QList<ValueTableItem*> childItems;
    QVector<QVariant> itemData;
};

#endif // VALUETABLEITEM_H
