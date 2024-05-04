#include "edituserspanelmodel.h"
#include "users.h"

void EditUsersPanelModel::update(Users* userList)
{
    Tools::debugLog("@@@@@ EditUsersPanelModel::update " + Tools::intToString(userList->count()));
    users = userList;
    QStringList ss;
    for (int i = 0; i < users->count(); i++) ss << Users::getName(users->get(i));
    setStringList(ss);
}
