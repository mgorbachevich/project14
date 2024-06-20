#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include "database.h"
#include "showcasedbtable.h"
#include "productdbtable.h"
#include "transactiondbtable.h"
#include "resourcedbtable.h"
#include "logdbtable.h"
#include "tools.h"
#include "constants.h"
#include "netserver.h"
#include "appmanager.h"

bool isExecuting = false;

DataBase::DataBase(AppManager *parent) : ExternalMessager(parent)
{
    Tools::debugLog("@@@@@ DataBase::DataBase");
    tables.append(new ProductDBTable(DBTABLENAME_PRODUCTS, this));
    tables.append(new ResourceDBTable(DBTABLENAME_LABELFORMATS, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGEFILES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_PICTURES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MOVIES, this));
    tables.append(new ResourceDBTable(DBTABLENAME_SOUNDS, this));
    tables.append(new ShowcaseDBTable(DBTABLENAME_SHOWCASE, this));
    tables.append(new LogDBTable(DBTABLENAME_LOG, this));
    tables.append(new TransactionDBTable(DBTABLENAME_TRANSACTIONS, this));
}

DataBase::~DataBase()
{
    Tools::debugLog("@@@@@ DataBase::~DataBase");
    close(productDB);
    close(logDB);
}

void DataBase::onAppStart()
{
    Tools::debugLog("@@@@@ DataBase::onAppStart");
    QString message;
    QString path = Tools::dbPath(DB_PRODUCT_NAME);
    if(REMOVE_PRODUCT_DB_ON_START) Tools::removeFile(path);
    bool exists = QFile(path).exists();
    Tools::debugLog(QString("@@@@@ DataBase::onAppStart %1 %2").arg(path, Tools::boolToString(exists)));
    started = addAndOpen(productDB, path);
    if(!exists && started)
    {
        started &= createTable(productDB, getTable(DBTABLENAME_PRODUCTS));
        started &= createTable(productDB, getTable(DBTABLENAME_LABELFORMATS));
        started &= createTable(productDB, getTable(DBTABLENAME_MESSAGES));
        started &= createTable(productDB, getTable(DBTABLENAME_MESSAGEFILES));
        started &= createTable(productDB, getTable(DBTABLENAME_PICTURES));
        started &= createTable(productDB, getTable(DBTABLENAME_MOVIES));
        started &= createTable(productDB, getTable(DBTABLENAME_SOUNDS));
        started &= createTable(productDB, getTable(DBTABLENAME_SHOWCASE));
        if(!started) message += "\nОШИБКА! Не создана БД товаров";
    }
    if (started)
    {
        path = Tools::dbPath(DB_LOG_NAME);
        if(REMOVE_LOG_DB_ON_START) Tools::removeFile(path);
        exists = QFile(path).exists();
        Tools::debugLog(QString("@@@@@ DataBase::onAppStart %1 %2").arg(path, Tools::boolToString(exists)));
        started = addAndOpen(logDB, path);
        if(!exists && started)
        {
            started &= createTable(logDB, getTable(DBTABLENAME_LOG));
            started &= createTable(logDB, getTable(DBTABLENAME_TRANSACTIONS));
            if(!started) message += "\nОШИБКА! Не создана БД лога";
        }
    }
    if (started)
    {
#ifdef DB_EMULATION
        Tools::debugLog("@@@@@ DataBase::onAppStart emulation");
        executeSQL(productDB, "INSERT INTO pictures (code, value, hash, field, source) VALUES ('6', 'pictures/6.png', '', '', '');");
        executeSQL(productDB, "INSERT INTO pictures (code, value, hash, field, source) VALUES ('8', 'pictures/8.png', '', '', '');");
        executeSQL(productDB, "INSERT INTO pictures (code, value, hash, field, source) VALUES ('9', 'pictures/9.png', '', '', '');");
        executeSQL(productDB, "INSERT INTO pictures (code, value, hash, field, source) VALUES ('10', 'pictures/10.png', '', '', '');");
        executeSQL(productDB, "INSERT INTO pictures (code, value, hash, field, source) VALUES ('11', 'pictures/11.png', '', '', '');");
        executeSQL(productDB, "INSERT INTO pictures (code, value, hash, field, source) VALUES ('12', 'pictures/12.png', '', '', '');");
        executeSQL(productDB, "INSERT INTO pictures (code, value, hash, field, source) VALUES ('13', 'pictures/13.png', '', '', '');");
        executeSQL(productDB, "INSERT INTO pictures (code, value, hash, field, source) VALUES ('14', 'pictures/14.png', '', '', '');");
        executeSQL(productDB, "INSERT INTO pictures (code, value, hash, field, source) VALUES ('15', 'pictures/15.png', '', '', '');");
#endif
    }
    else message += "\nОШИБКА! Не открыта база данных";
    if(!message.isEmpty())
        QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this, message]() { showAttention(message); });
    emit dbStarted();
    Tools::debugLog(QString("@@@@@ DataBase::startDB %1").arg(Tools::boolToString(started)));
}

bool DataBase::open(QSqlDatabase& db, const QString& name)
{
    if (!started) return false;
    const QString path = Tools::dbPath(name);
    Tools::debugLog("@@@@@ DataBase::open " + path);
    if(!QFile(path).exists()) return false;
    return db.isOpen() ? true : db.open();
}

bool DataBase::addAndOpen(QSqlDatabase& db, const QString& filePath, const bool open)
{
    Tools::debugLog(QString("@@@@@ DataBase::addAndOpen %1").arg(filePath));
    if(db.isOpen()) return true;
    db = QSqlDatabase::addDatabase("QSQLITE", filePath);
    db.setDatabaseName(filePath);
    //db.setHostName(hostName);
    if(!open || db.open()) return true;
    Tools::debugLog("@@@@@ DataBase::addAndOpen ERROR " + db.lastError().text());
    return false;
}

DBTable* DataBase::getTable(const QString &name) const
{
    for (DBTable* t : tables) if (t->name == name) return t;
    Tools::debugLog(QString("@@@@@ DataBase::getTable ERROR %1").arg(name));
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
    if (!started) return false;
    Tools::debugLog(QString("@@@@@ DataBase::createTable %1").arg(table->name));

    QString sql =  "CREATE TABLE " + table->name + " (";
    for (int i = 0; i < table->columnCount(); i++)
    {
        sql += table->columnName(i) + " " + table->columnType(i);
        sql += (i == table->columnCount() - 1) ? ");" : ", ";
    }
    return executeSQL(db, sql);
}

bool DataBase::query(const QSqlDatabase& db, const QString& sql, DBTable* table, DBRecordList* records)
{
    if (!started) return false;
    Tools::debugLog(QString("@@@@@ DataBase::query %1 %2").arg(Tools::fileNameFromPath(db.databaseName()), sql));
    QString select = sql;
    bool isSelect = select.contains("SELECT");
    if(isSelect)
    {
        if(records == nullptr || table == nullptr)
        {
            Tools::debugLog("@@@@@ DataBase::query ERROR bad param");
            return false;
        }
        records->clear();
    }

    quint64 t = Tools::nowMsec();
    while(isExecuting) // жду пока выполнится предыдущий запрос из дркгого потока
    {
        Tools::debugLog("@@@@@ DataBase::query waiting");
        Tools::pause(SQL_EXECUTION_SLEEP_MSEC);
        if(Tools::nowMsec() - t > SQL_EXECUTION_WAIT_MSEC)
        {
            Tools::debugLog("@@@@@ DataBase::query ERROR timeout");
            return false;
        }
    }
    isExecuting = true;
    QSqlQuery q(db);
    if (!q.exec(sql))
    {
        Tools::debugLog(QString("@@@@@ DataBase::executeSelectSQL ERROR %1").arg(q.lastError().text()));
        isExecuting = false;
        return false;
    }
    if(isSelect)
    {
        while (q.next())
        {
            DBRecord r;
            for(int i = 0; i < table->columnCount(); i++) r << q.value(i);
            records->append(r);
        }
    }
    isExecuting = false;
    return true;
}

bool DataBase::executeSQL(const QSqlDatabase& db, const QString& sql)
{
    return query(db, sql, nullptr, nullptr);
}

bool DataBase::executeSelectSQL(const QSqlDatabase& db, DBTable* table, const QString& sql, DBRecordList& resultRecords)
{
    return query(db, sql, table, &resultRecords) && resultRecords.count() > 0;
}

bool DataBase::selectById(const QSqlDatabase& db, const QString& table, const QString& id, DBRecord& resultRecord)
{
    return selectById(db, getTable(table), id, resultRecord);
}

bool DataBase::selectById(const QSqlDatabase& db, DBTable* table, const QString& id, DBRecord& resultRecord)
{
    Tools::debugLog(QString("@@@@@ DataBase::selectById %1").arg(id));
    resultRecord.clear();
    if(table == nullptr) return false;
    QString sql = QString("SELECT * FROM %1 WHERE %2 = '%3';").arg(table->name, table->columnName(0), id);
    DBRecordList records;
    if(!executeSelectSQL(db, table, sql, records)) return false;
    if(records.count() > 0) resultRecord.append(records.at(0));
    return true;
}

void DataBase::selectAll(const QSqlDatabase& db, DBTable *table, DBRecordList& resultRecords)
{
    Tools::debugLog("@@@@@ DataBase::selectAll");
    resultRecords.clear();
    if(table != nullptr)
    {
        DBRecordList records;
        QString sql = QString("SELECT * FROM %1;").arg(table->name);
        executeSelectSQL(db, table, sql, records);
        resultRecords.append(table->checkList(this, records));
    }
}

QStringList DataBase::selectAllCodes(const QSqlDatabase& db, DBTable *table)
{
    Tools::debugLog("@@@@@ DataBase::selectAllCodes");
    QStringList result;
    if(table != nullptr)
    {
        DBRecordList records;
        QString sql = QString("SELECT %1 FROM %2;").arg(table->columnName(0), table->name);
        executeSelectSQL(db, table, sql, records);
        Tools::sortByInt(records, 0);
        for(int i = 0; i < records.count(); i++) result << records[i][0].toString();
    }
    return result;
}

bool DataBase::removeAll(const QSqlDatabase& db, DBTable* table)
{
    Tools::debugLog(QString("@@@@@ DataBase::removeAll %1").arg(table->name));
    QString sql = QString("DELETE FROM %1;").arg(table->name);
    bool ok = executeSQL(db, sql);
    return ok;
}

bool DataBase::removeRecord(const QSqlDatabase& db, DBTable* table, const QString& code)
{
    if (!started) return false;
    Tools::debugLog(QString("@@@@@ DataBase::removeRecord %1").arg(table->name));
    QString sql = QString("DELETE FROM %1 WHERE %2 = '%3';").arg(table->name, table->columnName(0), code);
    return executeSQL(db, sql);
}

bool DataBase::insertRecord(const QSqlDatabase& sqlDb, DBTable *table, const DBRecord& record)
{
    Tools::debugLog("@@@@@ DataBase::insertRecord");
    if (!started)
    {
        Tools::debugLog("@@@@@ DataBase::insertRecord ERROR opened=false");
        return false;
    }
    if (table == nullptr)
    {
        Tools::debugLog("@@@@@ DataBase::insertRecord ERROR table=nullptr");
        return false;
    }
    DBRecord r = table->checkRecord(record);
    if (r.isEmpty())
    {
        Tools::debugLog("@@@@@ DataBase::insertRecord ERROR checkedRecord.isEmpty()");
        //saveLog(LogDBTable::LogType_Error, "БД. Не сохранена запись в таблице " + table->name);
        return false;
    }

    Tools::debugLog(QString("@@@@@ DataBase::insertRecord %1 %2").arg(table->name, r.at(0).toString()));
    const int n = table->columnCount();
    QString sql = "INSERT OR REPLACE INTO " + table->name + " (";
    for (int i = 0; i < n; i++)
    {
        sql += table->columnName(i);
        sql += (i == n - 1) ? ")" : ", ";
    }
    sql +=  " VALUES (";
    for (int i = 0; i < n; i++)
    {
        sql += (i < r.count()) ? "'" + r[i].toString() + "'" : "''";
        sql += (i == n - 1) ? ");" : ", ";
    }
    Tools::pause(DEBUG_INSERT_DB_DELAY_MSEC); // замедляю загрузку для отладки
    return executeSQL(sqlDb, sql);
}

bool DataBase::isLogging(const int type)
{
    int logging = appManager->settings->getIntValue(SettingCode_Logging);
    return logging > 0 && type > 0 && type <= logging;
}

void DataBase::saveLog(const int type, const int source, const QString &comment)
{
    if (started && isLogging(type))
    {
        auto future = QtConcurrent::run([type, source, comment, this]
        {
            Tools::debugLog(QString("@@@@@ DataBase::saveLog %1 %2 %3").arg(QString::number(type), QString::number(source), comment));
            LogDBTable* t = (LogDBTable*)getTable(DBTABLENAME_LOG);
            if (t != nullptr && insertRecord(logDB, t, t->createRecord(type, source, comment)))
            {
                // remove old records:
                DBTable* t = getTable(DBTABLENAME_LOG);
                quint64 logDuration = appManager->settings->getIntValue(SettingCode_LogDuration);
                if(t == nullptr || logDuration <= 0) return;
                if(removeOldLogRecordsCounter == 0)
                {
                    quint64 first = Tools::nowMsec() - logDuration * 24 * 60 * 60 * 1000;
                    QString sql = QString("DELETE FROM %1 WHERE %2 < '%3';").arg(
                                t->name, t->columnName(LogDBTable::DateTime), QString::number(first));
                    if(!executeSQL(logDB, sql)) Tools::debugLog("@@@@@ DataBase::removeOldLogRecords ERROR");
                }
                if((++removeOldLogRecordsCounter) >= MAX_REMOVE_OLD_LOG_RECORDS_COUNTER) removeOldLogRecordsCounter = 0;
            }
            else Tools::debugLog("@@@@@ DataBase::saveLog ERROR");
        });
    }
}

void DataBase::clearLog()
{
    if (!started) return;
    Tools::debugLog("@@@@@ DataBase::clearLog");
    DBTable* t = getTable(DBTABLENAME_LOG);
    if(t == nullptr) return;
    QString sql = QString("DELETE FROM %1;").arg(t->name);
    if(!executeSQL(logDB, sql)) Tools::debugLog("@@@@@ DataBase::clearLog ERROR");
}

void DataBase::select(const DBSelector selector, const QString& param1, const QString& param2)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос
/*
    Tools::debugLog(QString("@@@@@ DataBase::select %1 ").arg(selector) + param1);
    DBRecordList resultRecords;
    switch(selector)
    {
    case DBSelector_None: return;

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
        const int sort = Tools::stringToInt(param1);
        const bool increase = Tools::stringToBool(param2);
        switch(sort)
        {
        case ShowcaseSort_Code: Tools::sortByInt(resultRecords, ProductDBTable::Code, increase); break;
        case ShowcaseSort_Code2: Tools::sortByInt(resultRecords, ProductDBTable::Code2, increase); break;
        case ShowcaseSort_Name: Tools::sortByString(resultRecords, ProductDBTable::Name, increase); break;
        }
        break;
    }

    case DBSelector_GetImageByResourceCode:
    // Запрос картинки из ресурсов по коду ресурса:
    {
        DBRecord r;
        if(selectById(productDB, DBTABLENAME_PICTURES, param1, r)) resultRecords.append(r);
        break;
    }

    case DBSelector_GetMessageByResourceCode:
    // Запрос сообщения (или файла сообщения?) из ресурсов по коду ресурса:
    // todo
    {
        DBRecord r;
        if(selectById(productDB, DBTABLENAME_MESSAGES, param1, r)) resultRecords.append(r);
        break;
    }

    case DBSelector_GetProductsByGroupCodeIncludeGroups:
    // Запрос списка товаров по коду группы включая группы:
    {
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql;
        DBRecordList rs;
        sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), param1);
        sql += QString(" AND %1 = '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Name));
        executeSelectSQL(productDB, t, sql, rs);
        resultRecords.append(rs);
        sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), param1);
        sql += QString(" AND %1 != '%2';").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        executeSelectSQL(productDB, t, sql, rs);
        Tools::sortByInt(rs, ProductDBTable::Code);
        resultRecords.append(rs);
        break;
    }

    case DBSelector_GetProductsByInputCode:
    // Запрос списка товаров по фрагменту кода:
    {
        QString p = param1.trimmed();
        if (p.isEmpty()) break;
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::Code), p, "%");
        sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Code));
        executeSelectSQL(productDB, t, sql, resultRecords);
        break;
    }

    case DBSelector_GetProductsByFilteredCode:
    // Запрос списка товаров по фрагменту кода исключая группы:
    {
        QString p = param1.trimmed();
        if (p.isEmpty()) break;
        if (!appManager->settings->getBoolValue(SettingCode_SearchType))
        {
            const int n1 = p.size();
            const int n2 = appManager->settings->getIntValue(SettingCode_SearchCodeSymbols);
            Tools::debugLog(QString("@@@@@ DataBase::select GetProductsByFilteredCode %1 %2").arg(QString::number(n1), QString::number(n2)));
            if(n1 < n2) break;
        }
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::Code), p, "%");
        sql += QString(" AND %1 != '%2';").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        executeSelectSQL(productDB, t, sql, resultRecords);
        Tools::sortByInt(resultRecords, ProductDBTable::Code); break;
        break;
    }

    case DBSelector_GetProductsByFilteredCode2:
    // Запрос списка товаров по фрагменту номера исключая группы:
    {
        QString p = param1.trimmed();
        if (p.isEmpty()) break;
        if (!appManager->settings->getBoolValue(SettingCode_SearchType))
        {
            const int n1 = p.size();
            const int n2 = appManager->settings->getIntValue(SettingCode_SearchCodeSymbols);
            Tools::debugLog(QString("@@@@@ DataBase::select GetProductsByFilteredNumber %1 %2").arg(QString::number(n1), QString::number(n2)));
            if(n1 < n2) break;
        }
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::Code2), p, "%");
        sql += QString(" AND %1 != '%2';").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        executeSelectSQL(productDB, t, sql, resultRecords);
        Tools::sortByInt(resultRecords, ProductDBTable::Code2); break;
        break;
    }

    case DBSelector_GetProductsByFilteredBarcode:
    // Запрос списка товаров по фрагменту штрих-кода исключая группы:
    {
        QString p = param1.trimmed();
        if (p.isEmpty()) break;
        if (!appManager->settings->getBoolValue(SettingCode_SearchType))
        {
            const int n1 = p.size();
            const int n2 = appManager->settings->getIntValue(SettingCode_SearchBarcodeSymbols);
            Tools::debugLog(QString("@@@@@ DataBase::select GetProductsByFilteredBarcode %1 %2").arg(QString::number(n1), QString::number(n2)));
            if(n1 < n2) break;
        }
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::Barcode), p, "%");
        sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Barcode));
        executeSelectSQL(productDB, t, sql, resultRecords);
        break;
    }

    case DBSelector_GetProductsByFilteredName:
    // Запрос списка товаров по фрагменту наименования исключая группы:
    {
        QString p = param1.trimmed();
        if (p.isEmpty()) break;
        if (!appManager->settings->getBoolValue(SettingCode_SearchType))
        {
            const int n1 = p.size();
            const int n2 = appManager->settings->getIntValue(SettingCode_SearchNameSymbols);
            Tools::debugLog(QString("@@@@@ DataBase::select GetProductsByFilteredName %1 %2").arg(QString::number(n1), QString::number(n2)));
            if(n1 < n2) break;
        }
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::UpperName), p.toUpper(), "%");
        sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Name));
        executeSelectSQL(productDB, t, sql, resultRecords);
        break;
    }

    case DBSelector_GetProductByInputCode:
    case DBSelector_SetProductByInputCode:
    case DBSelector_RefreshCurrentProduct:
    // Запрос товара по коду
    {
        DBRecord r;
        if(selectById(productDB, DBTABLENAME_PRODUCTS, param1, r)) resultRecords.append(r);
        break;
    }

    default: break;
    }
    Tools::debugLog(QString("@@@@@ DataBase::select records = %1").arg(resultRecords.count()));
    emit selectResult(selector, resultRecords, true);
*/

    auto future = QtConcurrent::run([selector, param1, param2, this]
    {
        Tools::debugLog(QString("@@@@@ DataBase::select %1 ").arg(selector) + param1);
        DBRecordList resultRecords;
        switch(selector)
        {
        case DBSelector_None: return;

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
            const int sort = Tools::stringToInt(param1);
            const bool increase = Tools::stringToBool(param2);
            switch(sort)
            {
            case ShowcaseSort_Code: Tools::sortByInt(resultRecords, ProductDBTable::Code, increase); break;
            case ShowcaseSort_Code2: Tools::sortByInt(resultRecords, ProductDBTable::Code2, increase); break;
            case ShowcaseSort_Name: Tools::sortByString(resultRecords, ProductDBTable::Name, increase); break;
            }
            break;
        }

        case DBSelector_GetImageByResourceCode:
        // Запрос картинки из ресурсов по коду ресурса:
        {
            DBRecord r;
            if(selectById(productDB, DBTABLENAME_PICTURES, param1, r)) resultRecords.append(r);
            break;
        }

        case DBSelector_GetMessageByResourceCode:
        // Запрос сообщения (или файла сообщения?) из ресурсов по коду ресурса:
        // todo
        {
            DBRecord r;
            if(selectById(productDB, DBTABLENAME_MESSAGES, param1, r)) resultRecords.append(r);
            break;
        }

        case DBSelector_GetProductsByGroupCodeIncludeGroups:
        // Запрос списка товаров по коду группы включая группы:
        {
            DBTable* t = getTable(DBTABLENAME_PRODUCTS);
            QString sql;
            DBRecordList rs;
            sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), param1);
            sql += QString(" AND %1 = '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Name));
            executeSelectSQL(productDB, t, sql, rs);
            resultRecords.append(rs);
            sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), param1);
            sql += QString(" AND %1 != '%2';").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            executeSelectSQL(productDB, t, sql, rs);
            Tools::sortByInt(rs, ProductDBTable::Code);
            resultRecords.append(rs);
            break;
        }

        case DBSelector_GetProductsByInputCode:
        // Запрос списка товаров по фрагменту кода:
        {
            QString p = param1.trimmed();
            if (p.isEmpty()) break;
            DBTable* t = getTable(DBTABLENAME_PRODUCTS);
            QString sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::Code), p, "%");
            sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Code));
            executeSelectSQL(productDB, t, sql, resultRecords);
            break;
        }

        case DBSelector_GetProductsByFilteredCode:
        // Запрос списка товаров по фрагменту кода исключая группы:
        {
            QString p = param1.trimmed();
            if (p.isEmpty()) break;
            if (!appManager->settings->getBoolValue(SettingCode_SearchType))
            {
                const int n1 = p.size();
                const int n2 = appManager->settings->getIntValue(SettingCode_SearchCodeSymbols);
                Tools::debugLog(QString("@@@@@ DataBase::select GetProductsByFilteredCode %1 %2").arg(QString::number(n1), QString::number(n2)));
                if(n1 < n2) break;
            }
            DBTable* t = getTable(DBTABLENAME_PRODUCTS);
            QString sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::Code), p, "%");
            sql += QString(" AND %1 != '%2';").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            executeSelectSQL(productDB, t, sql, resultRecords);
            Tools::sortByInt(resultRecords, ProductDBTable::Code); break;
            break;
        }

        case DBSelector_GetProductsByFilteredCode2:
        // Запрос списка товаров по фрагменту номера исключая группы:
        {
            QString p = param1.trimmed();
            if (p.isEmpty()) break;
            if (!appManager->settings->getBoolValue(SettingCode_SearchType))
            {
                const int n1 = p.size();
                const int n2 = appManager->settings->getIntValue(SettingCode_SearchCodeSymbols);
                Tools::debugLog(QString("@@@@@ DataBase::select GetProductsByFilteredNumber %1 %2").arg(QString::number(n1), QString::number(n2)));
                if(n1 < n2) break;
            }
            DBTable* t = getTable(DBTABLENAME_PRODUCTS);
            QString sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::Code2), p, "%");
            sql += QString(" AND %1 != '%2';").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            executeSelectSQL(productDB, t, sql, resultRecords);
            Tools::sortByInt(resultRecords, ProductDBTable::Code2); break;
            break;
        }

        case DBSelector_GetProductsByFilteredBarcode:
        // Запрос списка товаров по фрагменту штрих-кода исключая группы:
        {
            QString p = param1.trimmed();
            if (p.isEmpty()) break;
            if (!appManager->settings->getBoolValue(SettingCode_SearchType))
            {
                const int n1 = p.size();
                const int n2 = appManager->settings->getIntValue(SettingCode_SearchBarcodeSymbols);
                Tools::debugLog(QString("@@@@@ DataBase::select GetProductsByFilteredBarcode %1 %2").arg(QString::number(n1), QString::number(n2)));
                if(n1 < n2) break;
            }
            DBTable* t = getTable(DBTABLENAME_PRODUCTS);
            QString sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::Barcode), p, "%");
            sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Barcode));
            executeSelectSQL(productDB, t, sql, resultRecords);
            break;
        }

        case DBSelector_GetProductsByFilteredName:
        // Запрос списка товаров по фрагменту наименования исключая группы:
        {
            QString p = param1.trimmed();
            if (p.isEmpty()) break;
            if (!appManager->settings->getBoolValue(SettingCode_SearchType))
            {
                const int n1 = p.size();
                const int n2 = appManager->settings->getIntValue(SettingCode_SearchNameSymbols);
                Tools::debugLog(QString("@@@@@ DataBase::select GetProductsByFilteredName %1 %2").arg(QString::number(n1), QString::number(n2)));
                if(n1 < n2) break;
            }
            DBTable* t = getTable(DBTABLENAME_PRODUCTS);
            QString sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 LIKE '%2%3'").arg(t->columnName(ProductDBTable::UpperName), p.toUpper(), "%");
            sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Name));
            executeSelectSQL(productDB, t, sql, resultRecords);
            break;
        }

        case DBSelector_GetProductByInputCode:
        case DBSelector_SetProductByInputCode:
        case DBSelector_RefreshCurrentProduct:
        // Запрос товара по коду
        {
            DBRecord r;
            if(selectById(productDB, DBTABLENAME_PRODUCTS, param1, r)) resultRecords.append(r);
            break;
        }

        default: break;
        }
        Tools::debugLog(QString("@@@@@ DataBase::select records = %1").arg(resultRecords.count()));
        emit selectResult(selector, resultRecords, true);
    });
}

void DataBase::select(const DBSelector selector, const DBRecordList& param)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос
/*
    Tools::debugLog(QString("@@@@@ DataBase::select %1 %2").arg(QString::number(selector), QString::number(param.count())));
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
    emit selectResult(selector, resultRecords, true);
*/

    auto future = QtConcurrent::run([selector, param, this]
    {
        Tools::debugLog(QString("@@@@@ DataBase::select %1 %2").arg(QString::number(selector), QString::number(param.count())));
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
        emit selectResult(selector, resultRecords, true);
    });
}

void DataBase::saveTransaction(const DBRecord& r)
{
    auto future = QtConcurrent::run([r, this]
    {
        Tools::debugLog("@@@@@ DataBase::saveTransaction");
        insertRecord(logDB, getTable(DBTABLENAME_TRANSACTIONS), r);
    });
}

bool DataBase::isInShowcase(const DBRecord& record)
{
    DBRecord r;
    const QString& code = record[ProductDBTable::Code].toString();
    return selectById(productDB, getTable(DBTABLENAME_SHOWCASE), code, r) && !r.empty();
}

bool DataBase::removeFromShowcase(const DBRecord& record)
{
    const QString& code = record[ProductDBTable::Code].toString();
    Tools::debugLog("@@@@@ DataBase::removeFromShowcase " + code);
    return removeRecord(productDB, getTable(DBTABLENAME_SHOWCASE), code);
}

bool DataBase::addToShowcase(const DBRecord& record)
{
    const QString& code = record[ProductDBTable::Code].toString();
    Tools::debugLog("@@@@@ DataBase::addToShowcase " + code);
    ShowcaseDBTable* t = (ShowcaseDBTable*)getTable(DBTABLENAME_SHOWCASE);
    return t != nullptr && insertRecord(productDB, t, t->createRecord(code));
}

QString DataBase::netDelete(const QString& tableName, const QString& codeList)
{
    // Удаление из таблицы по списку кодов

    Tools::debugLog(QString("@@@@@ DataBase::netDelete %1").arg(tableName));
    saveLog(LogType_Error, LogSource_DB, QString(" Удаление. Таблица: %1").arg(tableName));
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    bool detailedLog = isLogging(LogType_Info);
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
        if(removeAll(productDB, t) && detailedLog)
        {
            QString s = QString("Удалены все записи таблицы %1").arg(tableName);
            saveLog(LogType_Warning, LogSource_DB, s);
        }
    }
    else
    {
        for (int i = 0; i < codes.count(); i++)
        {
            if(removeRecord(productDB, t, codes[i]))
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
    QString resultJson = NetServer::makeResultJson(errorCode, description, tableName, DBTable::toJsonString(t, records));
    Tools::debugLog(QString("@@@@@ DataBase::netDelete result = %1").arg(resultJson));
    return resultJson;
}

QString DataBase::netUpload(const QString& tableName, const QString& codesToUpload, const bool codesOnly)
{
    // Выгрузка из таблицы

    Tools::debugLog(QString("@@@@@ DataBase::netUpload %1 %2").arg(tableName, Tools::boolToString(codesOnly)));
    if(codesOnly) saveLog(LogType_Error, LogSource_DB, QString("Выгрузка. Таблица: %1 %2").arg(
                tableName, Tools::intToString(codesToUpload.split(',').count())));
    else saveLog(LogType_Error, LogSource_DB, QString("Выгрузка кодов. Таблица: %1").arg(tableName));
    int recordCount = 0;
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";
    bool detailedLog = isLogging(LogType_Info);
    DBRecordList records;
    DBTable* t = getTable(tableName);
    QStringList codes = codesToUpload.split(','); // Коды через запятую

    if(t == nullptr)
    {
        errorCount = 1;
        errorCode = LogError_UnknownTable;
        description = "Неизвестная таблица";
    }
    else if (codesOnly) // Только коды
    {
        QStringList codes = selectAllCodes(productDB, t);
        saveLog(LogType_Error, LogSource_DB, QString("Выгрузка кодов завершена. Таблица: %1. Записи: %2").
                arg(tableName, QString::number(codes.count())));
        const QString resultJson = NetServer::makeResultJson(errorCode, description, tableName, codes);
        Tools::debugLog(QString("@@@@@ DataBase::netUpload result = %1").arg(resultJson));
        return resultJson;
    }
    else if (codes.isEmpty() || codes[0].isEmpty()) // Upload all fields
    {
        selectAll(productDB, t, records);
        recordCount = records.count();
        if (!codesOnly && detailedLog)
        {
            for (int i = 0; i < recordCount; i++)
            {
                saveLog(LogType_Warning, LogSource_DB, QString("Выгружена запись. Таблица: %1. Код: %2").
                        arg(tableName, records[i].at(0).toString()));
            }
        }
    }
    else // По списку кодов
    {
        for (int i = 0; i < codes.count(); i++)
        {
            DBRecord r;
            if(selectById(productDB, t, codes[i], r))
            {
                recordCount++;
                records.append(r);
                if (detailedLog)
                    saveLog(LogType_Warning, LogSource_DB, QString("Запись выгружена. Таблица: %1. Код: %2").
                            arg(tableName, r.at(0).toString()));
            }
            else
            {
                errorCount++;
                errorCode = LogError_RecordNotFound;
                description = "Запись не найдена";
                if (detailedLog)
                    saveLog(LogType_Error, LogSource_DB, QString("Ошибка выгрузки записи. Таблица: %1. Код ошибки: %2. Описание: %3").
                            arg(tableName, QString::number(errorCode), description));
            }
        }
    }
    saveLog(LogType_Error, LogSource_DB, QString("Выгрузка завершена. Таблица: %1. Записи: %2. Ошибки: %3. Описание: %4").
            arg(tableName, QString::number(recordCount), QString::number(errorCount), description));
    const QString resultJson = NetServer::makeResultJson(errorCode, description, tableName, DBTable::toJsonString(t, records));
    Tools::debugLog(QString("@@@@@ DataBase::netUpload result = %1").arg(resultJson));
    return resultJson;
}

void DataBase::netDownload(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount)
{
    Tools::debugLog("@@@@@ DataBase::netDownload");
    bool detailedLog = isLogging(LogType_Info);
    QList tables = records.keys();
    for (DBTable* table : tables)
    {
        DBRecordList tableRecords = records.value(table);
        for(DBRecord& r : tableRecords)
        {
            QString code = r.count() > 0 ? r.at(0).toString() : "";
            QString s;
            if(code.isEmpty() || !insertRecord(productDB, table, r))
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
            Tools::debugLog("@@@@@ DataBase::netDownload "+ s);
        }
    }
}
/*
bool DataBase::copyDBFile(const QString& fromName, const QString& toName)
{
    Tools::debugLog(QString("@@@@@ DataBase::copyDBFile %1 %2").arg(fromName, toName));
    if(Tools::copyFile(Tools::dbPath(fromName), Tools::dbPath(toName))) return true;
    Tools::debugLog("@@@@@ DataBase::copyDBFile ERROR");
    return false;
}

bool DataBase::renameDBFile(const QString& fromName, const QString& toName)
{
    Tools::debugLog(QString("@@@@@ DataBase::renameDBFile %1 %2").arg(fromName, toName));
    if(Tools::renameFile(Tools::dbPath(fromName), Tools::dbPath(toName))) return true;
    Tools::debugLog("@@@@@ DataBase::renameDBFile ERROR");
    return false;
}
*/
