#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "Public/Define.h"
#include "TreeItem.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    TreeModel(QObject *parent = 0);
    ~TreeModel();
    QVariant data(TreeItem * item) const;
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
    bool insertRow(TreeItem * item);
    bool removeChild(TreeItem * item);
    bool removeChild(TreeItem * item, int row);
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    void setRootItem(TreeItem * item);
    TreeItem *getRootItem() const;
    TreeItem *itemFromIndex(const QModelIndex &index) const;
    QModelIndex indexFromItem(const TreeItem *item) const;
    bool setText(TreeItem * item, const QString &value);
    void clear();
    TreeItem *getItem(const QModelIndex &index) const;
    void sortItem(TreeItem *item, Qt::SortOrder order = Qt::AscendingOrder);
private:

    QIcon * conicon;
    QIcon * dbicon;
    QIcon * keyicon;
    TreeItem *rootItem;
    QString headerInfo;
};

#endif // TREEMODEL_H
