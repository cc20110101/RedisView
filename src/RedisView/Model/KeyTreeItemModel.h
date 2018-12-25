#ifndef KEYTREEITEMMODEL_H
#define KEYTREEITEMMODEL_H
#include <QAbstractItemModel>

class KeyTreeItemModel : public QAbstractItemModel
{
public:
    explicit KeyTreeItemModel(QObject *parent=NULL);
};

#endif // KEYTREEITEMMODEL_H
