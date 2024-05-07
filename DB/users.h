#ifndef USERS_H
#define USERS_H

#include "jsonarrayfile.h"
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

class Users : public JsonArrayFile
{
public:
    Users(AppManager*);
    DBRecord& getUser();
    void setUser(const DBRecord& r) { user = r; }
    DBRecordList getAll();
    DBRecord getByName(const QString&);
    void onInputUser(const QString&, const QString&, const QString&, const bool);
    void onDeleteUser(const QString&);
    void replaceOrInsertInputUser();
    void deleteInputUser();
    static int getCode(const DBRecord& u) { return u[UserField_Code].toInt(); }
    static QString getPassword(const DBRecord& u) { return u[UserField_Password].toString(); }
    static QString getName(const DBRecord& u) { return u.isEmpty() ? "" : u[UserField_Name].toString(); }
    static bool isAdmin(const DBRecord& u) { return u[UserField_Role].toInt() == UserRole_Admin; }
    static QString toAdminName(const QString&);
    static bool isEqual(const DBRecord&, const DBRecord&);
    static QString fromAdminName(const QString&);

protected:
    void sort() { Tools::sortByString(items, UserField_Name);  }
    DBRecord createUser(const QString&, const QString&, const QString&, const bool);
    void update();

    DBRecord user;
    DBRecord inputUser;
};

#endif // USERS_H
