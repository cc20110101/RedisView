/**
* @file      KeyTreeItem.h
* @brief     树项
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef KEYTREEITEM_H
#define KEYTREEITEM_H
#include <QVariant>

class KeyTreeItem
{
public:
    explicit KeyTreeItem(const QString &data = "", KeyTreeItem *parent = nullptr);
    virtual ~KeyTreeItem();

    KeyTreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void appendChildren(KeyTreeItem *item);
    KeyTreeItem *parent();
    bool removeChildren(int position, int count);
    int childNumber() const;
    bool setData(int column, const QVariant &value);
    QString text();
    void clear();
    void sort(Qt::SortOrder order = Qt::AscendingOrder);
    virtual bool haveIcon();
    virtual void setIconId(const qint8 &value);
    virtual qint8 getIconId() const;
private:

    QString itemData;
    KeyTreeItem *parentItem;
    QList<KeyTreeItem*> childItems;
};

class KeyDbTreeItem : public KeyTreeItem {
public:
    explicit KeyDbTreeItem(const QString &data = "", KeyTreeItem *parent = nullptr)
        : KeyTreeItem(data,parent) {}
    void setIconId(const qint8 &value);
    qint8 getIconId() const;
    bool haveIcon();

private:
    qint8 iconId;
};

#endif // KEYTREEITEM_H
