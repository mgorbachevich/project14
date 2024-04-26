#include "usernamemodel.h"
#include "users.h"
#include "tools.h"

void UserNameModel::update(const DBRecordList& records)
{
    Tools::debugLog("@@@@@ UserNameModel::update " + QString::number(records.count()));
    QStringList ss;
    for (int i = 0; i < records.count(); i++)
    {
        const DBRecord& user = records[i];
        const QString name = user[UserField_Name].toString();
        if (Users::isAdmin(user)) ss << Users::toAdminName(name);
        else ss << name;
    }
    setStringList(ss);
}

