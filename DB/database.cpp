#include <QSqlError>
#include <QSqlQuery>
#include "database.h"
#include "showcasedbtable.h"
#include "productdbtable.h"
#include "labelformatdbtable.h"
#include "settingdbtable.h"
#include "transactiondbtable.h"
#include "resourcedbtable.h"
#include "userdbtable.h"
#include "logdbtable.h"
#include "tools.h"
#include "constants.h"
#include "requestparser.h"

DataBase::DataBase(Settings& globalSettings, QObject *parent):
    QObject(parent), settings(globalSettings)
{
    qDebug() << "@@@@@ DataBase::DataBase ";

    tables.append(new ProductDBTable(DBTABLENAME_PRODUCTS, this));
    tables.append(new UserDBTable(DBTABLENAME_USERS, this));
    tables.append(new LabelFormatDBTable(DBTABLENAME_LABELFORMATS, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGEFILES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_PICTURES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MOVIES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_SOUNDS, this));
    tables.append(new ShowcaseDBTable(DBTABLENAME_SHOWCASE, this));
    tables.append(new SettingDBTable(DBTABLENAME_SETTINGS, this));
    tables.append(new LogDBTable(DBTABLENAME_LOG, this));
    tables.append(new TransactionDBTable(DBTABLENAME_TRANSACTIONS, this));
}

DataBase::~DataBase()
{
    qDebug() << "@@@@@ DataBase::~DataBase ";
    close(productDB);
    close(settingsDB);
    close(logDB);
    close(tempDB);
    if(REMOVE_TEMP_DB) Tools::removeFile(Tools::dataBaseFilePath(DB_TEMP_NAME));
}

bool DataBase::init()
{
    qDebug() << "@@@@@ DataBase::init";
    QString path = Tools::dataBaseFilePath(DB_PRODUCT_NAME);
    if(REMOVE_PRODUCT_DB_ON_START) QFile(path).remove();
    bool exists = QFile(path).exists();
    qDebug() << QString("@@@@@ DataBase::init %1 exists %2").arg(path, Tools::boolToString(exists));
    opened = addAndOpen(productDB, path);
    if(!exists && opened)
    {
        qDebug() << QString("@@@@@ DataBase::init %1 create tables").arg(path);
        opened &= createTable(productDB, getTable(DBTABLENAME_PRODUCTS));
        opened &= createTable(productDB, getTable(DBTABLENAME_USERS));
        opened &= createTable(productDB, getTable(DBTABLENAME_LABELFORMATS));
        opened &= createTable(productDB, getTable(DBTABLENAME_MESSAGES));
        opened &= createTable(productDB, getTable(DBTABLENAME_MESSAGEFILES));
        opened &= createTable(productDB, getTable(DBTABLENAME_PICTURES));
        opened &= createTable(productDB, getTable(DBTABLENAME_MOVIES));
        opened &= createTable(productDB, getTable(DBTABLENAME_SOUNDS));
        opened &= createTable(productDB, getTable(DBTABLENAME_SHOWCASE));
    }
    if (!opened) return false;

    copyDBFiles(DB_PRODUCT_NAME, DB_TEMP_NAME);
    opened = addAndOpen(tempDB, Tools::dataBaseFilePath(DB_TEMP_NAME), false);
    if (!opened) return false;

    path = Tools::dataBaseFilePath(DB_SETTINGS_NAME);
    if(REMOVE_SETTINGS_DB_ON_START) QFile(path).remove();
    exists = QFile(path).exists();
    qDebug() << QString("@@@@@ DataBase::init %1 exists %2").arg(path, Tools::boolToString(exists));
    opened = addAndOpen(settingsDB, path);
    if(!exists && opened) opened &= createTable(settingsDB, getTable(DBTABLENAME_SETTINGS));
    if (!opened) return false;

    path = Tools::dataBaseFilePath(DB_LOG_NAME);
    if(REMOVE_LOG_DB_ON_START) QFile(path).remove();
    exists = QFile(path).exists();
    qDebug() << QString("@@@@@ DataBase::init %1 exists %2").arg(path, Tools::boolToString(exists));
    opened = addAndOpen(logDB, path);
    if(!exists && opened)
    {
        opened &= createTable(logDB, getTable(DBTABLENAME_LOG));
        opened &= createTable(logDB, getTable(DBTABLENAME_TRANSACTIONS));
    }
    qDebug() << "@@@@@ DataBase::init " << opened;
    return opened;
}

void DataBase::onStart()
{
    qDebug() << "@@@@@ DataBase::onStart ";
    init();
    if (opened) emit started();
    else
    {
        qDebug() << "@@@@@ DataBase::onStart ERROR";
        emit showMessage("ВНИМАНИЕ!", "Ошибка при открытии базы данных");
    }
}

bool DataBase::open(QSqlDatabase& db, const QString& name)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::openDBFile ";
    const QString path = Tools::dataBaseFilePath(name);
    qDebug() << QString("@@@@@ DataBase::open %1").arg(path);
    if(!QFile(path).exists()) return false;
    return db.isOpen() ? true : db.open();
}

bool DataBase::addAndOpen(QSqlDatabase& db, const QString& filePath, const bool open)
{
    qDebug() << "@@@@@ DataBase::addAndOpen " << filePath;
    if(db.isOpen()) return true;
    db = QSqlDatabase::addDatabase("QSQLITE", filePath);
    db.setDatabaseName(filePath);
    //db.setHostName(hostName);
    if(!open || db.open()) return true;
    qDebug() << "@@@@@ DataBase::addAndOpen ERROR";
    return false;
}

DBTable* DataBase::getTable(const QString &name) const
{
    for (DBTable* t : tables) if (t->name == name) return t;
    qDebug() << "@@@@@ DataBase::getTable ERROR " << name;
    return nullptr;
}

QString DataBase::getProductMessageById(const QString& id)
{
    DBRecord r;
    if(selectById(productDB, DBTABLENAME_MESSAGES, id, r) && r.count() > ResourceDBTable::Value)
        return r[ResourceDBTable::Value].toString();
    return "";
}

bool DataBase::createTable(const QSqlDatabase& db, DBTable* table)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::createTable: name =" << table->name;

    QString sql =  "CREATE TABLE " + table->name + " (";
    for (int i = 0; i < table->columnCount(); i++)
    {
        sql += table->columnName(i) + " " + table->columnType(i);
        sql += (i == table->columnCount() - 1) ? ")" : ", ";
    }
    return executeSQL(db, sql);
}

bool DataBase::executeSQL(const QSqlDatabase& db, const QString& sql)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::executeSQL: sql =" << sql;

    QSqlQuery q(db);
    if (!q.exec(sql))
    {
        qDebug() << "DataBase::executeSQL @@@@@ ERROR: " << q.lastError().text();
        //saveLog(LogDBTable::LogType_Error, "БД. Не выполнен запрос " + sql);
        return false;
    }
    return true;
}

bool DataBase::executeSelectSQL(const QSqlDatabase& db, DBTable* table, const QString& sql, DBRecordList& resultRecords)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::executeSelectSQL: sql =" << sql;
    resultRecords.clear();

    QSqlQuery q(db);
    if (!q.exec(sql))
    {
        qDebug() << "@@@@@ DataBase::executeSelectSQL ERROR: " << q.lastError().text();
        //saveLog(LogDBTable::LogType_Error, "БД. Не выполнен запрос " + sql, log);
        return false;
    }
    while (q.next())
    {
        DBRecord r;
        for(int i = 0; i < table->columnCount(); i++)
            r << q.value(i);
        resultRecords.append(r);
    }
    qDebug() << "@@@@@ DataBase::executeSelectSQL: records =" << resultRecords.count();
    return resultRecords.count() > 0;
}

bool DataBase::selectById(const QSqlDatabase& db, const QString& table, const QString& id, DBRecord& resultRecord)
{
    return selectById(db, getTable(table), id, resultRecord);
}

bool DataBase::selectById(const QSqlDatabase& db, DBTable* table, const QString& id, DBRecord& resultRecord)
{
    qDebug() << "@@@@@ DataBase::selectById " << id;
    resultRecord.clear();
    if(table == nullptr) return false;
    QString sql = QString("SELECT * FROM %1 WHERE %2 = '%3'").arg(table->name, table->columnName(0), id);
    DBRecordList records;
    if(!executeSelectSQL(db, table, sql, records)) return false;
    if(records.count() > 0) resultRecord.append(records.at(0));
    return true;
}

void DataBase::selectAll(const QSqlDatabase& db, DBTable *table, DBRecordList& resultRecords)
{
    qDebug() << "@@@@@ DataBase::selectAll ";
    resultRecords.clear();
    if(table != nullptr)
    {
        DBRecordList records;
        QString sql = QString("SELECT * FROM %1").arg(table->name);
        executeSelectSQL(db, table, sql, records);
        resultRecords.append(table->checkList(this, records));
    }
}

bool DataBase::removeAll(const QSqlDatabase& db, DBTable* table)
{
    qDebug() << "@@@@@ DataBase::removeAll: table =" << table->name;
    QString sql = QString("DELETE FROM %1").arg(table->name);
    bool ok = executeSQL(db, sql);
    return ok;
}

bool DataBase::removeRecord(const QSqlDatabase& db, DBTable* table, const QString& code)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::removeRecord: table name =" << table->name;
    QString sql = QString("DELETE FROM %1 WHERE %2 = '%3'").arg(table->name, table->columnName(0), code);
    return executeSQL(db, sql);
}

bool DataBase::insertRecord(const QSqlDatabase& sqlDb, DBTable *table, const DBRecord& record)
{
    qDebug() << "@@@@@ DataBase::insertRecord";
    if (!opened)
    {
        qDebug() << "@@@@@ DataBase::insertRecord ERROR (!opened)";
        return false;
    }
    DBRecord checkedRecord = table->checkRecord(record);
    if (checkedRecord.isEmpty())
    {
        qDebug() << "@@@@@ DataBase::insertRecord ERROR (checkedRecord.isEmpty())";
        //saveLog(LogDBTable::LogType_Error, "БД. Не сохранена запись в таблице " + table->name);
        return false;
    }

    QString code = checkedRecord.at(0).toString();
    QString sql;
    DBRecord r;
    qDebug() << "@@@@@ DataBase::insertRecord " << table->name << code;
    if (selectById(sqlDb, table, code, r)) removeRecord(sqlDb, table, code);
    sql = "INSERT INTO " + table->name + " (";
    for (int i = 0; i < table->columnCount(); i++)
    {
        sql += table->columnName(i);
        sql += (i == table->columnCount() - 1) ? ")" : ", ";
    }
    sql +=  " VALUES (";
    for (int i = 0; i < table->columnCount(); i++)
    {
        sql += (i < checkedRecord.count()) ? "'" + checkedRecord[i].toString() + "'" : "''";
        sql += (i == table->columnCount() - 1) ? ")" : ", ";
    }
    return executeSQL(sqlDb, sql);
}

bool DataBase::insertSettingsRecord(const DBRecord &r)
{
    qDebug() << "@@@@@ DataBase::insertSettingsRecord";
    return insertRecord(settingsDB, getTable(DBTABLENAME_SETTINGS), r);
}

bool DataBase::copyDBFiles(const QString& fromName, const QString& toName)
{
    QString from = Tools::dataBaseFilePath(fromName);
    QString to = Tools::dataBaseFilePath(toName);
    qDebug() << QString("@@@@@ DataBase::copyDBFiles %1 >> %2").arg(from, to);
    if(Tools::copyFile(from, to)) return true;
    qDebug() << "@@@@@ DataBase::copyDBFiles ERROR";
    return false;
}

void DataBase::saveLog(const int type, const int source, const QString &comment)
{
    if (!opened) return;
    int logging = settings.getItemIntValue(SettingCode_Logging);
    if (type > 0 && type <= logging)
    {
        qDebug() << "@@@@@ DataBase::saveLog: " << type << source << comment;
        LogDBTable* t = (LogDBTable*)getTable(DBTABLENAME_LOG);
        if (t != nullptr && insertRecord(logDB, t, t->createRecord(type, source, comment)))
            removeOldLogRecords();
        else
            qDebug() << "@@@@@ DataBase::saveLog ERROR";
    }
}

void DataBase::removeOldLogRecords()
{
    if (!opened) return;
    qDebug() << "@@@@@ DataBase::removeOldLogRecords";
    DBTable* t = getTable(DBTABLENAME_LOG);
    quint64 logDuration = settings.getItemIntValue(SettingCode_LogDuration);
    if(t != nullptr && logDuration > 0) // Remove old records
    {
        quint64 first = Tools::currentDateTimeToUInt() - logDuration * 24 * 60 * 60 * 1000;
        QString sql = QString("DELETE FROM %1 WHERE %2 < '%3'").
            arg(t->name, t->columnName(LogDBTable::DateTime), QString::number(first));
        logDB.exec(sql);
    }
}

void DataBase::clearLog()
{
    if (!opened) return;
    qDebug() << "@@@@@ DataBase::clearLog";
    DBTable* t = getTable(DBTABLENAME_LOG);
    if(t != nullptr) logDB.exec(QString("DELETE FROM %1").arg(t->name));
}

void DataBase::select(const DBSelector selector, const QString& param)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос

    qDebug() << "@@@@@ DataBase::select: selector =" << selector;
    DBRecordList resultRecords;
    switch(selector)
    {
    case DBSelector_UpdateSettings:
    case DBSelector_ChangeSettings:
    // Запрос списка настроек:
        selectAll(settingsDB, getTable(DBTABLENAME_SETTINGS), resultRecords);
        break;

    case DBSelector_GetUsers:
    // Запрос списка пользователей для авторизации:
        selectAll(productDB, getTable(DBTABLENAME_USERS), resultRecords);
        Tools::sortByString(resultRecords, UserDBTable::Name);
        break;

    case DBSelector_GetLog:
    // Запрос списка записей лога:
        selectAll(logDB, getTable(DBTABLENAME_LOG), resultRecords);
        break;

    case DBSelector_GetShowcaseProducts:
    // Запрос списка товаров для отображения на экране Showcase:
    {
        DBRecordList showcaseRecords;
        selectAll(productDB, getTable(DBTABLENAME_SHOWCASE), showcaseRecords);
        for (int i = 0; i < showcaseRecords.count(); i++)
        {
            DBRecord r;
            QString productCode = showcaseRecords[i][ShowcaseDBTable::Code].toString();
            if (selectById(productDB, DBTABLENAME_PRODUCTS,  productCode, r) && ProductDBTable::isForShowcase(r))
                resultRecords.append(r);
        }
        switch(Tools::stringToInt(param))
        {
        case Sort_Code: Tools::sortByInt(resultRecords, ProductDBTable::Code); break;
        case Sort_Name: Tools::sortByString(resultRecords, ProductDBTable::Name); break;
        }
        break;
    }

    case DBSelector_GetAuthorizationUserByName:
    // Запрос пользователя по имени для авторизации:
    {
        DBTable* t = getTable(DBTABLENAME_USERS);
        QString sql = QString("SELECT * FROM %1 WHERE %2 = '%3'").arg(t->name, t->columnName(UserDBTable::Name), param);
        executeSelectSQL(productDB, t, sql, resultRecords);
        break;
    }

    case DBSelector_GetImageByResourceCode:
    // Запрос картинки из ресурсов по коду ресурса:
    {
        DBRecord r;
        if(selectById(productDB, DBTABLENAME_PICTURES, param, r)) resultRecords.append(r);
        break;
    }

    case DBSelector_GetMessageByResourceCode:
    // Запрос сообщения (или файла сообщения?) из ресурсов по коду ресурса:
    // todo
    {
        DBRecord r;
        if(selectById(productDB, DBTABLENAME_MESSAGES, param, r)) resultRecords.append(r);
        break;
    }

    case DBSelector_GetProductsByGroupCode:
    case DBSelector_GetProductsByGroupCodeIncludeGroups:
    // Запрос списка товаров по коду группы (исключая / включая группы):
    {
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), param);
        if (selector == DBSelector_GetProductsByGroupCode)
            sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 DESC, %2 ASC").arg(t->columnName(ProductDBTable::Type), t->columnName(ProductDBTable::Name));
        executeSelectSQL(productDB, t, sql, resultRecords);
        break;
    }

    case DBSelector_GetProductsByInputCode:
    // Запрос списка товаров по фрагменту кода:
    {
        QString p = param.trimmed();
        if (p.isEmpty()) break;
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += " WHERE " + t->columnName(ProductDBTable::Code) + " LIKE '" + p + "%'";
        sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC").arg(t->columnName(ProductDBTable::Code));
        executeSelectSQL(productDB, t, sql, resultRecords);
        break;
    }

    case DBSelector_GetProductsByFilteredCode:
    // Запрос списка товаров по фрагменту кода исключая группы:
    {
        QString p = param.trimmed();
        if (p.isEmpty()) break;
        if (!settings.getItemBoolValue(SettingCode_SearchType) &&
            p.size() < settings.getItemIntValue(SettingCode_SearchCodeSymbols)) break;
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += " WHERE " + t->columnName(ProductDBTable::Code) + " LIKE '%" + p + "%'";
        sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC").arg(t->columnName(ProductDBTable::Code));
        executeSelectSQL(productDB, t, sql, resultRecords);
        break;
    }

    case DBSelector_GetProductsByFilteredBarcode:
    // Запрос списка товаров по фрагменту штрих-кода исключая группы:
    {
        QString p = param.trimmed();
        if (p.isEmpty()) break;
        if (!settings.getItemBoolValue(SettingCode_SearchType) &&
            p.size() < settings.getItemIntValue(SettingCode_SearchBarcodeSymbols)) break;
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += " WHERE " + t->columnName(ProductDBTable::Barcode) + " LIKE '%" + p + "%'";
        sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC").arg(t->columnName(ProductDBTable::Barcode));
        executeSelectSQL(productDB, t, sql, resultRecords);
        break;
    }

    case DBSelector_GetProductByInputCode:
    case DBSelector_SetProductByInputCode:
    case DBSelector_RefreshCurrentProduct:
    // Запрос товара по коду
    {
        DBRecord r;
        if(selectById(productDB, DBTABLENAME_PRODUCTS, param, r)) resultRecords.append(r);
        break;
    }

    default: break;
    }
    qDebug() << "@@@@@ DataBase::selectByParam: records =" << resultRecords.count();
    emit requestResult(selector, resultRecords, true);
}

void DataBase::afterNetAction()
{
    qDebug() << "@@@@@ DataBase::afterNetAction ";
    close(tempDB);
    close(productDB);
    copyDBFiles(DB_TEMP_NAME, DB_PRODUCT_NAME);
    open(productDB, DB_PRODUCT_NAME);
}

void DataBase::select(const DBSelector selector, const DBRecordList& param)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос

    qDebug() << "@@@@@ DataBase::select: selector =" << selector << param.count();
    DBRecordList resultRecords;
    switch(selector)
    {
    case DBSelector_GetShowcaseResources:
    // Запрос списка картинок из ресурсов для списка товаров:
    {
        for (int i = 0; i < param.count(); i++)
        {
            QString imageCode = param[i][ProductDBTable::PictureCode].toString();
            DBRecord r;
            selectById(productDB, DBTABLENAME_PICTURES, imageCode, r);
            resultRecords.append(r);
        }
        break;
    }

    default: break;
    }
    emit requestResult(selector, resultRecords, true);
}

void DataBase::updateSettingsRecord(const DBSelector selector, const DBRecord& record)
{
    qDebug() << "@@@@@ DataBase::updateSettingsRecord: selector =" << selector;
    bool result = false;
    switch(selector)
    {
    case DBSelector_ReplaceSettingsItem:
    {
        result = insertRecord(settingsDB, getTable(DBTABLENAME_SETTINGS), record);
        break;
    }
    default: break;
    }
    emit requestResult(selector, DBRecordList(), result);
}

void DataBase::saveTransaction(const DBRecord& t)
{
    qDebug() << "@@@@@ DataBase::saveTransaction";
    insertRecord(logDB, getTable(DBTABLENAME_TRANSACTIONS), t);
}

QString DataBase::netDelete(const QString& tableName, const QString& codeList)
{
    // Удаление из таблицы по списку кодов

    qDebug() << "@@@@@ DataBase::netDelete" << tableName << codeList;
    saveLog(LogType_Error, LogSource_DB, QString(" Удаление. Таблица: %1").arg(tableName));
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    int logging = settings.getItemIntValue(SettingCode_Logging);
    bool detailedLog = logging >= LogType_Info;
    DBTable* t = getTable(tableName);
    QStringList codes = codeList.split(','); // Коды товаров через запятую

    if(t == nullptr || !open(tempDB, DB_TEMP_NAME))
    {
        errorCount = 1;
        errorCode = LogError_UnknownTable;
        description = "Неизвестная таблица";
    }
    else if (codes.isEmpty() || codes[0].isEmpty()) // Delete all records
    {
        if(removeAll(tempDB, t) && detailedLog)
        {
            QString s = QString("Удалены все записи таблицы %1").arg(tableName);
            saveLog(LogType_Warning, LogSource_DB, s);
        }
    }
    else
    {
        for (int i = 0; i < codes.count(); i++)
        {
            if(removeRecord(tempDB, t, codes[i]))
            {
                if (detailedLog)
                {
                    QString s = QString("Запись удалена. Таблица: %1. Код: %2").arg(tableName, codes[i]);
                    saveLog(LogType_Warning, LogSource_DB, s);
                }
            }
            else
            {
                errorCount++;
                errorCode = LogError_RecordNotFound;
                description = "Запись не найдена";
                if (detailedLog)
                {
                    QString s = QString("Ошибка выгрузки записи. Таблица: %1. Код ошибки: %2. Описание: %3").
                            arg(tableName, QString::number(errorCode), description);
                    saveLog(LogType_Error, LogSource_DB, s);
                }
            }
        }
    }
    QString s = QString("Удаление завершено. Таблица: %1. Ошибки: %2. Описание: %3").
            arg(tableName, QString::number(errorCount), description);
    saveLog(LogType_Error, LogSource_DB, s);
    DBRecordList records;
    QString resultJson = RequestParser::makeResultJson(errorCode, description, tableName, DBTable::toJsonString(t, records));
    qDebug() << "@@@@@ DataBase::netDelete: result" << resultJson;
    return resultJson;
}

QString DataBase::netUpload(const QString& tableName, const QString& codeList)
{
    // Выгрузка из таблицы по списку кодов

    qDebug() << "@@@@@ DataBase::netUpload" << tableName << codeList;
    saveLog(LogType_Error, LogSource_DB, QString("Выгрузка. Таблица: %1").arg(tableName));
    int recordCount = 0;
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    bool detailedLog = settings.getItemIntValue(SettingCode_Logging) >= LogType_Info;
    DBRecordList records;
    DBTable* t = getTable(tableName);
    QStringList codes = codeList.split(','); // Коды товаров через запятую

    if(t == nullptr || !open(tempDB, DB_TEMP_NAME))
    {
        errorCount = 1;
        errorCode = LogError_UnknownTable;
        description = "Неизвестная таблица";
    }
    else if (codes.isEmpty()) // Upload all records
    {
        selectAll(tempDB, t, records);
        recordCount = records.count();
        if (detailedLog)
        {
            for (int i = 0; i < recordCount; i++)
            {
                QString s = QString("Выгружена запись. Таблица: %1. Код: %2").
                        arg(tableName, records[i].at(0).toString());
                saveLog(LogType_Warning, LogSource_DB, s);
            }
        }
    }
    else
    {
        for (int i = 0; i < codes.count(); i++)
        {
            DBRecord r;
            if(selectById(tempDB, t, codes[i], r))
            {
                recordCount++;
                records.append(r);
                if (detailedLog)
                {
                    QString s = QString("Запись выгружена. Таблица: %1. Код: %2").arg(tableName, r.at(0).toString());
                    saveLog(LogType_Warning, LogSource_DB, s);
                }
            }
            else
            {
                errorCount++;
                errorCode = LogError_RecordNotFound;
                description = "Запись не найдена";
                if (detailedLog)
                {
                    QString s = QString("Ошибка выгрузки записи. Таблица: %1. Код ошибки: %2. Описание: %3").
                            arg(tableName, QString::number(errorCode), description);
                    saveLog(LogType_Error, LogSource_DB, s);
                }
            }
        }
    }
    QString s = QString("Выгрузка завершена. Таблица: %1. Записи: %2. Ошибки: %3. Описание: %4").
            arg(tableName, QString::number(recordCount), QString::number(errorCount), description);
    saveLog(LogType_Error, LogSource_DB, s);
    QString resultJson = RequestParser::makeResultJson(errorCode, description, tableName, DBTable::toJsonString(t, records));
    qDebug() << "@@@@@ DataBase::netUpload: result" << resultJson;
    return resultJson;
}

void DataBase::netDownload(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount)
{
    qDebug() << "@@@@@ DataBase::netDownload";
    if(!open(tempDB, DB_TEMP_NAME)) return;
    bool detailedLog = settings.getItemIntValue(SettingCode_Logging) >= LogType_Info;
    QList tables = records.keys();
    for (DBTable* table : tables)
    {
        DBRecordList tableRecords = records.value(table);
        for(DBRecord& r : tableRecords)
        {
            QString code = r.count() > 0 ? r.at(0).toString() : "";
            QString s;
            if(code.isEmpty() || !insertRecord(tempDB, table, r))
            {
                errorCount++;
                s = QString("Ошибка загрузки записи. Таблица: %1. Код: %2. Код ошибки: %3. Описание: Некорректная запись").
                        arg(table->name, code, QString::number(LogError_WrongRecord));
                if (detailedLog) saveLog(LogType_Error, LogSource_DB, s);
            }
            else
            {
                successCount++;
                s = QString("Запись загружена. Таблица: %1. Код: %2").arg(table->name, code);
                if (detailedLog) saveLog(LogType_Warning, LogSource_DB, s);
            }
            qDebug() << "@@@@@ DataBase::netDownload " << s;
        }
    }
}

