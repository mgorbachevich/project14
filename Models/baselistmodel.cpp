#include "baselistmodel.h"

QVariant BaseListModel::data(const QModelIndex &index, int role) const
{
    return (index.isValid() && role == ValueRole)? stringList().at(index.row()) : QVariant();
}

QHash<int, QByteArray> BaseListModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[ValueRole] = "value";
    return roles;
}




