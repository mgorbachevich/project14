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
    DBRecord getCurrentUser() { return currentUser; }
    DBRecord* getByName(const QString&);
    void setDefaultAdmin() { currentUser = defaultAdmin(); }
    void setUserByName(const QString&);
    DBRecord defaultAdmin();
    bool isAdmin() { return isAdmin(currentUser); }
    static bool isAdmin(const DBRecord& user) { return user[UserField_Role].toInt() == UserRole_Admin; }
    DBRecordList getAll() { return items; }
    static QString toAdminName(const QString &);
    static QString fromAdminName(const QString &);

protected:
    void sort() { Tools::sortByString(items, UserField_Name);  }

    DBRecord currentUser;

};

#endif // USERS_H
