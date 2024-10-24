#include <QJsonObject>
#include <QJsonDocument>
#include "users.h"
#include "appmanager.h"

Users::Users(AppManager *parent): JsonArrayFile(USERS_FILE, parent)
{
    Tools::debugLog("@@@@@ Users::Users");
    itemArrayName = DBTABLENAME_USERS;
    fields.insert(UserField_Code,     "code");
    fields.insert(UserField_Name,     "name");
    fields.insert(UserField_Role,     "role");
    fields.insert(UserField_Password, "password");
}

DBRecord &Users::getCurrentUser()
{
    if(user.isEmpty())
    {
        getAll();
        setCurrentUser(items[0]);
    }
    return user;
}

DBRecord Users::getByName(const QString &value)
{
    getAll();
    for (DBRecord& r : items) if (QString::compare(value, getName(r)) == 0) return r;
    return DBRecord();
}

DBRecord Users::createUser(const QString& code, const QString& name, const QString& password, const bool admin)
{
    DBRecord r;
    for (int i = 0; i < fields.count(); i++) r << QVariant("");
    r[UserField_Code] = code;
    r[UserField_Name] = name;
    r[UserField_Password] = password;
    r[UserField_Role] = admin ? Tools::toIntString(UserRole_Admin) : Tools::toIntString(UserRole_Operator);
    return r;
}

void Users::onDeleteUser(const QString& code)
{
    Tools::debugLog(QString("@@@@@ Users::onDeleteUser %1").arg(code));
    DBRecord* p = getByCode(Tools::toInt(code));
    if(p == nullptr)
    {
        showAttention(QString("Не найден пользователь с кодом %1?").arg(code));
        return;
    }
    inputUser = createUser(code, getName(*p), getPassword(*p), isAdmin(*p));
    appManager->showConfirmation(ConfirmSelector_DeleteUser,
                QString("Удалить пользователя %1 с кодом %2?").arg(getName(*p), code), "");
}

void Users::onInputUser(const QString& code, const QString& name, const QString& password, const bool admin)
{
    Tools::debugLog(QString("@@@@@ Users::onInputUser %1 %2 %3 %4").arg(
                        code, name, password, Tools::sortIncrement(admin)));
    if(code.isEmpty() && name.isEmpty() && password.isEmpty())
        return;
    if(Tools::toInt(code) <= 0)
    {
        showAttention("Неверный код пользователя " + code);
        return;
    }
    if(name.isEmpty())
    {
        showAttention("Неверное имя пользователя");
        return;
    }
    inputUser = createUser(code, name, password, admin);

    DBRecord* p = getByCode(Tools::toInt(code));
    if(p != nullptr)
    {
        if(!isEqual(*p, inputUser))
            appManager->showConfirmation(ConfirmSelector_ReplaceUser,
                QString("Уже есть пользователь с кодом %1, заменить?").arg(code), "");
        return;
    }
    DBRecord u = getByName(name);
    if(!u.isEmpty())
    {
        if(!isEqual(u, inputUser))
            appManager->showConfirmation(ConfirmSelector_ReplaceUser,
                QString("Уже есть пользователь с именем %1, заменить?").arg(name), "");
        return;
    }
    if(!isEqual(u, inputUser)) replaceOrInsertInputUser();
}

void Users::replaceOrInsertInputUser()
{
    if(JsonArrayFile::insertOrReplaceRecord(inputUser))
    {
        update();
        write();
    }
}

void Users::deleteInputUser()
{
    const int code = getCode(inputUser);
    Tools::debugLog("@@@@@ Users::deleteInputUser " + Tools::toString(code));
    if(getByCode(code) != nullptr)
    {
        const int i = getIndex(code);
        if(!getByIndex(i).isEmpty())
        {
            items.remove(i);
            update();
            write();
        }
    }
}

void Users::update()
{
    // Удалить всех с кодом 0:
    bool foundZeroCode = false;
    do
    {
        foundZeroCode = false;
        for (int i = 0; i < items.count(); i++)
        {
            if(getCode(items[i]) == 0)
            {
                foundZeroCode = true;
                items.remove(i);
                break;
            }
        }
    }
    while (foundZeroCode);

    // Нужен хотя бы один администратор:
    bool foundAdmin = false;
    for (DBRecord& r : items) foundAdmin |= isAdmin(r);
    if (!foundAdmin) items << createUser(Tools::toString(DEFAULT_ADMIN_CODE), DEFAULT_ADMIN_NAME, "", true);
    sort();
}

QString Users::toAdminName(const QString& name)
{
    return name + USER_ADMIN_POSTFIX;
}

QString Users::normalizedName(const QString& name)
{
    QString s = name;
    return s.remove(USER_ADMIN_POSTFIX);
}

QString Users::getDisplayName(const DBRecord& r)
{
    const QString name = getName(r);
    return isAdmin(r) ? toAdminName(name) : name;
}

bool Users::write()
{
    Tools::debugLog("@@@@@ Users::write");
    QJsonObject data;
    data.insert("data", toJsonObject());
    return Tools::writeTextFile(fileName, Tools::toString(data));
}

bool Users::isEqual(const DBRecord& u1, const DBRecord& u2)
{
    if(u1.count() != u2.count()) return false;
    for (int i = 0; i < u1.count(); i++)
        if(u1[i].toString() != u2[i].toString()) return false;
    return true;
}

DBRecordList Users::getAll()
{
    Tools::debugLog("@@@@@ Users::getAll");
    JsonArrayFile::getAll();
    update();
    return items;
}




