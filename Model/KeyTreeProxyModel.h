#ifndef KEYTREEPROXYMODEL_H
#define KEYTREEPROXYMODEL_H

#include "Public/Define.h"

class KeyTreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    KeyTreeProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

};

#endif // KEYTREEPROXYMODEL_H
