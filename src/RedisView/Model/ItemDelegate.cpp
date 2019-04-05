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
