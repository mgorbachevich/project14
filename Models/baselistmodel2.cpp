#include "baselistmodel2.h"

QVariant BaseListModel2::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > items.count()) return QVariant();
    const StringPair& p = items[index.row()];
    if (role == FirstRole) return p.first;
    if (role == SecondRole) return p.second;
    return QVariant();
}

QHash<int, QByteArray> BaseListModel2::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "first";
    roles[SecondRole] = "second";
    return roles;
}
