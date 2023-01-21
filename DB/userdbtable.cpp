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
