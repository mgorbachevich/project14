#include "edituserspanelmodel3.h"
#include "tools.h"
#include "users.h"

QHash<int, QByteArray> EditUsersPanelModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "code";
    roles[SecondRole] = "name";
    return roles;
}

void EditUsersPanelModel3::update(Users* users)
{
    Tools::debugLog("@@@@@ EditUsersPanelModel3::update ");
    beginResetModel();
    items.clear();
    for (int i = 0; i < users->count(); i++)
    {
        const DBRecord& ui = users->get(i);
        QStringList data;
        data << Tools::intToString(Users::getCode(ui)) << Users::getDisplayName(ui);
        addItem(data);
    }
    endResetModel();
}
