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
#include "transactiondbtable.h"
#include "resourcedbtable.h"
#include "userdbtable.h"

DataBase::DataBase(QObject *parent): QObject(parent)
{
    qDebug() << "@@@@@ DataBase::DataBase";

    tables.append(new ShowcaseDBTable(DBTABLENAME_SHOWCASE, this));
    tables.append(new ProductDBTable(DBTABLENAME_PRODUCTS, this));
    tables.append(new LabelFormatDBTable(DBTABLENAME_LABELFORMATS, this));
    tables.append(new SettingDBTable(DBTABLENAME_SETTINGS, this));
    tables.append(new TransactionDBTable(DBTABLENAME_TRANSACTIONS, this));
    tables.append(new UserDBTable(DBTABLENAME_USERS, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGEFILES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_PICTURES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MOVIES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_SOUNDS, this));
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
    return nullptr;
}

bool DataBase::open()
{
    QString s = Tools::getDataFilePath(DB_FILENAME);
    qDebug() <<  "@@@@@ DataBase::open " << s;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName(DB_HOSTNAME);
    db.setDatabaseName(s);
    if(!db.open())
    {
        qDebug() << "@@@@@ DataBase::open ERROR";
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
        parser.run(this, Tools::readTextFile(":/Text/json_settings.txt"));
        parser.run(this, Tools::readTextFile(":/Text/json_products.txt"));
        parser.run(this, Tools::readTextFile(":/Text/json_users.txt"));
        parser.run(this, Tools::readTextFile(":/Text/json_showcase.txt"));
        parser.run(this, Tools::readTextFile(":/Text/json_pictures.txt"));
        parser.run(this, Tools::readTextFile(":/Text/json_messages.txt"));

        emit dbStarted();
    }
    if (!started) qDebug() << "@@@@@ DataBase::onStart ERROR";
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

    QString sql = "DELETE FROM " + table->name + " WHERE " + table->columnName(0) + "=" + record[0].toString();
    //QString sql = QString("DELETE FROM %1 WHERE %2=%3").arg(table.name, table.columnName(0), record.field(0).toString());
    return executeSQL(sql);
}

bool DataBase::insertRecord(DBTable* table, const DBRecord& record)
{
    if (!started) return false;
    qDebug() << "@@@@@ DataBase::insertRecord into " << table->name;

    if (!table->checkRecordForInsert(record))
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

bool DataBase::executeSQL(const QString& sql)
{
    if (!started) return false;
    qDebug() << "@@@@@ DataBase::executeSQL " << sql;

    QSqlQuery q;
    if (!q.exec(sql))
    {
        qDebug() << "DataBase::executeSQL @@@@@ ERROR: " << q.lastError().text();
        return false;
     }
    return true;
}

void DataBase::executeSelectSQL(DBTable* table, const QString& sql, DBRecordList& records)
{
    if (!started) return;
    qDebug() << "@@@@@ DataBase::executeSelectSQL " << sql;

    records.clear();
    QSqlQuery q;
    if (!q.exec(sql))
    {
        qDebug() << "@@@@@ DataBase::executeSelectSQL ERROR: " << q.lastError().text();
        return;
    }
    while (q.next())
    {
        DBRecord r;
        for(int i = 0; i < table->columnCount(); i++)
            r << q.value(i);
        records.append(r);
    }
    qDebug() << "@@@@@ DataBase::executeSelectSQL records: " << records.count();
#ifdef LOG_SELECT_RESULT
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord ri = records.[i];
        QString s = "";
        for (int j = 0; j < ri.count(); j++)
             s += " " + ri.field(j).toString();
        qDebug() << "@@@@@ fields:" << s;
    }
#endif
}

bool DataBase::selectById(DBTable* table, const QString& id, DBRecord& record)
{
    record.clear();
    QString sql = "SELECT * FROM " + table->name + " WHERE " + table->columnName(0) + "='" + id + "'";
    DBRecordList rs;
    executeSelectSQL(table, sql, rs);
    if(rs.count() > 0)
    {
        record = rs[0];
        return true;
    }
    return false;
}

void DataBase::selectAll(DBTable* table, DBRecordList& records)
{
    QString sql = "SELECT * FROM " + table->name;
    executeSelectSQL(table, sql, records);
}

void DataBase::onSelect(const DataBase::Selector selector, const QString& param)
{
    qDebug() << "@@@@@ DataBase::onSelect " << selector;

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
            QString showcaseProductCode = showcaseRecords[i][ShowcaseDBTable::Columns::Code].toString();
            if (selectById(productTable, showcaseProductCode, r) && ProductDBTable::isSuitableForShowcase(r))
                resultRecords.append(r);
        }
        break;
    }

    case Selector::GetAuthorizationUserByName:
    // Запрос пользователя по имени для авторизации:
    {
        DBTable* t = getTableByName(DBTABLENAME_USERS);
        QString sql = "SELECT * FROM " + t->name;
        sql += " WHERE " + t->columnName(UserDBTable::Columns::Name) + "='" + param + "'";
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
        sql += " WHERE " + t->columnName(ProductDBTable::Columns::GroupCode) + "='" + param + "'";
        if (selector == Selector::GetProductsByGroupCode)
           sql += " AND " + t->columnName(ProductDBTable::Columns::Type) + "!='" + QString::number(ProductDBTable::ProductType::ProductType_Group) + "'";
        sql += " ORDER BY ";
        sql += t->columnName(ProductDBTable::Columns::Type) + " DESC, ";
        sql += t->columnName(ProductDBTable::Columns::Name) + " ASC";
        executeSelectSQL(t, sql, resultRecords);
        break;
    }

    case Selector::GetProductsByFilteredCode:
    case Selector::GetProductsByFilteredCodeIncludeGroups:
    // Запрос списка товаров по фрагменту кода (исвключая / включая группы):
    {
        DBTable* t = getTableByName(DBTABLENAME_PRODUCTS);
        QString sql = "SELECT * FROM " + t->name;
        sql += " WHERE " + t->columnName(ProductDBTable::Columns::Code) + " LIKE '%" + param + "%'";
        if (selector == Selector::GetProductsByFilteredCode)
           sql += " AND " + t->columnName(ProductDBTable::Columns::Type) + "!='" + QString::number(ProductDBTable::ProductType::ProductType_Group) + "'";
        sql += " ORDER BY ";
        sql += t->columnName(ProductDBTable::Columns::Code) + " ASC";
        executeSelectSQL(t, sql, resultRecords);
        break;
    }

    case Selector::GetProductsByFilteredBarcode:
    case Selector::GetProductsByFilteredBarcodeIncludeGroups:
    // Запрос списка товаров по фрагменту штрих-кода (исвключая / включая группы):
    {
        DBTable* t = getTableByName(DBTABLENAME_PRODUCTS);
        QString sql = "SELECT * FROM " + t->name;
        sql += " WHERE " + t->columnName(ProductDBTable::Columns::Barcode) + " LIKE '%" + param + "%'";
        if (selector == Selector::GetProductsByFilteredBarcode)
            sql += " AND " + t->columnName(ProductDBTable::Columns::Type) + "!='" + QString::number(ProductDBTable::ProductType::ProductType_Group) + "'";
        sql += " ORDER BY ";
        sql += t->columnName(ProductDBTable::Columns::Barcode) + " ASC";
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
            QString imageCode = param[i][ProductDBTable::Columns::PictureCode].toString();
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

void DataBase::onUpdate(const DataBase::Selector selector, const DBRecord& record)
{
    qDebug() << "@@@@@ DataBase::onUpdate " << selector;
    bool result = false;
    switch(selector)
    {
    case DataBase::Selector::ReplaceSettingsItem:
    {
        result = updateRecord(getTableByName(DBTABLENAME_SETTINGS), record);
        break;
    }
    default: break;
    }
    emit updateResult(selector, result);
}

void DataBase::onNewData(const QString& json)
{
    qDebug() << "@@@@@ DataBase::onNewData " << json;
    parser.run(this, json);
}

void DataBase::onTableParsed(DBTable* table, const DBRecordList& records)
{
    qDebug() << "@@@@@ DataBase::onTableParsed " << records.count();

    for (int j = 0; j < records.count(); j++)
        insertRecord(table, records[j]);
}


