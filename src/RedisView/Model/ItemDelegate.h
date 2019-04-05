#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QItemDelegate>
#include <QPainter>

class ValueTableDelegate : public QItemDelegate {

    Q_OBJECT

public:

    ValueTableDelegate(QObject* parent = 0);
    virtual void paint(QPainter* painter,
                       const QStyleOptionViewItem& option,
                       const QModelIndex& index) const;

};

#endif // ITEMDELEGATE_H
