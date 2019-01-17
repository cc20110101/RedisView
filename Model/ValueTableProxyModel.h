#ifndef VALUETABLEPROXYMODEL_H
#define VALUETABLEPROXYMODEL_H

#include "Public/Define.h"

class ValueTableProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    ValueTableProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

};

#endif // VALUETABLEPROXYMODEL_H
