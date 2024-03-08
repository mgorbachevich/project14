#include "userdbtable.h"
#include "tools.h"

UserDBTable::UserDBTable(const QString& name, QObject *parent): DBTable(name, parent)
{
    Tools::debugLog("@@@@@ UserDBTable::UserDBTable");

    addColumn("Код",    "code",     "INT PRIMARY KEY");
    addColumn("Имя",    "name",     "TEXT");
    addColumn("Права",  "role",     "INT");
    addColumn("Пароль", "password", "TEXT");
}

DBRecord UserDBTable::defaultAdmin()
{
    Tools::debugLog("@@@@@ UserDBTable::defaultAdmin");
    DBRecord r;
    for (int i = 0; i < UserDBTable::Columns::COLUMN_COUNT; i++)
        r << QVariant("");
    r[UserDBTable::Code] = QVariant(0);
    r[UserDBTable::Name] = QVariant("АДМИНИСТРАТОР");
    r[UserDBTable::Role] = QVariant(UserRole_Admin);
    r[UserDBTable::Password] = QVariant("0");
    return r;
}

bool UserDBTable::isAdmin(const DBRecord& record)
{
    return (record.count() >= UserDBTable::COLUMN_COUNT) &&
            record[UserDBTable::Role].toInt() == UserRole_Admin;
}

QString UserDBTable::toAdminName(const QString & name)
{
    //return name + USER_ADMIN_POSTFIX;
    return USER_ADMIN_PREFIX + name + USER_ADMIN_POSTFIX;
}

QString UserDBTable::fromAdminName(const QString & name)
{
    QString s = name;
    return s.remove(USER_ADMIN_PREFIX).remove(USER_ADMIN_POSTFIX);
}
