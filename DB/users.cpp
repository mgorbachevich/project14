#include <QJsonObject>
#include <QJsonDocument>
#include "users.h"

Users::Users(QObject *parent): JsonFile(USERS_FILE, parent)
{
    mainObjectName = "data";
    itemArrayName = "users";
    fields.insert(UserField_Code,     "code");
    fields.insert(UserField_Name,     "name");
    fields.insert(UserField_Role,     "role");
    fields.insert(UserField_Password, "password");
    setDefaultAdmin();
}

DBRecord *Users::getByName(const QString &value)
{
    bool ok = false;
    for (DBRecord& r : items) if (QString::compare(value, r[UserField_Name].toString()) == 0) return &r;
    return nullptr;
}

void Users::setUserByName(const QString &value)
{
    DBRecord* u = getByName(value);
    if(u == nullptr) setDefaultAdmin();
    else currentUser = *u;
}

DBRecord Users::defaultAdmin()
{
    Tools::debugLog("@@@@@ Users::defaultAdmin");
    DBRecord r;
    for (int i = 0; i < fields.count(); i++) r << QVariant("");
    r[UserField_Code] = QVariant(0);
    r[UserField_Name] = QVariant(DEFAULT_ADMIN_NAME);
    r[UserField_Role] = QVariant(UserRole_Admin);
    r[UserField_Password] = QVariant("");
    return r;
}

QString Users::toAdminName(const QString& name)
{
    return USER_ADMIN_PREFIX + name + USER_ADMIN_POSTFIX;
}

QString Users::fromAdminName(const QString& name)
{
    QString s = name;
    return s.remove(USER_ADMIN_PREFIX).remove(USER_ADMIN_POSTFIX);
}
