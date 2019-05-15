/**
* @file      ValueTableModel.h
* @brief     表模型
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef VALUETABLEMODEL_H
#define VALUETABLEMODEL_H

#include "Model/ValueTableItem.h"

class ValueTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    ValueTableModel(QObject *parent = nullptr);
    ~ValueTableModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;
    void setHeaderData(const QVector<QVariant> &data);
    bool insertRow(ValueTableItem * item);
    bool insertRow(int position, ValueTableItem *&item);
    bool removeRow(int position);
    bool removeItem(ValueTableItem *item);
    int childNumber(ValueTableItem *item) const;
    ValueTableItem * item(int row);
    void clear();
    void setColumnCount(int column);
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::ItemFlags flags(int column);
    bool setFlags(int column, Qt::ItemFlags flags);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

signals:
    void itemChanged(ValueTableItem *item, int column);

private:

    int _column;
    QVector<Qt::ItemFlags> flagsData;
    ValueTableItem *rootItem;
};

#endif // VALUETABLEMODEL_H
