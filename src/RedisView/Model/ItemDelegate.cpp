/**
* @file      ItemDelegate.cpp
* @brief     项代理
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "ItemDelegate.h"

ValueTableDelegate::ValueTableDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

void ValueTableDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    painter->drawText(option.rect,
                      Qt::TextWordWrap | Qt::AlignVCenter | Qt::AlignVCenter,
                      index.model()->data(index,Qt::DisplayRole).toString());
    drawFocus(painter, option, option.rect);
}
