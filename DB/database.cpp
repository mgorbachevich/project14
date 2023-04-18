#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "database.h"
#include "tools.h"
#include "showcasedbtable.h"
#include "productdbtable.h"
#include "labelformatdbtable.h"
#include "settingdbtable.h"
#include "settinggroupdbtable.h"
#include "transactiondbtable.h"
#include "resourcedbtable.h"
#include "userdbtable.h"
#include "logdbtable.h"
#include "jsonparser.h"

DataBase::DataBase(Settings& newSettings, QObject *parent): QObject(parent), settings(newSettings)
{
    qDebug() << "@@@@@ DataBase::DataBase";

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

DataBase::~DataBase()
{
    db.close();
}

DBTable *DataBase::getTableByName(const QString &name)
{
    qDebug() << "@@@@@ DataBase::getTableByName " << name;
    for (int i = 0; i < tables.size(); i++)
        if (tables[i]->name == name)
            return tables[i];
    qDebug() << "@@@@@ DataBase::getTableByName ERROR";
    emit log(LogDBTable::LogType_Error, "БД. Не найдена таблица " + name);
    return nullptr;
}

bool DataBase::open()
{
    QString dbFileName = Tools::getDataFilePath(DB_FILENAME);
    qDebug() <<  "@@@@@ DataBase::open " << dbFileName;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName(DB_HOSTNAME);
    db.setDatabaseName(dbFileName);
    if(!db.open())
    {
        qDebug() << "@@@@@ DataBase::open ERROR";
        emit log(LogDBTable::LogType_Error, "БД. Не открыт файл " + dbFileName);
        return false;
    }
    return true;
}

void DataBase::onStart()
{
    qDebug() << "@@@@@ DataBase::onStart";

#ifdef DB_EMULATION
    QFile(Tools::getDataFilePath(DB_FILENAME)).remove();
#endif

    if (QFile(Tools::getDataFilePath(DB_FILENAME)).exists())
        started = this->open();
    else
    {
        started = this->open();
        for (int i = 0; i < tables.size(); i++)
            started &= createTable(tables[i]);

        // todo:
        JSONParser parser;
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_settings.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_products.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_users.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_showcase.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_pictures.txt"));
        parser.parseAllTables(this, Tools::readTextFile(":/Text/json_messages.txt"));

        emit dbStarted();
    }
    if (!started)
    {
        qDebug() << "@@@@@ DataBase::onStart ERROR";
        emit log(LogDBTable::LogType_Error, "БД не запущена");
    }
}

bool DataBase::createTable(DBTable* table)
{
    if (!started) return false;
    qDebug() << "@@@@@ DataBase::createTable " << table->name;

    QString sql =  "CREATE TABLE " + table->name + " (";
    for (int i = 0; i < table->columnCount(); i++)
    {
        sql += table->columnName(i) + " " + table->columnType(i);
        sql += (i == table->columnCount() - 1) ? ")" : ", ";
    }
    return executeSQL(sql);
}

bool DataBase::removeRecord(DBTable* table, const DBRecord& record)
{
    if (!started) return false;
    qDebug() << "@@@@@ DataBase::removeRecord from " << table->name;
    QString sql = QString("DELETE FROM %1 WHERE %2 = '%3'").
            arg(table->name, table->columnName(0), record[0].toString());
    return executeSQL(sql);
}
/*
bool DataBase::insertRecord(DBTable* table, const DBRecord& record)
{
    if (!started) return false;
    qDebug() << "@@@@@ DataBase::insertRecord into " << table->name;

    if (table->checkRecord(record) == nullptr)
    {
        qDebug() << "@@@@@ DataBase::insertRecord ERROR (check record)";
        return false;
    }

    QString sql =  "INSERT INTO " + table->name + " (";
    for (int i = 0; i < table->columnCount(); i++)
    {
        sql += table->columnName(i);
        sql += (i == table->columnCount() - 1) ? ")" : ", ";
    }
    sql +=  " VALUES (";
    for (int i = 0; i < table->columnCount(); i++)
    {
        sql += (i < record.count()) ? "'" + record[i].toString() + "'" : "''";
        sql += (i == table->columnCount() - 1) ? ")" : ", ";
    }
    return executeSQL(sql);
}

bool DataBase::updateRecord(DBTable *table, const DBRecord& record)
{
    if (!started) return false;
    qDebug() << "@@@@@ DataBase::updateRecord in " << table->name;

    QString sql =  "UPDATE " + table->name + " SET ";
    for (int i = 1; i < table->columnCount(); i++)
    {
        sql += table->columnName(i) + " = ";
        sql +=  "'" + record[i].toString() + "'";
        sql += (i == table->columnCount() - 1) ? " " : ", ";
    }
    sql +=  " WHERE " + table->columnName(0) + " = '" + record[0].toString() + "'";
    return executeSQL(sql);
}
*/

bool DataBase::updateOrInsertRecord(DBTable *table, const DBRecord& record, const bool forceInsert)
{
    if (!started) return false;
    qDebug() << "@@@@@ DataBase::updateOrInsertRecord in " << table->name;
    if (table->checkRecord(record) == nullptr)
    {
        qDebug() << "@@@@@ DataBase::updateOrInsertRecord ERROR (check record)";
        emit log(LogDBTable::LogType_Error, "БД. Не сохранена запись в таблице " + table->name);
        return false;
    }

    QString sql;
    DBRecord r;
    if (!forceInsert && selectById(table, record.at(0).toString(), r))
    {
        sql = "UPDATE " + table->name + " SET ";
        for (int i = 1; i < table->columnCount(); i++)
        {
            sql += table->columnName(i) + " = ";
            sql +=  "'" + record[i].toString() + "'";
            sql += (i == table->columnCount() - 1) ? " " : ", ";
        }
        sql +=  " WHERE " + table->columnName(0) + " = '" + record[0].toString() + "'";
    }
    else
    {
        sql = "INSERT INTO " + table->name + " (";
        for (int i = 0; i < table->columnCount(); i++)
        {
            sql += table->columnName(i);
            sql += (i == table->columnCount() - 1) ? ")" : ", ";
        }
        sql +=  " VALUES (";
        for (int i = 0; i < table->columnCount(); i++)
        {
            sql += (i < record.count()) ? "'" + record[i].toString() + "'" : "''";
            sql += (i == table->columnCount() - 1) ? ")" : ", ";
        }
    }
    return executeSQL(sql);
}

/*
int DataBase::getMax(DBTable *table, const QString& field)
{
    QString sql = QString("SELECT * FROM %1 WHERE %2 = (SELECT MAX(%3) FROM %4) ORDER BY %5 DESC").
            arg(table->name, field, field, table->name, field);
    qDebug() << "@@@@@ DataBase::getMax " << sql;
    QSqlQuery q;
    if (!q.exec(sql))
    {
        qDebug() << "@@@@@ DataBase::getMax ERROR: " << q.lastError().text();
        return 0;
    }
    if (!q.first())
        return 0;
    return q.value(0).toInt();
}
*/

bool DataBase::executeSQL(const QString& sql)
{
    if (!started) return false;
    qDebug() << "@@@@@ DataBase::executeSQL " << sql;

    QSqlQuery q;
    if (!q.exec(sql))
    {
        qDebug() << "DataBase::executeSQL @@@@@ ERROR: " << q.lastError().text();
        emit log(LogDBTable::LogType_Error, "БД. Не выполнен запрос " + sql);
        return false;
    }
    return true;
}

bool DataBase::executeSelectSQL(DBTable* table, const QString& sql, DBRecordList& records)
{
    if (!started) return false;
    qDebug() << "@@@@@ DataBase::executeSelectSQL " << sql;

    records.clear();
    QSqlQuery q;
    if (!q.exec(sql))
    {
        qDebug() << "@@@@@ DataBase::executeSelectSQL ERROR: " << q.lastError().text();
        emit log(LogDBTable::LogType_Error, "БД. Не выполнен запрос " + sql);
        return false;
    }
    while (q.next())
    {
        DBRecord r;
        for(int i = 0; i < table->columnCount(); i++)
            r << q.value(i);
        records.append(r);
    }
    qDebug() << "@@@@@ DataBase::executeSelectSQL records: " << records.count();
#ifdef DEBUG_LOG_SELECT_RESULT
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord ri = records.[i];
        QString s = "";
        for (int j = 0; j < ri.count(); j++)
             s += " " + ri.field(j).toString();
        qDebug() << "@@@@@ fields:" << s;
    }
#endif
    return records.count() > 0;
}

bool DataBase::selectById(DBTable* table, const QString& id, DBRecord& record)
{
    record.clear();
    QString sql = QString("SELECT * FROM %1 WHERE %2 = '%3'").arg(table->name, table->columnName(0), id);
    DBRecordList records;
    if(!executeSelectSQL(table, sql, records))
        return false;
    record = records.at(0);
    return true;
}

void DataBase::selectAll(DBTable* table, DBRecordList& records)
{
    QString sql = QString("SELECT * FROM %1").arg(table->name);
    executeSelectSQL(table, sql, records);
}

void DataBase::selectAndCheckAll(DBTable *table, DBRecordList& resultRecords)
{
    qDebug() << "@@@@@ DataBase::selectAndCheckAll ";

    DBRecordList records;
    selectAll(table, records);
    resultRecords.clear();
    resultRecords.append(table->checkAll(records));
}

void DataBase::onSelect(const DataBase::Selector selector, const QString& param)
{
    qDebug() << "@@@@@ DataBase::onSelect " << selector;

    DBRecordList resultRecords;
    switch(selector)
    {
    case Selector::GetSettings:
    // Запрос списка настроек:
        selectAndCheckAll(getTableByName(DBTABLENAME_SETTINGS), resultRecords);
        break;

    case Selector::GetUserNames:
    // Запрос списка пользователей для авторизации:
        selectAndCheckAll(getTableByName(DBTABLENAME_USERS), resultRecords);
        break;

    case Selector::GetLog:
    // Запрос списка записей лога:
        selectAndCheckAll(getTableByName(DBTABLENAME_LOG), resultRecords);
        break;

    case Selector::GetShowcaseProducts:
    // Запрос списка товаров для отображения на экране Showcase:
    {
        DBTable* productTable = getTableByName(DBTABLENAME_PRODUCTS);
        DBTable* showcaseTable = getTableByName(DBTABLENAME_SHOWCASE);
        DBRecordList showcaseRecords;
        selectAndCheckAll(showcaseTable, showcaseRecords);
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
    // Запрос списка товаров по коду группы (исвключая / включая группы):
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

    default: break;
    }
    emit selectResult(selector, resultRecords);
}

void DataBase::onSelectByList(const DataBase::Selector selector, const DBRecordList& param)
{
    qDebug() << "@@@@@ DataBase::onSelectByList " << selector;

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
    emit selectResult(selector, resultRecords);
}

void DataBase::onUpdateRecord(const DataBase::Selector selector, const DBRecord& record)
{
    qDebug() << "@@@@@ DataBase::onUpdateRecord " << selector;
    bool result = false;
    switch(selector)
    {
    case DataBase::ReplaceSettingsItem:
    {
        result = updateOrInsertRecord(getTableByName(DBTABLENAME_SETTINGS), record);
        break;
    }
    default: break;
    }
    emit updateResult(selector, result);
}

void DataBase::onNewData(const QString& json)
{
    qDebug() << "@@@@@ DataBase::onNewData " << json;
    JSONParser parser;
    if(!parser.parseAllTables(this, json))
        emit log(LogDBTable::LogType_Warning, "БД. Не обработаны данные " + json);
}

void DataBase::onPrinted(const DBRecord& transaction)
{
    qDebug() << "@@@@@ DataBase::onPrinted";
    // Транзакция:
    insertRecord(getTableByName(DBTABLENAME_TRANSACTIONS), transaction);
    emit log(LogDBTable::LogType_Transaction, transaction.at(TransactionDBTable::DateTime).toString());
}

void DataBase::onSaveLog(const DBRecord& record)
{
    if (!started) return;
    qDebug() << "@@@@@ DataBase::onSaveLog";
    DBTable* t = getTableByName(DBTABLENAME_LOG);
    insertRecord(t, record);

    qint64 logDuration = settings.getItemIntValue(SettingDBTable::SettingCode_LogDuration);
    if(logDuration > 0)
    {
        qint64 first = Tools::currentDateTimeToInt() - logDuration * 24 * 60 * 60 * 1000;
        QString sql = QString("DELETE FROM %1 WHERE %2 < '%3'").
                arg(t->name, t->columnName(LogDBTable::DateTime), QString::number(first));
        executeSQL(sql);
    }
}

void DataBase::onTableParsed(DBTable* table, const DBRecordList& records)
{
    qDebug() << "@@@@@ DataBase::onTableParsed " << records.count();
    for (int j = 0; j < records.count(); j++) updateOrInsertRecord(table, records[j]);
}



