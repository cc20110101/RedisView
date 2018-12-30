#ifndef TREEITEM_H
#define TREEITEM_H
#include <QVariant>

class TreeItem
{
public:
    explicit TreeItem(const QString &data = "", TreeItem *parent = nullptr);
    ~TreeItem();

    TreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void appendChildren(TreeItem *item);
    TreeItem *parent();
    bool removeChildren(int position, int count);
    int childNumber() const;
    bool setData(int column, const QVariant &value);
    QString text();
    void clear();
    void setIconId(const qint8 &value);
    qint8 getIconId() const;
    void sort(Qt::SortOrder order = Qt::AscendingOrder);

private:

    qint8 iconId;
    QString itemData;
    TreeItem *parentItem;
    QList<TreeItem*> childItems;
};

#endif // TREEITEM_H
