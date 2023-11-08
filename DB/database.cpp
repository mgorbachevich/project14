#include <QSqlError>
#include "database.h"
#include "showcasedbtable.h"
#include "productdbtable.h"
#include "labelformatdbtable.h"
#include "settingdbtable.h"
#include "settinggroupdbtable.h"
#include "transactiondbtable.h"
#include "resourcedbtable.h"
#include "userdbtable.h"
#include "logdbtable.h"
#include "tools.h"
#include "constants.h"
#include "jsonparser.h"
#include "requestparser.h"


DataBase::DataBase(const QString& fileName, Settings& globalSettings, QObject *parent):
    QObject(parent), settings(globalSettings)
{
    filePath = Tools::dataFilePath(fileName);
    qDebug() << "@@@@@ DataBase::DataBase " << filePath;
    tables.append(new ShowcaseDBTable(DBTABLENAME_SHOWCASE, this));
    tables.append(new ProductDBTable(DBTABLENAME_PRODUCTS, this));
    tables.append(new LabelFormatDBTable(DBTABLENAME_LABELFORMATS, this));
    tables.append(new SettingDBTable(DBTABLENAME_SETTINGS, this));
    tables.append(new SettingGroupDBTable(DBTABLENAME_SETTINGGROUPS, this));
    tables.append(new TransactionDBTable(DBTABLENAME_TRANSACTIONS, this));
    tables.append(new UserDBTable(DBTABLENAME_USERS, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGEFILES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_PICTURES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MOVIES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_SOUNDS, this));
    tables.append(new LogDBTable(DBTABLENAME_LOG, this));
}

void DataBase::onStart()
{
    qDebug() << "@@@@@ DataBase::start";
    if(CLEAR_DB_ON_START) QFile(filePath).remove();

    bool ok = true;
    if (QFile(filePath).exists())
        ok = opened = this->open();
    else
    {
        ok = opened = this->open();
        for (int i = 0; ok && i < tables.size(); i++)
            ok &= createTable(tables[i]);
    }
    if (!opened || !ok)
    {
        qDebug() << "@@@@@ DataBase::onStart ERROR";
        //emit showMessageBox("ВНИМАНИЕ!", "Ошибка при открытии базы данных", true);
    }

    if (opened && ok)
    {
#ifdef NET_SERVER_DEMO
        JSONParser::parseAndSaveAllTables(this, Tools::readTextFile(":/Text/json_pictures.txt"));
        JSONParser::parseAndSaveAllTables(this, Tools::readTextFile(":/Text/json_settings.txt"));
        JSONParser::parseAndSaveAllTables(this, Tools::readTextFile(":/Text/json_products.txt"));
        JSONParser::parseAndSaveAllTables(this, Tools::readTextFile(":/Text/json_users.txt"));
        JSONParser::parseAndSaveAllTables(this, Tools::readTextFile(":/Text/json_showcase.txt"));
        JSONParser::parseAndSaveAllTables(this, Tools::readTextFile(":/Text/json_messages.txt"));
#endif
        emit started();
    }
    //return ok && opened;
}

bool DataBase::open()
{
    qDebug() << "@@@@@ DataBase::open: fileName =" << filePath;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName(DB_HOSTNAME);
    db.setDatabaseName(filePath);
    if(!db.open())
    {
        qDebug() << "@@@@@ DataBase::open ERROR";
        return false;
    }
    return true;
}

DBTable *DataBase::getTableByName(const QString &name) const
{
    //qDebug() << "@@@@@ DataBase::getTableByName: name =" << name;
    for (int i = 0; i < tables.size(); i++)
        if (tables[i]->name == name)
            return tables[i];
    qDebug() << "@@@@@ DataBase::getTableByName ERROR";
    // saveLog(LogDBTable::LogType_Error, "БД. Не найдена таблица " + name);
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
    return executeSQL(sql);
}

bool DataBase::executeSQL(const QString& sql)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::executeSQL: sql =" << sql;

    QSqlQuery q;
    if (!q.exec(sql))
    {
        qDebug() << "DataBase::executeSQL @@@@@ ERROR: " << q.lastError().text();
        //saveLog(LogDBTable::LogType_Error, "БД. Не выполнен запрос " + sql);
        return false;
    }
    return true;
}
/*
int DataBase::getRecordCount(DBTable *table)
{
    if (!opened || table == nullptr) return false;
    qDebug() << "@@@@@ DataBase::getRecordCount: tableName =" << table->name;
    QSqlQuery q;
    QString sql = QString("SELECT COUNT(*) FROM %1").arg(table->name);
    if (!q.exec(sql))
    {
        qDebug() << "@@@@@ DataBase::getRecordCount ERROR: " << q.lastError().text();
        //saveLog(LogDBTable::LogType_Error, "БД. Не выполнен запрос " + sql, log);
        return 0;
    }
    q.first();
    int result = q.value(0).toInt();
    qDebug() << "@@@@@ DataBase::getRecordCount =" << result;
    return result;
}
*/

bool DataBase::executeSelectSQL(DBTable* table, const QString& sql, DBRecordList& resultRecords)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::executeSelectSQL: sql =" << sql;

    resultRecords.clear();
    QSqlQuery q;
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
    return selectById(getTableByName(tableName), id, resultRecord);
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
        resultRecords.append(table->checkList(records));
    }
}

QString DataBase::version()
{
    return DB_VERSION;
}

bool DataBase::removeAll(DBTable* table)
{
    qDebug() << "@@@@@ DataBase::removeAll: table =" << table->name;
    QString sql = QString("DELETE FROM %1").arg(table->name);
    bool ok = executeSQL(sql);
    return ok;
}

bool DataBase::removeRecord(DBTable* table, const QString& code)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::removeRecord: table name =" << table->name;
    QString sql = QString("DELETE FROM %1 WHERE %2 = '%3'").
            arg(table->name, table->columnName(0), code);
    return executeSQL(sql);
}

bool DataBase::insertRecord(DBTable *table, const DBRecord& record)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::insertRecord: table name =" << table->name;
    DBRecord checkedRecord = table->checkRecord(record);
    if (checkedRecord.isEmpty())
    {
        qDebug() << "@@@@@ DataBase::insertRecord ERROR (check record)";
        //saveLog(LogDBTable::LogType_Error, "БД. Не сохранена запись в таблице " + table->name);
        return false;
    }

    QString sql;
    DBRecord r;
    if (selectById(table, checkedRecord.at(0).toString(), r)) removeRecord(table, r.at(0).toString());
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
    return executeSQL(sql);
}

void DataBase::saveLog(const int type, const int source, const QString &comment)
{
    if (!opened) return;
    int logging = settings.getItemIntValue(SettingDBTable::SettingCode_Logging);
    if (type > 0 && type <= logging)
    {
        qDebug() << "@@@@@ DataBase::saveLog: " << type << source << comment;
        LogDBTable* t = (LogDBTable*)getTableByName(DBTABLENAME_LOG);
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
    DBTable* t = getTableByName(DBTABLENAME_LOG);
    quint64 logDuration = settings.getItemIntValue(SettingDBTable::SettingCode_LogDuration);
    if(t != nullptr && logDuration > 0) // Remove old records
    {
        quint64 first = Tools::currentDateTimeToUInt() - logDuration * 24 * 60 * 60 * 1000;
        QString sql = QString("DELETE FROM %1 WHERE %2 < '%3'").
            arg(t->name, t->columnName(LogDBTable::DateTime), QString::number(first));
        QSqlQuery q;
        q.exec(sql);
    }
}

void DataBase::onSelect(const DataBase::Selector selector, const QString& param)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос

    qDebug() << "@@@@@ DataBase::onSelect: selector =" << selector;
    DBRecordList resultRecords;
    switch(selector)
    {
    case Selector::GetSettings:
    // Запрос списка настроек:
        selectAll(getTableByName(DBTABLENAME_SETTINGS), resultRecords);
        break;

    case Selector::GetUserNames:
    // Запрос списка пользователей для авторизации:
        selectAll(getTableByName(DBTABLENAME_USERS), resultRecords);
        break;

    case Selector::GetLog:
    // Запрос списка записей лога:
        selectAll(getTableByName(DBTABLENAME_LOG), resultRecords);
        break;

    case Selector::GetShowcaseProducts:
    // Запрос списка товаров для отображения на экране Showcase:
    {
        DBTable* productTable = getTableByName(DBTABLENAME_PRODUCTS);
        DBTable* showcaseTable = getTableByName(DBTABLENAME_SHOWCASE);
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
        DBTable* t = getTableByName(DBTABLENAME_USERS);
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
        DBTable* t = getTableByName(DBTABLENAME_PRODUCTS);
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
        if (!settings.getItemBoolValue(SettingDBTable::SettingCode_SearchType))
            if (p.size() < settings.getItemIntValue(SettingDBTable::SettingCode_SearchCodeSymbols))
                break;
        DBTable* t = getTableByName(DBTABLENAME_PRODUCTS);
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
        if (!settings.getItemBoolValue(SettingDBTable::SettingCode_SearchType))
            if (p.size() < settings.getItemIntValue(SettingDBTable::SettingCode_SearchBarcodeSymbols))
                break;
        DBTable* t = getTableByName(DBTABLENAME_PRODUCTS);
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
    qDebug() << "@@@@@ DataBase::onSelect: records =" << resultRecords.count();
    emit requestResult(selector, resultRecords, true);
}

void DataBase::onSelectByList(const DataBase::Selector selector, const DBRecordList& param)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос

    qDebug() << "@@@@@ DataBase::onSelectByList: selector =" << selector;
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
            if(selectById(DBTABLENAME_PICTURES, imageCode, r)) resultRecords.append(r);
        }
        break;
    }

    default: break;
    }
    emit requestResult(selector, resultRecords, true);
}

void DataBase::onUpdateRecord(const DataBase::Selector selector, const DBRecord& record)
{
    qDebug() << "@@@@@ DataBase::onUpdateRecord: selector =" << selector;
    bool result = false;
    switch(selector)
    {
    case Selector::ReplaceSettingsItem:
    {
        result = insertRecord(getTableByName(DBTABLENAME_SETTINGS), record);
        break;
    }
    default: break;
    }
    emit requestResult(selector, DBRecordList(), result);
}

void DataBase::onTransaction(const DBRecord& t)
{
    qDebug() << "@@@@@ DataBase::onTransaction";
    insertRecord(getTableByName(DBTABLENAME_TRANSACTIONS), t);
}

void DataBase::onLog(const int type, const int source, const QString &comment)
{
    saveLog(type, source, comment);
}

QString DataBase::deleteRecords(const QString& tableName, const QString& codeList)
{
    // Удаление из таблицы по списку кодов

    qDebug() << "@@@@@ DataBase::deleteRecords" << tableName << codeList;
    saveLog(LogType_Error, LogSource_DB, QString(" Удаление. Таблица: %1").arg(tableName));

    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    int logging = settings.getItemIntValue(SettingDBTable::SettingCode_Logging);
    bool detailedLog = logging >= LogType_Info;
    DBTable* t = getTableByName(tableName);
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
    emit updateDBFinished("Delete Records");
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
    bool detailedLog = settings.getItemIntValue(SettingDBTable::SettingCode_Logging) >= LogType_Info;
    DBRecordList records;
    DBTable* t = getTableByName(tableName);
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
/*
QString DataBase::downloadRecords(const QString& json, const QString& fileName, const QByteArray& fileData)
{
    // Загрузка в БД

    qDebug() << "@@@@@ DataBase::downloadRecords json length = " << json.length();
    qDebug() << "@@@@@ DataBase::downloadRecords fileName = " << fileName;
    qDebug() << "@@@@@ DataBase::downloadRecords fileData length = " << fileData.length();

    //bool multiPartRequest = !fileName.isEmpty() && !fileData.isEmpty();
    bool singlePartRequest = fileName.isEmpty() && fileData.isEmpty();
    bool multiPartRequest = true;
    JSONParser parser;
    int result = 0;
    QString description;

    if(singlePartRequest)
    {
        qDebug() << "@@@@@ DataBase::downloadRecords SinglePart";
        parser.parseAndSaveAllTables(this, json, &result, &description);
    }
    if(multiPartRequest)
    {
        qDebug() << "@@@@@ DataBase::downloadRecords MultiPart";
        if(!json.isEmpty()) parser.parseAndSaveAllTables(this, json, &result, &description);
        if(!fileName.isEmpty() && !fileData.isEmpty())
        {
            QString filePath = Tools::appFilePath(DATA_STORAGE_SUBDIR, fileName);
            qDebug() << "@@@@@ DataBase::downloadRecords filePath = " << filePath;
            if(Tools::writeBinaryFile(filePath, fileData)) n++;
            else
            {
                qDebug() << "@@@@@ DataBase::downloadRecords MultiPart Write File ERROR " << fileName;
                if(result == 0) result = -1;
            }
        }
    }
    QString resultJson = RequestParser::makeResultJson(result, description, "", "");
    qDebug() << "@@@@@ DataBase::downloadRecords: result" << resultJson;
    emit updateDBFinished("Download Records");
    return resultJson;
}
*/

void DataBase::downloadTableRecords(DBTable* table, DBRecordList& records, int& successCount, int& errorCount)
{
    qDebug() << "@@@@@ DataBase::downloadTableRecords";
    bool detailedLog = settings.getItemIntValue(SettingDBTable::SettingCode_Logging) >= LogType_Info;
    for(int i = 0; i < records.count(); i++)
    {
        DBRecord& r = records[i];
        QString code = r.count() > 0 ? r.at(0).toString() : "";
        QString s;
        if(code.isEmpty() || !insertRecord(table, r))
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
        qDebug() << "@@@@@ DataBase::downloadTableRecords " << s;
    }
}

void DataBase::downloadRecords(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount)
{
    qDebug() << "@@@@@ DataBase::downloadRecords";
    QList tables = records.keys();
    for (DBTable* table : tables)
    {
        DBRecordList tableRecords = records.value(table);
        downloadTableRecords(table, tableRecords, successCount, errorCount);
    }
    emit updateDBFinished("Download Records");
}

