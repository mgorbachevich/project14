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
    qDebug() << QString("@@@@@ DataBase::init %1 exists %2").arg(DB_PRODUCT_NAME, Tools::boolToString(exists));
    opened = addAndOpen(productDB, path);
    tables.append(new ProductDBTable(productDB, DBTABLENAME_PRODUCTS, this));
    tables.append(new UserDBTable(productDB, DBTABLENAME_USERS, this));
    tables.append(new LabelFormatDBTable(productDB, DBTABLENAME_LABELFORMATS, this));
    tables.append(new ResourceDBTable(productDB, DBTABLENAME_MESSAGES, this));
    tables.append(new ResourceDBTable(productDB, DBTABLENAME_MESSAGEFILES, this));
    tables.append(new ResourceDBTable(productDB, DBTABLENAME_PICTURES, this));
    tables.append(new ResourceDBTable(productDB, DBTABLENAME_MOVIES, this));
    tables.append(new ResourceDBTable(productDB, DBTABLENAME_SOUNDS, this));
    tables.append(new ShowcaseDBTable(productDB, DBTABLENAME_SHOWCASE, this));
    if(!exists && opened)
    {
        qDebug() << QString("@@@@@ DataBase::init %1 create tables").arg(DB_PRODUCT_NAME);
        opened &= createTable(getTable(DBTABLENAME_PRODUCTS));
        opened &= createTable(getTable(DBTABLENAME_USERS));
        opened &= createTable(getTable(DBTABLENAME_LABELFORMATS));
        opened &= createTable(getTable(DBTABLENAME_MESSAGES));
        opened &= createTable(getTable(DBTABLENAME_MESSAGEFILES));
        opened &= createTable(getTable(DBTABLENAME_PICTURES));
        opened &= createTable(getTable(DBTABLENAME_MOVIES));
        opened &= createTable(getTable(DBTABLENAME_SOUNDS));
        opened &= createTable(getTable(DBTABLENAME_SHOWCASE));
    }
    if (!opened) return false;

    copyDBFiles(DB_PRODUCT_NAME, DB_TEMP_NAME);
    opened = addAndOpen(tempDB, Tools::dataBaseFilePath(DB_TEMP_NAME), false);
    if (!opened) return false;

    path = Tools::dataBaseFilePath(DB_SETTINGS_NAME);
    if(REMOVE_SETTINGS_DB_ON_START) QFile(path).remove();
    exists = QFile(path).exists();
    opened = addAndOpen(settingsDB, path);
    tables.append(new SettingDBTable(settingsDB, DBTABLENAME_SETTINGS, this));
    if(!exists && opened) opened &= createTable(getTable(DBTABLENAME_SETTINGS));
    if (!opened) return false;

    path = Tools::dataBaseFilePath(DB_LOG_NAME);
    if(REMOVE_LOG_DB_ON_START) QFile(path).remove();
    exists = QFile(path).exists();
    opened = addAndOpen(logDB, path);
    tables.append(new LogDBTable(logDB, DBTABLENAME_LOG, this));
    tables.append(new TransactionDBTable(logDB, DBTABLENAME_TRANSACTIONS, this));
    if(!exists && opened)
    {
        opened &= createTable(getTable(DBTABLENAME_LOG));
        opened &= createTable(getTable(DBTABLENAME_TRANSACTIONS));
    }
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

DBTable *DataBase::getTable(const QString &name) const
{
    for (DBTable* t : tables) if (t->name == name) return t;
    qDebug() << "@@@@@ DataBase::getTable ERROR";
    return nullptr;
}

QString DataBase::getProductMessageById(const QString& id)
{
    DBRecord r;
    if(selectById(DBTABLENAME_MESSAGES, id, r) && r.count() > ResourceDBTable::Value)
        return r[ResourceDBTable::Value].toString();
    return "";
}

bool DataBase::createTable(DBTable* table)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::createTable: name =" << table->name;

    QString sql =  "CREATE TABLE " + table->name + " (";
    for (int i = 0; i < table->columnCount(); i++)
    {
        sql += table->columnName(i) + " " + table->columnType(i);
        sql += (i == table->columnCount() - 1) ? ")" : ", ";
    }
    return executeSQL(table->db, sql);
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

bool DataBase::executeSelectSQL(DBTable* table, const QString& sql, DBRecordList& resultRecords)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::executeSelectSQL: sql =" << sql;
    resultRecords.clear();

    QSqlQuery q(table->db);
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

bool DataBase::selectById(DBTable* table, const QString& id, DBRecord& resultRecord)
{
    qDebug() << "@@@@@ DataBase::selectById " << id;
    resultRecord.clear();
    if(table == nullptr) return false;
    QString sql = QString("SELECT * FROM %1 WHERE %2 = '%3'").arg(table->name, table->columnName(0), id);
    DBRecordList records;
    if(!executeSelectSQL(table, sql, records)) return false;
    if(records.count() > 0) resultRecord.append(records.at(0));
    return true;
}

bool DataBase::selectById(const QString& tableName, const QString& id, DBRecord& resultRecord)
{
    return selectById(getTable(tableName), id, resultRecord);
}

void DataBase::selectAll(DBTable *table, DBRecordList& resultRecords)
{
    qDebug() << "@@@@@ DataBase::selectAll ";
    resultRecords.clear();
    if(table != nullptr)
    {
        DBRecordList records;
        QString sql = QString("SELECT * FROM %1").arg(table->name);
        executeSelectSQL(table, sql, records);
        resultRecords.append(table->checkList(this, records));
    }
}

bool DataBase::removeAll(DBTable* table)
{
    qDebug() << "@@@@@ DataBase::removeAll: table =" << table->name;
    QString sql = QString("DELETE FROM %1").arg(table->name);
    bool ok = executeSQL(table->db, sql);
    return ok;
}

bool DataBase::removeRecord(DBTable* table, const QString& code)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::removeRecord: table name =" << table->name;
    QString sql = QString("DELETE FROM %1 WHERE %2 = '%3'").
            arg(table->name, table->columnName(0), code);
    return executeSQL(table->db, sql);
}

bool DataBase::insertRecord(DBTable *table, const DBRecord& record)
{
    return insertRecord(table->db, table, record);
}

bool DataBase::insertRecord(const QSqlDatabase& sqlDb, DBTable *table, const DBRecord& record)
{
    if (!opened) return false;
    DBRecord checkedRecord = table->checkRecord(record);
    if (checkedRecord.isEmpty())
    {
        qDebug() << "@@@@@ DataBase::insertRecord ERROR";
        //saveLog(LogDBTable::LogType_Error, "БД. Не сохранена запись в таблице " + table->name);
        return false;
    }

    QString code = checkedRecord.at(0).toString();
    QString sql;
    DBRecord r;
    qDebug() << "@@@@@ DataBase::insertRecord " << table->name << code;
    if (selectById(table, code, r)) removeRecord(table, code);
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

bool DataBase::copyDBFiles(const QString& fromName, const QString& toName)
{
    if(Tools::copyFile(Tools::dataBaseFilePath(fromName), Tools::dataBaseFilePath(toName))) return true;
    qDebug() << "@@@@@ DataBase::copyDBFiles ERROR";
    return false;
}

void DataBase::saveLog(const int type, const int source, const QString &comment)
{
    if (!opened) return;
    int logging = settings.getItemIntValue(Settings::SettingCode_Logging);
    if (type > 0 && type <= logging)
    {
        qDebug() << "@@@@@ DataBase::saveLog: " << type << source << comment;
        LogDBTable* t = (LogDBTable*)getTable(DBTABLENAME_LOG);
        if (t != nullptr && insertRecord(t, t->createRecord(type, source, comment)))
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
    quint64 logDuration = settings.getItemIntValue(Settings::SettingCode_LogDuration);
    if(t != nullptr && logDuration > 0) // Remove old records
    {
        quint64 first = Tools::currentDateTimeToUInt() - logDuration * 24 * 60 * 60 * 1000;
        QString sql = QString("DELETE FROM %1 WHERE %2 < '%3'").
            arg(t->name, t->columnName(LogDBTable::DateTime), QString::number(first));
        t->db.exec(sql);
    }
}

void DataBase::selectByParam(const DataBase::Selector selector, const QString& param)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос

    qDebug() << "@@@@@ DataBase::selectByParam: selector =" << selector;
    DBRecordList resultRecords;
    switch(selector)
    {
    case Selector::UpdateSettings:
    case Selector::ChangeSettings:
    // Запрос списка настроек:
        selectAll(getTable(DBTABLENAME_SETTINGS), resultRecords);
        break;

    case Selector::GetUserNames:
    // Запрос списка пользователей для авторизации:
        selectAll(getTable(DBTABLENAME_USERS), resultRecords);
        break;

    case Selector::GetLog:
    // Запрос списка записей лога:
        selectAll(getTable(DBTABLENAME_LOG), resultRecords);
        break;

    case Selector::GetShowcaseProducts:
    // Запрос списка товаров для отображения на экране Showcase:
    {
        DBTable* productTable = getTable(DBTABLENAME_PRODUCTS);
        DBTable* showcaseTable = getTable(DBTABLENAME_SHOWCASE);
        DBRecordList showcaseRecords;
        selectAll(showcaseTable, showcaseRecords);
        for (int i = 0; i < showcaseRecords.count(); i++)
        {
            DBRecord r;
            QString showcaseProductCode = showcaseRecords[i][ShowcaseDBTable::Code].toString();
            if (selectById(productTable, showcaseProductCode, r) && ProductDBTable::isForShowcase(r))
                resultRecords.append(r);
        }
        break;
    }

    case Selector::GetAuthorizationUserByName:
    // Запрос пользователя по имени для авторизации:
    {
        DBTable* t = getTable(DBTABLENAME_USERS);
        QString sql = QString("SELECT * FROM %1 WHERE %2 = '%3'").
                arg(t->name, t->columnName(UserDBTable::Name), param);
        executeSelectSQL(t, sql, resultRecords);
        break;
    }

    case Selector::GetImageByResourceCode:
    // Запрос картинки из ресурсов по коду ресурса:
    {
        DBRecord r;
        if(selectById(DBTABLENAME_PICTURES, param, r)) resultRecords.append(r);
        break;
    }

    case Selector::GetMessageByResourceCode:
    // Запрос сообщения (или файла сообщения?) из ресурсов по коду ресурса:
    // todo
    {
        DBRecord r;
        if(selectById(DBTABLENAME_MESSAGES, param, r)) resultRecords.append(r);
        break;
    }

    case Selector::GetProductsByGroupCode:
    case Selector::GetProductsByGroupCodeIncludeGroups:
    // Запрос списка товаров по коду группы (исключая / включая группы):
    {
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = "SELECT * FROM " + t->name;
        sql += " WHERE " + t->columnName(ProductDBTable::GroupCode) + "='" + param + "'";
        if (selector == Selector::GetProductsByGroupCode)
           sql += " AND " + t->columnName(ProductDBTable::Type) + "!='" + QString::number(ProductDBTable::ProductType_Group) + "'";
        sql += " ORDER BY ";
        sql += t->columnName(ProductDBTable::Type) + " DESC, ";
        sql += t->columnName(ProductDBTable::Name) + " ASC";
        executeSelectSQL(t, sql, resultRecords);
        break;
    }

    case Selector::GetProductsByFilteredCode:
    //case Selector::GetProductsByFilteredCodeIncludeGroups:
    // Запрос списка товаров по фрагменту кода (исключая / включая группы):
    {
        QString p = param.trimmed();
        if (p.isEmpty())
            break;
        if (!settings.getItemBoolValue(Settings::SettingCode_SearchType))
            if (p.size() < settings.getItemIntValue(Settings::SettingCode_SearchCodeSymbols))
                break;
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = "SELECT * FROM " + t->name;
        sql += " WHERE " + t->columnName(ProductDBTable::Code) + " LIKE '%" + p + "%'";
        /*
        if (selector == Selector::GetProductsByFilteredCode)
           sql += " AND " + t->columnName(ProductDBTable::Type) + "!='" + QString::number(ProductDBTable::ProductType_Group) + "'";
        */
        sql += " AND " + t->columnName(ProductDBTable::Type) + "!='" + QString::number(ProductDBTable::ProductType_Group) + "'";
        sql += " ORDER BY ";
        sql += t->columnName(ProductDBTable::Code) + " ASC";
        executeSelectSQL(t, sql, resultRecords);
        break;
    }

    case Selector::GetProductsByFilteredBarcode:
    //case Selector::GetProductsByFilteredBarcodeIncludeGroups:
    // Запрос списка товаров по фрагменту штрих-кода (исключая / включая группы):
    {
        QString p = param.trimmed();
        if (p.isEmpty())
            break;
        if (!settings.getItemBoolValue(Settings::SettingCode_SearchType))
            if (p.size() < settings.getItemIntValue(Settings::SettingCode_SearchBarcodeSymbols))
                break;
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = "SELECT * FROM " + t->name;
        sql += " WHERE " + t->columnName(ProductDBTable::Barcode) + " LIKE '%" + p + "%'";
        /*
        if (selector == Selector::GetProductsByFilteredBarcode)
            sql += " AND " + t->columnName(ProductDBTable::Type) + "!='" + QString::number(ProductDBTable::ProductType_Group) + "'";
        */
        sql += " AND " + t->columnName(ProductDBTable::Type) + "!='" + QString::number(ProductDBTable::ProductType_Group) + "'";
        sql += " ORDER BY ";
        sql += t->columnName(ProductDBTable::Barcode) + " ASC";
        executeSelectSQL(t, sql, resultRecords);
        break;
    }

    case Selector::RefreshCurrentProduct:
    {
        DBRecord r;
        if(selectById(DBTABLENAME_PRODUCTS, param, r)) resultRecords.append(r);
        break;
    }

    default: break;
    }
    qDebug() << "@@@@@ DataBase::selectByParam: records =" << resultRecords.count();
    emit requestResult(selector, resultRecords, true);
}

void DataBase::onDownloadFinished()
{
    qDebug() << "@@@@@ DataBase::onDownloadFinished";
    close(tempDB);
    close(productDB);
    copyDBFiles(DB_TEMP_NAME, DB_PRODUCT_NAME);
    open(productDB, DB_PRODUCT_NAME);
}

void DataBase::onDeleteFinished()
{
    qDebug() << "@@@@@ DataBase::onDeleteFinished";
    close(tempDB);
    close(productDB);
    copyDBFiles(DB_PRODUCT_NAME, DB_TEMP_NAME);
    open(productDB, DB_PRODUCT_NAME);
}

void DataBase::onSelectByList(const DataBase::Selector selector, const DBRecordList& param)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос

    qDebug() << "@@@@@ DataBase::onSelectByList: selector =" << selector << param.count();
    DBRecordList resultRecords;
    switch(selector)
    {
    case Selector::GetShowcaseResources:
    // Запрос списка картинок из ресурсов для списка товаров:
    {
        for (int i = 0; i < param.count(); i++)
        {
            QString imageCode = param[i][ProductDBTable::PictureCode].toString();
            DBRecord r;
            selectById(DBTABLENAME_PICTURES, imageCode, r);
            resultRecords.append(r);
        }
        break;
    }

    default: break;
    }
    emit requestResult(selector, resultRecords, true);
}

void DataBase::updateRecord(const DataBase::Selector selector, const DBRecord& record)
{
    qDebug() << "@@@@@ DataBase::updateRecord: selector =" << selector;
    bool result = false;
    switch(selector)
    {
    case Selector::ReplaceSettingsItem:
    {
        result = insertRecord(getTable(DBTABLENAME_SETTINGS), record);
        break;
    }
    default: break;
    }
    emit requestResult(selector, DBRecordList(), result);
}

void DataBase::saveTransaction(const DBRecord& t)
{
    qDebug() << "@@@@@ DataBase::saveTransaction";
    insertRecord(getTable(DBTABLENAME_TRANSACTIONS), t);
}

QString DataBase::deleteRecords(const QString& tableName, const QString& codeList)
{
    // Удаление из таблицы по списку кодов

    qDebug() << "@@@@@ DataBase::deleteRecords" << tableName << codeList;
    saveLog(LogType_Error, LogSource_DB, QString(" Удаление. Таблица: %1").arg(tableName));
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    int logging = settings.getItemIntValue(Settings::SettingCode_Logging);
    bool detailedLog = logging >= LogType_Info;
    DBTable* t = getTable(tableName);
    QStringList codes = codeList.split(','); // Коды товаров через запятую

    if(t == nullptr)
    {
        errorCount = 1;
        errorCode = LogError_UnknownTable;
        description = "Неизвестная таблица";
    }
    else if (codes.isEmpty() || codes[0].isEmpty()) // Delete all records
    {
        if(removeAll(t) && detailedLog)
        {
            QString s = QString("Удалены все записи таблицы %1").arg(tableName);
            saveLog(LogType_Warning, LogSource_DB, s);
        }
    }
    else
    {
        for (int i = 0; i < codes.count(); i++)
        {
            if(removeRecord(t, codes[i]))
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
    qDebug() << "@@@@@ DataBase::deleteRecords: result" << resultJson;
    emit deleteFinished();
    return resultJson;
}

QString DataBase::uploadRecords(const QString& tableName, const QString& codeList)
{
    // Выгрузка из таблицы по списку кодов

    qDebug() << "@@@@@ DataBase::uploadRecords" << tableName << codeList;
    saveLog(LogType_Error, LogSource_DB, QString("Выгрузка. Таблица: %1").arg(tableName));
    int recordCount = 0;
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    bool detailedLog = settings.getItemIntValue(Settings::SettingCode_Logging) >= LogType_Info;
    DBRecordList records;
    DBTable* t = getTable(tableName);
    QStringList codes = codeList.split(','); // Коды товаров через запятую

    if(t == nullptr)
    {
        errorCount = 1;
        errorCode = LogError_UnknownTable;
        description = "Неизвестная таблица";
    }
    else if (codes.isEmpty()) // Upload all records
    {
        selectAll(t, records);
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
            if(selectById(t, codes[i], r))
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
    qDebug() << "@@@@@ DataBase::uploadRecords: result" << resultJson;
    return resultJson;
}

void DataBase::downloadRecords(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount)
{
    qDebug() << "@@@@@ DataBase::downloadRecords";
    if(!open(tempDB, DB_TEMP_NAME)) return;
    bool detailedLog = settings.getItemIntValue(Settings::SettingCode_Logging) >= LogType_Info;
    QList tables = records.keys();
    for (DBTable* table : tables)
    {
        DBRecordList tableRecords = records.value(table);
        for(DBRecord& r : tableRecords)
        {
#if defined(CONCURRENT_SERVER) & defined(DOWNLOAD_PAUSE)
            if(DOWNLOAD_PAUSE > 0) Tools::pause(DOWNLOAD_PAUSE); // debug concurrent downloading
#endif
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
            qDebug() << "@@@@@ DataBase::downloadRecords " << s;
        }
    }
    emit downloadFinished();
}

