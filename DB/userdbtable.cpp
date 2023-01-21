#include "userdbtable.h"

DBRecord UserDBTable::defaultAdmin()
{
    qDebug() << "@@@@@ UserDBTable::defaultAdmin ";
    DBRecord r;
    for (int i = 0; i < UserDBTable::Columns::COLUMN_COUNT; i++)
        r << QVariant("");
    r[UserDBTable::Columns::Code] = QVariant(0);
    r[UserDBTable::Columns::Name] = QVariant("АДМИНИСТРАТОР");
    r[UserDBTable::Columns::Role] = QVariant(UserDBTable::UserRole::UserRole_Admin);
    r[UserDBTable::Columns::Password] = QVariant("0");
    return r;
}

bool UserDBTable::isAdmin(const DBRecord& record)
{
    return (record.count() >= UserDBTable::Columns::COLUMN_COUNT) &&
            record[UserDBTable::Columns::Role].toInt() == UserDBTable::UserRole::UserRole_Admin;

}

QString UserDBTable::toAdminName(const QString & name)
{
    return name + USER_ADMIN_POSTFIX;
}

QString UserDBTable::fromAdminName(const QString & name)
{
    QString s = name;
    return s.remove(USER_ADMIN_POSTFIX);
}
