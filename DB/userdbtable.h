#ifndef USERDBTABLE_H
#define USERDBTABLE_H

#include "dbtable.h"

#define USER_ADMIN_PREFIX ""
#define USER_ADMIN_POSTFIX "*"

class UserDBTable: public DBTable
{
public:
    enum Columns
    {
        Code = 0,
        Name,
        Role, // enum UserRole
        Password,
        COLUMN_COUNT
    };

    enum UserRole
    {
        UserRole_Admin = 0,
        UserRole_Operator = 1,
    };

    UserDBTable(const QString& name, QObject *parent): DBTable(name, parent)
    {
        qDebug() << "@@@@@ UserDBTable::UserDBTable";

        addColumn("Код",    "code",     "INT PRIMARY KEY");
        addColumn("Имя",    "name",     "TEXT");
        addColumn("Права",  "role",     "INT");
        addColumn("Пароль", "password", "TEXT");
    }
    int columnCount() { return Columns::COLUMN_COUNT; }
    static DBRecord defaultAdmin();
    static bool isAdmin(const DBRecord&);
    static QString toAdminName(const QString&);
    static QString fromAdminName(const QString&);
};

#endif // USERDBTABLE_H
