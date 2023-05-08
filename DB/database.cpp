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

DataBase::DataBase(const QString& fileName, Settings& globalSettings, QObject *parent):
    QObject(parent), settings(globalSettings)
{
    qDebug() << "@@@@@ DataBase::DataBase " << fileName;
    filePath = Tools::getDataFilePath(fileName);
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

bool DataBase::start()
{
    qDebug() << "@@@@@ DataBase::onStart";
#ifdef DB_EMULATION
    QFile(filePath).remove();
#endif

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
        emit showMessageBox("ВНИМАНИЕ!", "Ошибка при открытии базы данных", true);
    }

    if (opened && ok)
    {
        JSONParser parser;
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_settings.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_products.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_users.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_showcase.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_pictures.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_messages.txt"));
        emit started();
    }
    return ok && opened;
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

DBTable *DataBase::getTableByName(const QString &name, const bool log)
{
    //qDebug() << "@@@@@ DataBase::getTableByName: name =" << name;
    for (int i = 0; i < tables.size(); i++)
        if (tables[i]->name == name)
            return tables[i];
    qDebug() << "@@@@@ DataBase::getTableByName ERROR";
    saveLog(LogDBTable::LogType_Error, "БД. Не найдена таблица " + name, log);
    return nullptr;
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
    return executeSQL(sql, false);
}

bool DataBase::executeSQL(const QString& sql, const bool log)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::executeSQL: sql =" << sql;

    QSqlQuery q;
    if (!q.exec(sql))
    {
        qDebug() << "DataBase::executeSQL @@@@@ ERROR: " << q.lastError().text();
        saveLog(LogDBTable::LogType_Error, "БД. Не выполнен запрос " + sql, log);
        return false;
    }
    return true;
}

bool DataBase::executeSelectSQL(DBTable* table, const QString& sql, DBRecordList& resultRecords, const bool log)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::executeSelectSQL: sql =" << sql;

    resultRecords.clear();
    QSqlQuery q;
    if (!q.exec(sql))
    {
        qDebug() << "@@@@@ DataBase::executeSelectSQL ERROR: " << q.lastError().text();
        saveLog(LogDBTable::LogType_Error, "БД. Не выполнен запрос " + sql, log);
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

bool DataBase::selectById(DBTable* table, const QString& id, DBRecord& resultRecord, const bool log)
{
    qDebug() << "@@@@@ DataBase::selectById " << id;
    resultRecord.clear();
    QString sql = QString("SELECT * FROM %1 WHERE %2 = '%3'").arg(table->name, table->columnName(0), id);
    DBRecordList records;
    if(!executeSelectSQL(table, sql, records, log)) return false;
    if(records.count() > 0) resultRecord.append(records.at(0));
    return true;
}

void DataBase::selectAll(DBTable *table, DBRecordList& resultRecords, const bool log)
{
    qDebug() << "@@@@@ DataBase::selectAll ";
    DBRecordList records;
    QString sql = QString("SELECT * FROM %1").arg(table->name);
    executeSelectSQL(table, sql, records, log);
    resultRecords.clear();
    resultRecords.append(table->checkList(records));
}

bool DataBase::removeAll(DBTable* table, const bool log)
{
    qDebug() << "@@@@@ DataBase::removeAll: table =" << table->name;
    QString sql = QString("DELETE FROM %1").arg(table->name);
    return executeSQL(sql, log);
}

bool DataBase::removeRecord(DBTable* table, const DBRecord& record, const bool log)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::removeRecord: table name =" << table->name;
    QString sql = QString("DELETE FROM %1 WHERE %2 = '%3'").
            arg(table->name, table->columnName(0), record[0].toString());
    return executeSQL(sql, log);
}

bool DataBase::insertRecord(DBTable *table, const DBRecord& record, const bool log)
{
    if (!opened) return false;
    qDebug() << "@@@@@ DataBase::insertRecord: table name =" << table->name;
    DBRecord checkedRecord = table->checkRecord(record);
    if (checkedRecord.isEmpty())
    {
        qDebug() << "@@@@@ DataBase::insertRecord ERROR (check record)";
        saveLog(LogDBTable::LogType_Error, "БД. Не сохранена запись в таблице " + table->name, log);
        return false;
    }

    QString sql;
    DBRecord r;
    if (selectById(table, checkedRecord.at(0).toString(), r, log))
    {
        removeRecord(table, r, log);
    }
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
    return executeSQL(sql, log);
}

int DataBase::insertRecords(DBTable* table, const DBRecordList& records, const bool log)
{
    qDebug() << "@@@@@ DataBase::insertRecords: record count =" << records.count();
    int result = 0;
    for (int j = 0; j < records.count(); j++)
        if(insertRecord(table, records[j], log))
            result++;
    qDebug() << "@@@@@ DataBase::insertRecords: result =" << result;
    return result;
}

void DataBase::saveLog(const int type, const QString &comment, const bool reallySave)
{
#ifdef SAVE_LOG_IN_DB
    if (!opened || !reallySave) return;
    qDebug() << "@@@@@ DataBase::saveLog: comment =" << comment;
    DBTable* t = getTableByName(DBTABLENAME_LOG, false);
    if (t != nullptr)
    {
        DBRecord r = t->createRecord();
        r[LogDBTable::DateTime] = Tools::currentDateTimeToInt();
        r[LogDBTable::Type] = type;
        r[LogDBTable::Comment] = comment;
        if (insertRecord(t, r, false))
        {
            qint64 logDuration = settings.getItemIntValue(SettingDBTable::SettingCode_LogDuration);
            if(logDuration > 0)
            {
                qint64 first = Tools::currentDateTimeToInt() - logDuration * 24 * 60 * 60 * 1000;
                QString sql = QString("DELETE FROM %1 WHERE %2 < '%3'").
                    arg(t->name, t->columnName(LogDBTable::DateTime), QString::number(first));
                QSqlQuery q;
                q.exec(sql);
            }
        }
    }
#endif
}

void DataBase::onSelect(const DataBase::Selector selector, const QString& param)
{
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
        selectById(getTableByName(DBTABLENAME_PICTURES), param, r);
        resultRecords.append(r);
        break;
    }

    case Selector::GetMessageByResourceCode:
    // Запрос сообщения (или файла сообщения?) из ресурсов по коду ресурса:
    // todo
    {
        DBRecord r;
        selectById(getTableByName(DBTABLENAME_MESSAGES), param, r);
        resultRecords.append(r);
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
        if (settings.getItemIntValue(SettingDBTable::SettingCode_SearchType) != SettingDBTable::SearchType_Dynamic)
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
        if (settings.getItemIntValue(SettingDBTable::SettingCode_SearchType) != SettingDBTable::SearchType_Dynamic)
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

    case Selector::GetCurrentProduct:
    {
        DBRecord r;
        selectById(getTableByName(DBTABLENAME_PRODUCTS), param, r);
        resultRecords.append(r);
        break;
    }

    default: break;
    }
    qDebug() << "@@@@@ DataBase::onSelect: records =" << resultRecords.count();
    emit requestResult(selector, resultRecords, true);
}

void DataBase::onSelectItems(const DataBase::Selector selector, const QString& tableName, const QString& param)
{
    qDebug() << "@@@@@ DataBase::onSelectItems: selector =" << selector;
    DBRecordList resultRecords;
    switch(selector)
    {
    case Selector::GetItemsByCodes:
    {
        DBTable* t = getTableByName(tableName);
        if (t != nullptr)
        {
            QStringList codes = param.split(QLatin1Char(',')); // Коды товаров через запятую
            if (codes.count() > 0)
            {
                QString codeColumnName = t->columnName(0);
                QString sql = "SELECT * FROM " + t->name + " WHERE (";
                for (int i = 0; i < codes.count(); i++)
                {
                    if (i > 0) sql += " OR ";
                    sql += codeColumnName + "='" + codes[i] + "'";
                }
                sql += ")";
                executeSelectSQL(t, sql, resultRecords);
            }
            else
            {
                selectAll(t, resultRecords);
            }
        }
        break;
    }

    default: break;
    }
    emit requestResult(selector, resultRecords, true);
}

void DataBase::onSelectByList(const DataBase::Selector selector, const DBRecordList& param)
{
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
            selectById(getTableByName(DBTABLENAME_PICTURES), imageCode, r);
            resultRecords.append(r);
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
    emit requestResult(selector, *new DBRecordList(), result);
}

void DataBase::onPrinted(const DBRecord& transaction)
{
    qDebug() << "@@@@@ DataBase::onPrinted";
    // Транзакция:
    insertRecord(getTableByName(DBTABLENAME_TRANSACTIONS), transaction);
    saveLog(LogDBTable::LogType_Transaction, transaction.at(TransactionDBTable::DateTime).toString());
}

void DataBase::onDownload(const QString& json)
{
    qDebug() << "@@@@@ DataBase::onDownload: json =" << json;
    JSONParser parser;
    int n = parser.parseAllTables(this, json);
    saveLog(LogDBTable::LogType_Info, QString("Загружено записей: %1").arg(n));
    emit downloadFinished(n);
}



