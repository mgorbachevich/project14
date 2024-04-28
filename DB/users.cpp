#include <QJsonObject>
#include <QJsonDocument>
#include "users.h"

Users::Users(QObject *parent): JsonFile(USERS_FILE, parent)
{
    Tools::debugLog("@@@@@ Users::Users");
    mainObjectName = "data";
    itemArrayName = "users";
    fields.insert(UserField_Code,     "code");
    fields.insert(UserField_Name,     "name");
    fields.insert(UserField_Role,     "role");
    fields.insert(UserField_Password, "password");
}

DBRecord Users::getByName(const QString &value)
{
    getAll();
    for (DBRecord& r : items) if (QString::compare(value, getName(r)) == 0) return r;
    return DBRecord();
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

