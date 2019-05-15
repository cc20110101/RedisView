/**
* @file      KeyTreeModel.h
* @brief     树模型
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef KEYTREEMODEL_H
#define KEYTREEMODEL_H

#include "Public/Define.h"
#include "KeyTreeItem.h"

class KeyTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    KeyTreeModel(QObject *parent = nullptr);
    ~KeyTreeModel();
    QVariant data(KeyTreeItem * item) const;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;
    bool insertRow(KeyTreeItem * item);
    bool removeChild(KeyTreeItem * item);
    bool removeChild(KeyTreeItem * item, int row);
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    void setRootItem(KeyTreeItem * item);
    KeyTreeItem *getRootItem() const;
    KeyTreeItem *itemFromIndex(const QModelIndex &index) const;
    QModelIndex indexFromItem(const KeyTreeItem *item) const;
    bool setText(KeyTreeItem * item, const QString &value);
    void clear();
    KeyTreeItem *getItem(const QModelIndex &index) const;
    void sortItem(KeyTreeItem *item, Qt::SortOrder order = Qt::AscendingOrder);
    void recurSortItem(KeyTreeItem *item, Qt::SortOrder order = Qt::AscendingOrder);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:

    QIcon * conicon;
    QIcon * dbicon;
    QIcon * keyicon;
    KeyTreeItem *rootItem;
    QString headerInfo;
};

#endif // KEYTREEMODEL_H
