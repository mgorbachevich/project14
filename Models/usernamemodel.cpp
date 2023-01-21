#include "usernamemodel.h"

void UserNameModel::update(const DBRecordList& newUsers)
{
    qDebug() << "@@@@@ UserNameModel::update";
    users.clear();
    users.append(newUsers);
}

DBRecord &UserNameModel::userByIndex(const int index)
{
    return index < users.count() ? users[index] : *new DBRecord();
}
