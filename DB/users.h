#ifndef USERS_H
#define USERS_H

#include "jsonfile.h"
#include "constants.h"
#include "tools.h"

#define USER_ADMIN_PREFIX ""
#define USER_ADMIN_POSTFIX "*"

enum UserField
{
    UserField_Code = 0,
    UserField_Name = 1,
    UserField_Role = 2,
    UserField_Password = 3,
};

class Users : public JsonFile
{
public:
    Users(QObject*);
    DBRecord& getUser() { return user; }
    void setDefaultAdmin() { setUser(defaultAdmin()); }
    void setUser(const DBRecord& r) { user = r; }
    DBRecord getByName(const QString&);
    static int getCode(const DBRecord& u) { return u[UserField_Code].toInt(); }
    static QString getPassword(const DBRecord& u) { return u[UserField_Password].toString(); }
    static QString getName(const DBRecord& u) { return u[UserField_Name].toString(); }
    static bool isAdmin(const DBRecord& u) { return u[UserField_Role].toInt() == UserRole_Admin; }
    static QString toAdminName(const QString&);
    static QString fromAdminName(const QString&);

protected:
    void sort() { Tools::sortByString(items, UserField_Name);  }
    DBRecord defaultAdmin();

    DBRecord user;

};

#endif // USERS_H
