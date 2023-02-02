#include "editlistmodel.h"

QVariant EditListModel::data(const QModelIndex &index, int role) const
{
    const int i = index.row();
    if (i >= 0 && i < items.count())
    {
        if (role == NameRole)
            return items[i].first;
        if (role == ValueRole)
            return items[i].second;
    }
    return QVariant();
}

QHash<int, QByteArray> EditListModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[NameRole] = "name";
    roles[ValueRole] = "value";
    return roles;
}


