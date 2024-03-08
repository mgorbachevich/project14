#include "usernamemodel.h"
#include "userdbtable.h"
#include "tools.h"

void UserNameModel::update(const DBRecordList& users)
{
    Tools::debugLog("@@@@@ UserNameModel::update " + QString::number(users.count()));
    QStringList ss;
    for (int i = 0; i < users.count(); i++)
    {
        const DBRecord& user = users[i];
        const QString name = user[UserDBTable::Name].toString();
        if (UserDBTable::isAdmin(user))
            ss << UserDBTable::toAdminName(name);
        else
            ss << name;
    }
    setStringList(ss);
}

