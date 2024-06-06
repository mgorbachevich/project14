#include "usernamemodel.h"
#include "users.h"
#include "tools.h"

void UserNameModel::update(const DBRecordList& records)
{
    Tools::debugLog("@@@@@ UserNameModel::update " + QString::number(records.count()));
    QStringList ss;
    for (int i = 0; i < records.count(); i++) ss << Users::getDisplayName(records[i]);
    setStringList(ss);
}

