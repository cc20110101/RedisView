#include "Model/KeyTreeProxyModel.h"

KeyTreeProxyModel::KeyTreeProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool KeyTreeProxyModel::filterAcceptsRow(int sourceRow,
                                         const QModelIndex &sourceParent) const
{
    //    bool filter = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    //    if (filter) { //满足条件，显示
    //        return true;
    //    } else {
    //        // check all decendant's
    //        QModelIndex source_index = sourceModel()->index(sourceRow, 0, sourceParent);
    //        for (int i=0; i < sourceModel()->rowCount(source_index); ++i) {
    //            if (filterAcceptsRow(i, source_index)) {
    //                return true;
    //            }
    //        }
    //    }
    //    return false;

    QModelIndex source_index = sourceModel()->index(sourceRow, 0, sourceParent);
    if(sourceModel()->rowCount(source_index) > 0)
        return true;
    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

bool KeyTreeProxyModel::lessThan(const QModelIndex &left,
                                 const QModelIndex &right) const
{
    if (!left.isValid() || !right.isValid())
        return false;

    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    switch(leftData.userType()) {
    case QVariant::Invalid:
        return rightData.type() != QVariant::Invalid;
    case QVariant::Int:
        return leftData.toInt() < rightData.toInt();
    case QVariant::UInt:
        return leftData.toUInt() < rightData.toUInt();
    case QVariant::LongLong:
        return leftData.toLongLong() < rightData.toLongLong();
    case QVariant::ULongLong:
        return leftData.toULongLong() < rightData.toULongLong();
    case QMetaType::Float:
        return leftData.toFloat() < rightData.toFloat();
    case QVariant::Double:
        return leftData.toDouble() < rightData.toDouble();
    case QVariant::Char:
        return leftData.toChar() < rightData.toChar();
    case QVariant::Date:
        return leftData.toDate() < rightData.toDate();
    case QVariant::Time:
        return leftData.toTime() < rightData.toTime();
    case QVariant::DateTime:
        return leftData.toDateTime() < rightData.toDateTime();
    case QVariant::String:
    default:
        return leftData.toString() < rightData.toString();
    }
}
