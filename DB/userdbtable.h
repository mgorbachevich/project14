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

    UserDBTable(const QString& name, QObject *parent);
    int columnCount() { return Columns::COLUMN_COUNT; }
    static DBRecord defaultAdmin();
    static bool isAdmin(const DBRecord&);
    static QString toAdminName(const QString&);
    static QString fromAdminName(const QString&);
};

#endif // USERDBTABLE_H
