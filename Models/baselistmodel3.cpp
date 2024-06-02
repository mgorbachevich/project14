#include "baselistmodel3.h"

QVariant BaseListModel3::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > items.count()) return QVariant();
    const QStringList& v = items[index.row()];
    switch(role)
    {
    case FirstRole: return v[0];
    case SecondRole: return v[1];
    case ThirdRole: return v[2];
    case FourthRole: return v[3];
    case FifthRole: return v[4];
    }
    return QVariant();
}

QHash<int, QByteArray> BaseListModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "first";
    roles[SecondRole] = "second";
    roles[ThirdRole] = "third";
    roles[FourthRole] = "third";
    roles[FifthRole] = "third";
    return roles;
}
