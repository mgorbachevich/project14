#include "edituserspanelmodel.h"
#include "users.h"

void EditUsersPanelModel::update(Users* userList)
{
    Tools::debugLog("@@@@@ EditUsersPanelModel::update " + Tools::intToString(userList->count()));
    users = userList;
    QStringList ss;
    for (int i = 0; i < users->count(); i++)
    {
        const DBRecord& ui = users->get(i);
        ss << Tools::intToString(Users::getCode(ui)) + LIST_ROW_DELIMETER + Users::getDisplayName(ui);
    }
    setStringList(ss);
}
