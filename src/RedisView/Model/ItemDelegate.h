/**
* @file      ItemDelegate.h
* @brief     项代理
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
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
