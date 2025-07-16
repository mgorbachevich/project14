#include <QSqlError>
#include <QSqlQuery>
#include <QtConcurrent/QtConcurrent>
#include <QTimer>
#include "database.h"
#include "productdbtable.h"
#include "transactiondbtable.h"
#include "resourcedbtable.h"
#include "logdbtable.h"
#include "tools.h"
#include "constants.h"
#include "appmanager.h"
#include "calculator.h"
#include "users.h"
#include "netactionresult.h"
#include "settings.h"

bool isExecuting = false;

DataBase::DataBase(AppManager *parent) : ExternalMessager(parent)
{
    Tools::debugLog("@@@@@ DataBase::DataBase");
    tables.append(new ProductDBTable(DBTABLENAME_PRODUCTS, productDB, this));
    tables.append(new ResourceDBTable(DBTABLENAME_LABELS, productDB, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGES, productDB, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MESSAGEFILES, productDB, this));
    tables.append(new ResourceDBTable(DBTABLENAME_PICTURES, productDB, this));
    tables.append(new ResourceDBTable(DBTABLENAME_MOVIES, productDB, this));
    tables.append(new ResourceDBTable(DBTABLENAME_SOUNDS, productDB, this));
    tables.append(new LogDBTable(DBTABLENAME_LOG, logDB, this));
    tables.append(new TransactionDBTable(DBTABLENAME_TRANSACTIONS, logDB, this));
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
    QString dbFilePath = Tools::dbPath(DB_PRODUCT_NAME);

#ifdef REMOVE_PRODUCT_DB_ON_START
    Tools::removeFile(dbFilePath);
#endif

    bool exists = QFile(dbFilePath).exists();
    Tools::debugLog(QString("@@@@@ DataBase::onAppStart %1 %2").arg(dbFilePath, Tools::sortIncrement(exists)));
    started = addAndOpen(productDB, dbFilePath);
    if(!exists && started)
    {
        started &= createTable(getTable(DBTABLENAME_PRODUCTS));
        started &= createTable(getTable(DBTABLENAME_LABELS));
        started &= createTable(getTable(DBTABLENAME_MESSAGES));
        started &= createTable(getTable(DBTABLENAME_MESSAGEFILES));
        started &= createTable(getTable(DBTABLENAME_PICTURES));
        started &= createTable(getTable(DBTABLENAME_MOVIES));
        started &= createTable(getTable(DBTABLENAME_SOUNDS));
        if(!started) message += "\nОШИБКА! Не создана БД товаров";
    }
    if (started)
    {
        dbFilePath = Tools::dbPath(DB_LOG_NAME);

#ifdef REMOVE_LOG_DB_ON_START
        Tools::removeFile(path);
#endif
        exists = QFile(dbFilePath).exists();
        Tools::debugLog(QString("@@@@@ DataBase::onAppStart %1 %2").arg(dbFilePath, Tools::sortIncrement(exists)));
        started = addAndOpen(logDB, dbFilePath);
        if(!exists && started)
        {
            started &= createTable(getTable(DBTABLENAME_LOG));
            started &= createTable(getTable(DBTABLENAME_TRANSACTIONS));
            if(!started) message += "\nОШИБКА! Не создана БД лога";
        }
    }
    if (started)
    {
        for (DBTable* t : tables) t->createIndexes(); // ?

        // Не выгружаем поля:
        getTable(DBTABLENAME_PRODUCTS)->setColumnNotUploadable(ProductDBTable::UpperName);
        getTable(DBTABLENAME_LABELS)  ->setColumnNotUploadable(ResourceDBTable::Source);
        getTable(DBTABLENAME_LABELS)  ->setColumnNotUploadable(ResourceDBTable::Hash);
        getTable(DBTABLENAME_MESSAGES)->setColumnNotUploadable(ResourceDBTable::Name);
        getTable(DBTABLENAME_MESSAGES)->setColumnNotUploadable(ResourceDBTable::Source);
        getTable(DBTABLENAME_PICTURES)->setColumnNotUploadable(ResourceDBTable::Name);
        getTable(DBTABLENAME_PICTURES)->setColumnNotUploadable(ResourceDBTable::Source);

        // Этикетки по умолчанию:
        DBRecordList labels = getTable(DBTABLENAME_LABELS)->parse(Tools::readTextFile(DEFAULT_LABELS_FILE));
        insertRecords(getTable(DBTABLENAME_LABELS), labels);
    }
    else
        message += "\nОШИБКА! Не открыта база данных";

    if(!message.isEmpty())
        QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this, message]() { showAttention(message); });
    emit dbStarted();
    Tools::debugLog(QString("@@@@@ DataBase::startDB %1").arg(Tools::sortIncrement(started)));
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

DBTable* DataBase::getTable(const QString &tableName) const
{
    for (DBTable* t : tables) if (t->name == tableName) return t;
    Tools::debugLog(QString("@@@@@ DataBase::getTable ERROR %1").arg(tableName));
    return nullptr;
}

QString DataBase::getProductMessage(const DBRecord& product)
{
    if(product.count() >= ProductDBTable::MessageCode)
    {
        DBRecord r;
        if(selectById(DBTABLENAME_MESSAGES, product[ProductDBTable::MessageCode].toString(), r) &&
                r.count() > ResourceDBTable::Value)
            return r[ResourceDBTable::Value].toString();
    }
    return "";
}

bool DataBase::createTable(DBTable* table)
{
    if (!started) return false;
    Tools::debugLog(QString("@@@@@ DataBase::createTable %1").arg(table->name));

    QString sql =  "CREATE TABLE " + table->name + " (";
    for (int i = 0; i < table->columnCount(); i++)
    {
        sql += table->columnName(i) + " " + table->columnType(i);
        sql += (i == table->columnCount() - 1) ? ");" : ", ";
    }
    return executeSQL(table->sqlDB, sql);
}

bool DataBase::query(const QSqlDatabase& db, const QString& sql, DBTable* table, DBRecordList* records)
{
    if (!started) return false;
    Tools::debugLog(QString("@@@@@ DataBase::query %1 %2").arg(
                        Tools::fileNameFromPath(db.databaseName()), sql));
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
    while(isExecuting) // жду пока выполнится предыдущий запрос из другого потока
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

bool DataBase::executeSelectSQL(DBTable* table, const QString& sql, DBRecordList& resultRecords)
{
    return query(table->sqlDB, sql, table, &resultRecords) && resultRecords.count() > 0;
}

bool DataBase::selectById(const QString& tableName, const QString& id, DBRecord& resultRecord)
{
    Tools::debugLog(QString("@@@@@ DataBase::selectById %1").arg(id));
    resultRecord.clear();
    DBTable* t = getTable(tableName);
    if(t == nullptr) return false;
    QString sql = QString("SELECT * FROM %1 WHERE %2 = '%3';").arg(t->name, t->columnName(0), id);
    DBRecordList records;
    if(!executeSelectSQL(t, sql, records) || records.count() == 0) return false;
    resultRecord = records.at(0);
    return true;
}

void DataBase::selectAll(DBTable *table, DBRecordList& resultRecords)
{
    Tools::debugLog("@@@@@ DataBase::selectAll");
    resultRecords.clear();
    if(table != nullptr)
    {
        DBRecordList records;
        QString sql = QString("SELECT * FROM %1;").arg(table->name);
        executeSelectSQL(table, sql, records);
        resultRecords.append(table->checkList(records));
    }
}

QStringList DataBase::selectAllCodes(DBTable *table)
{
    Tools::debugLog("@@@@@ DataBase::selectAllCodes");
    QStringList result;
    if(table != nullptr)
    {
        DBRecordList records;
        QString sql = QString("SELECT %1 FROM %2;").arg(table->columnName(0), table->name);
        executeSelectSQL(table, sql, records);
        Tools::sortByInt(records, 0);
        for(int i = 0; i < records.count(); i++) result << records[i][0].toString();
    }
    return result;
}

bool DataBase::removeAll(DBTable* table)
{
    Tools::debugLog(QString("@@@@@ DataBase::removeAll %1").arg(table->name));
    QString sql = QString("DELETE FROM %1;").arg(table->name);
    bool ok = executeSQL(table->sqlDB, sql);
    return ok;
}

bool DataBase::removeRecord(DBTable* table, const QString& code)
{
    if (!started) return false;
    Tools::debugLog(QString("@@@@@ DataBase::removeRecord %1").arg(table->name));
    QString sql = QString("DELETE FROM %1 WHERE %2 = '%3';").arg(table->name, table->columnName(0), code);
    return executeSQL(table->sqlDB, sql);
}

bool DataBase::insertRecord(DBTable *table, const DBRecord& record)
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
        // saveLog(LogType_Error, "БД. Не сохранена запись в таблице " + table->name);
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
#ifdef DEBUG_INSERT_DB_DELAY_MSEC
    Tools::pause(DEBUG_INSERT_DB_DELAY_MSEC); // замедляю загрузку для отладки
#endif
    return executeSQL(table->sqlDB, sql);
}

bool DataBase::insertRecords(DBTable *table, const DBRecordList& records)
{
    bool ok = true;
    foreach (auto r, records) ok &= insertRecord(table, r);
    return ok;
}

bool DataBase::isLogging(const int type)
{
    int logging = appManager->settings->getIntValue(SettingCode_Logging);
    return logging > 0 && type > 0 && type <= logging;
}

void DataBase::saveLog(const int type, const int source, const QString &comment)
{
    if (!started || !isLogging(type)) return;
    auto future = QtConcurrent::run([type, source, comment, this]
    {
        Tools::debugLog(QString("@@@@@ DataBase::saveLog %1 %2 %3").arg(
                            QString::number(type), QString::number(source), comment));
        LogDBTable* t = (LogDBTable*)getTable(DBTABLENAME_LOG);
        if (t != nullptr && insertRecord(t, t->createRecord(type, source, comment)))
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
                if(!executeSQL(t->sqlDB, sql)) Tools::debugLog("@@@@@ DataBase::removeOldLogRecords ERROR");
            }
            if((++removeOldLogRecordsCounter) >= MAX_REMOVE_OLD_LOG_RECORDS_COUNTER)
                removeOldLogRecordsCounter = 0;
        }
        else Tools::debugLog("@@@@@ DataBase::saveLog ERROR");
    });
}

void DataBase::clearLog()
{
    if (started)
    {
        Tools::debugLog("@@@@@ DataBase::clearLog");
        DBTable* t = getTable(DBTABLENAME_LOG);
        if(t != nullptr)
        {
            QString sql = QString("DELETE FROM %1;").arg(t->name);
            if(!executeSQL(t->sqlDB, sql)) Tools::debugLog("@@@@@ DataBase::clearLog ERROR");
        }
    }
}

/*
QString DataBase::getLabelPathByName(const QString& labelName)
{
    DBRecordList rs;
    DBTable* t = getTable(DBTABLENAME_LABELS);
    QString sql = QString("SELECT * FROM %1").arg(t->name);
    sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ResourceDBTable::Name), labelName);
    sql += QString(" ORDER BY %1 ASC").arg(t->columnName(ResourceDBTable::Code));
    executeSelectSQL(t, sql, rs);
    if(rs.count() < 1) return "";
    return rs[0][ResourceDBTable::Value].toString();
}
*/

QString DataBase::getLabelPathById(const QString& code)
{
    DBRecord r;
    if (!selectById(DBTABLENAME_LABELS, code, r)) return "";
    return r[ResourceDBTable::Value].toString();
}

DBRecord DataBase::selectByCode(const QString& tableName, const QString& code)
{
    Tools::debugLog(QString("@@@@@ DataBase::selectByCode %1 %2").arg(tableName, code));
    DBRecord r;
    if(!selectById(tableName, code, r)) r.clear();
    return r;
}

void DataBase::saveTransaction(const DBRecord& r)
{
    Tools::debugLog("@@@@@ DataBase::saveTransaction");
    insertRecord(getTable(DBTABLENAME_TRANSACTIONS), r);
}

NetActionResult DataBase::netDelete(const QString& tableName, const QString& codeList)
{
    // Удаление из таблицы по списку кодов

    Tools::debugLog(QString("@@@@@ DataBase::netDelete %1").arg(tableName));
    saveLog(LogType_Error, LogSource_DB, QString(" Удаление. Таблица: %1").arg(tableName));
    NetActionResult result(RouterRule_Delete);
    bool detailedLog = isLogging(LogType_Info);
    DBTable* t = getTable(tableName);
    QStringList codes = Tools::toStringList(codeList); // Коды товаров через запятую

    if(t == nullptr)
    {
        result.errorCount = 1;
        result.errorCode = Error_Log_UnknownTable;
        result.description = "Неизвестная таблица";
    }
    else
    {
        t->removeIndexes();
        if (codes.isEmpty() || codes[0].isEmpty()) // Delete all records
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
#ifdef LOG_LOAD_RECORDS
                    if (detailedLog)
                    {
                        QString s = QString("Запись удалена. Таблица: %1. Код: %2").arg(tableName, codes[i]);
                        saveLog(LogType_Warning, LogSource_DB, s);
                    }
#endif
                }
                else
                {
                    result.errorCount++;
                    result.errorCode = Error_Log_RecordNotFound;
                    result.description = "Запись не найдена";
                    if (detailedLog)
                    {
                        QString s = QString("Ошибка выгрузки записи. Таблица: %1. Код ошибки: %2. Описание: %3").
                                arg(tableName, QString::number(result.errorCode), result.description);
                        saveLog(LogType_Error, LogSource_DB, s);
                    }
                }
            }
        }
        t->createIndexes();
    }
    QString s = QString("Удаление завершено. Таблица: %1. Ошибки: %2. Описание: %3").
            arg(tableName, QString::number(result.errorCount), result.description);
    saveLog(LogType_Error, LogSource_DB, s);
    DBRecordList records;
    result.requestReply = result.makeJson(DBTable::toJsonObject(t, records));
    Tools::debugLog(QString("@@@@@ DataBase::netDelete result = %1").arg(result.requestReply));
    return result;
}

NetActionResult DataBase::netUpload(Settings* settings,
                            Users* users,
                            const QString& tableName,
                            const QString& codesToUpload,
                            const bool codesOnly)
{
    // Выгрузка из таблицы

    Tools::debugLog(QString("@@@@@ DataBase::netUpload %1 %2").arg(
                        tableName, Tools::sortIncrement(codesOnly)));
    if(codesOnly) saveLog(LogType_Error, LogSource_DB, QString("Выгрузка. Таблица: %1 %2").arg(
                tableName, Tools::toString((int)(Tools::toStringList(codesToUpload).count()))));
    else saveLog(LogType_Error, LogSource_DB, QString("Выгрузка кодов. Таблица: %1").arg(tableName));
    NetActionResult result(RouterRule_Get);

    QJsonObject jo;
    if(tableName == DBTABLENAME_USERS)         jo = users->toJsonObject();
    else if(tableName == DBTABLENAME_SETTINGS) jo = settings->toJsonObject();
    else if(tableName == DBTABLENAME_CONFIG)   jo = settings->getScaleConfig();
    if(!jo.isEmpty())
    {
        result.requestReply = result.makeJson(jo);
        Tools::debugLog(QString("@@@@@ DataBase::netUpload result = %1").arg(result.requestReply));
        QString s = QString("Выгрузка завершена. Таблица: %1. Записи: %2. Ошибки: %3. Описание: %4").arg(
                    tableName, QString::number(result.recordCount),
                    QString::number(result.errorCount), result.description);
        saveLog(LogType_Error, LogSource_DB, s);
        return result;
    }

    bool detailedLog = isLogging(LogType_Info);
    DBRecordList records;
    DBTable* t = getTable(tableName);
    QStringList codes = Tools::toStringList(codesToUpload); // Коды через запятую
    if(t == nullptr)
    {
        result.errorCount = 1;
        result.errorCode = Error_Log_UnknownTable;
        result.description = "Неизвестная таблица";
    }
    else if (codesOnly) // Только коды
    {
        QStringList allCodes = selectAllCodes(t);
        QString s =  QString("Выгрузка кодов завершена. Таблица: %1. Записи: %2").
                arg(tableName, QString::number(allCodes.count()));
        saveLog(LogType_Error, LogSource_DB, s);
        result.requestReply = result.makeCodeListJson(tableName, allCodes);
        Tools::debugLog(QString("@@@@@ DataBase::netUpload result = %1").arg(result.requestReply));
        return result;
    }
    else if (codes.isEmpty() || codes[0].isEmpty()) // Upload all fields
    {
        selectAll(t, records);
        result.recordCount = records.count();
#ifdef LOG_LOAD_RECORDS
        if (!codesOnly && detailedLog)
        {
            for (int i = 0; i < result.recordCount; i++)
                saveLog(LogType_Warning, LogSource_DB, QString("Выгружена запись. Таблица: %1. Код: %2").arg(
                            tableName, records[i].at(0).toString()));
        }
#endif
    }
    else // По списку кодов
    {
        for (int i = 0; i < codes.count(); i++)
        {
            DBRecord r;
            if(selectById(tableName, codes[i], r))
            {
                result.recordCount++;
                records.append(r);
#ifdef LOG_LOAD_RECORDS
                if (detailedLog)
                    saveLog(LogType_Warning, LogSource_DB, QString("Запись выгружена. Таблица: %1. Код: %2").
                        arg(tableName, r.at(0).toString()));
#endif
            }
            else
            {
                result.errorCount++;
                result.errorCode = Error_Log_RecordNotFound;
                result.description = "Запись не найдена";
                if (detailedLog)
                    saveLog(LogType_Error, LogSource_DB, QString("Ошибка выгрузки записи. Таблица: %1. Код ошибки: %2. Описание: %3").
                            arg(tableName, QString::number(result.errorCode), result.description));
            }
        }
    }
    QString s = QString("Выгрузка завершена. Таблица: %1. Записи: %2. Ошибки: %3. Описание: %4").arg(
                tableName, QString::number(result.recordCount),
                QString::number(result.errorCount), result.description);
    saveLog(LogType_Error, LogSource_DB, s);
    result.requestReply = result.makeJson(DBTable::toJsonObject(t, records));
    Tools::debugLog(QString("@@@@@ DataBase::netUpload result = %1").arg(result.requestReply));
    return result;
}

void DataBase::netDownload(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount)
{
    Tools::debugLog("@@@@@ DataBase::netDownload");
    bool detailedLog = isLogging(LogType_Info);
    QList tables = records.keys();
    for (DBTable* t : tables)
    {
        t->removeIndexes();
        DBRecordList tableRecords = records.value(t);
        for(DBRecord& r : tableRecords)
        {
            QString code = r.count() > 0 ? r.at(0).toString() : "";
            QString s;
            if(code.isEmpty() || !insertRecord(t, r))
            {
                errorCount++;
                s = QString("Ошибка загрузки записи. Таблица: %1. Код: %2. Код ошибки: %3. Описание: Некорректная запись").
                        arg(t->name, code, QString::number(Error_Log_WrongRecord));
                if (detailedLog) saveLog(LogType_Error, LogSource_DB, s);
            }
            else
            {
                successCount++;
                s = QString("Запись загружена. Таблица: %1. Код: %2").arg(t->name, code);
#ifdef LOG_LOAD_RECORDS
                if (detailedLog) saveLog(LogType_Warning, LogSource_DB, s);
#endif
            }
            Tools::debugLog("@@@@@ DataBase::netDownload "+ s);
        }
        t->createIndexes();
    }
}

bool DataBase::copyDBFile(const QString& fromDBName, const QString& toDBName)
{
    Tools::debugLog(QString("@@@@@ DataBase::copyDBFile %1 %2").arg(fromDBName, toDBName));
    if(Tools::copyFile(Tools::dbPath(fromDBName), Tools::dbPath(toDBName))) return true;
    Tools::debugLog("@@@@@ DataBase::copyDBFile ERROR");
    return false;
}

bool DataBase::renameDBFile(const QString& fromDBName, const QString& toDBName)
{
    Tools::debugLog(QString("@@@@@ DataBase::renameDBFile %1 %2").arg(fromDBName, toDBName));
    if(Tools::renameFile(Tools::dbPath(fromDBName), Tools::dbPath(toDBName))) return true;
    Tools::debugLog("@@@@@ DataBase::renameDBFile ERROR");
    return false;
}

bool DataBase::removeDBFile(const QString& dbName)
{
    Tools::debugLog(QString("@@@@@ DataBase::removeDBFile %1").arg(dbName));
    if(Tools::removeFile(Tools::dbPath(dbName))) return true;
    Tools::debugLog("@@@@@ DataBase::removeDBFile ERROR");
    return false;
}

bool DataBase::isDBFileExists(const QString& dbName)
{
    return Tools::isFileExists(Tools::dbPath(dbName));
}

DBRecordList DataBase::selectLog()
{
    DBRecordList resultRecords;
    selectAll(getTable(DBTABLENAME_LOG), resultRecords);
    return resultRecords;
}

DBRecordList DataBase::selectProductByNumber(const QString& number)
{
    DBRecordList resultRecords;
    DBTable* t = getTable(DBTABLENAME_PRODUCTS);
    QString sql = QString("SELECT * FROM %1").arg(t->name);
    sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::Code2), number);
    sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
    sql += QString(" ORDER BY %1 ASC").arg(t->columnName(ProductDBTable::Code2));
    executeSelectSQL(t, sql, resultRecords);
    return resultRecords;
}

DBRecordList DataBase::selectProductsByInputCode(const QString& param1, const QString& param2,
                                                 const int offset, const int limit)
{
    DBRecordList resultRecords;
    QString p = param1.trimmed();
    if (!p.isEmpty())
    {
        const int field = Tools::toInt(param2) == ShowcaseSort_Code2 ? ProductDBTable::Code2 : ProductDBTable::Code;
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 LIKE '%2%'").arg(t->columnName(field), p);
        sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC").arg(t->columnName(field));
        if(limit > 0) sql += QString(" LIMIT %1").arg(Tools::toString(limit));
        if(offset > 0) sql += QString(" OFFSET %2").arg(Tools::toString(offset));
        executeSelectSQL(t, sql, resultRecords);
    }
    return resultRecords;
}

DBRecordList DataBase::selectProductsByGroup(const QString& groupCode, const bool includeGroups)
{
    Tools::debugLog("@@@@@ DataBase::selectProductsByGroup " + groupCode);
    DBRecordList resultRecords;
    DBTable* t = getTable(DBTABLENAME_PRODUCTS);
    DBRecordList rs;
    if (includeGroups) // Группы:
    {
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), groupCode);
        sql += QString(" AND %1 = '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::UpperName));
        executeSelectSQL(t, sql, rs);
        resultRecords.append(rs);
    }
    // Товары:
    QString sql = QString("SELECT * FROM %1").arg(t->name);
    sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), groupCode);
    sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
    sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Code));
    executeSelectSQL(t, sql, rs);
    resultRecords.append(rs);
    return resultRecords;
}

DBRecordList DataBase::selectProductsByFilter(const int filter, const QString& param1, const int offset, const int limit)
{
    DBRecordList resultRecords;
    Settings* settings = appManager->settings;
    switch(filter)
    {
    case SearchFilterIndex_Code:
    case SearchFilterIndex_Code2:
    case SearchFilterIndex_Barcode:
    case SearchFilterIndex_Name:
    {
        QString p = param1.trimmed().toUpper();
        if (p.isEmpty()) break;
        SettingCode symbolsCode = SettingCode_SearchCodeSymbols;
        int field = ProductDBTable::Code;
        switch(filter)
        {
        case SearchFilterIndex_Code2:
            field = ProductDBTable::Code2;
            break;
        case SearchFilterIndex_Barcode:
            symbolsCode = SettingCode_SearchBarcodeSymbols;
            field = ProductDBTable::Barcode;
            break;
        case SearchFilterIndex_Name:
            symbolsCode = SettingCode_SearchNameSymbols;
            field = ProductDBTable::UpperName;
            break;
        default: break;
        }
        if (!settings->getBoolValue(SettingCode_SearchType) &&
                p.size() < settings->getIntValue(symbolsCode)) break;
        DBTable* t = getTable(DBTABLENAME_PRODUCTS);
        QString sql = QString("SELECT * FROM %1").arg(t->name);
        sql += QString(" WHERE %1 LIKE '%2%'").arg(t->columnName(field), p);
        sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
        sql += QString(" ORDER BY %1 ASC").arg(t->columnName(field));
        if(limit > 0) sql += QString(" LIMIT %1").arg(Tools::toString(limit));
        if(offset > 0) sql += QString(" OFFSET %2").arg(Tools::toString(offset));
        executeSelectSQL(t, sql, resultRecords);
        break;
    }

    default: break;
    }
    return resultRecords;
}

DBRecordList DataBase::selectShowcaseResources(const DBRecordList& products)
{
    DBRecordList resultRecords;
#ifdef DEBUG_LONG_DB_OPERATIONS
    Tools::pause(5000, "selectShowcaseResources");
#endif
    for (int i = 0; i < products.count(); i++)
    {
        QString imageCode = products[i][ProductDBTable::PictureCode].toString();
        DBRecord r;
        selectById(DBTABLENAME_PICTURES, imageCode, r);
        resultRecords.append(r);
    }
    return resultRecords;
}

DBRecordList DataBase::getAllLabels()
{
    Tools::debugLog("@@@@@ DataBase::getAllLabels");
    DBRecordList resultRecords;
    DBTable* table = getTable(DBTABLENAME_LABELS);
    if(table != nullptr)
    {
        DBRecordList records;
        const QString sql = QString("SELECT * FROM %1 ORDER BY %2 ASC").arg(table->name, table->columnName(0));
        executeSelectSQL(table, sql, records);
        resultRecords.append(table->checkList(records));
    }
    return resultRecords;
}

DBRecordList DataBase::selectSubgroups(const QString& parentGroupCode)
{
    Tools::debugLog("@@@@@ DataBase::selectSubgroups " + parentGroupCode);
    DBRecordList records;
    DBTable* t = getTable(DBTABLENAME_PRODUCTS);
    QString sql = QString("SELECT * FROM %1").arg(t->name);
    sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), parentGroupCode);
    sql += QString(" AND %1 == '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
    executeSelectSQL(t, sql, records);

    DBRecordList resultRecords;
    if(records.count() > 0)
    {
        resultRecords.append(records);
        foreach (DBRecord ri, records)
            resultRecords.append(selectSubgroups(ri[ProductDBTable::Code].toString()));
    }
    return resultRecords;
}

DBRecordList DataBase::selectShowcaseProducts(const DBRecord& currentGroup, const bool showHierarchy)
{
#ifdef DEBUG_LONG_DB_OPERATIONS
    Tools::pause(5000, "selectShowcaseProducts");
#endif
    DBRecordList resultRecords;
    const quint64 currentGroupCode = currentGroup.isEmpty() ? 0 : currentGroup[ProductDBTable::Code].toULongLong();
    Tools::debugLog("@@@@@ DataBase::selectShowcaseProducts currentGroupCode = " + Tools::toString(currentGroupCode));

    // Кнопка "назад" - группа сверху в иерархии
    if (currentGroupCode > 0)
    {
        DBRecord backGroup;
        const quint64 backGroupCode = currentGroup[ProductDBTable::GroupCode].toULongLong();
        if (backGroupCode > 0)
            selectById(DBTABLENAME_PRODUCTS, Tools::toString(backGroupCode), backGroup);
        else
            backGroup = ((ProductDBTable*)getTable(DBTABLENAME_PRODUCTS))->makeRootGroup();
        resultRecords.append(backGroup);
    }

    DBRecordList products;
    if(showHierarchy) // Группы и товары:
    {
        DBRecordList currentGroupRecords = selectProductsByGroup(Tools::toString(currentGroupCode));
        DBRecordList groups;
        foreach (DBRecord ri, currentGroupRecords)
        {
            if(Calculator::isGroup(ri))
            {
                if(!groups.contains(ri)) groups.append(ri);
                continue;
            }
            if(appManager->isProductInShowcase(ri) && !products.contains(ri))
                products.append(ri);
        }
        //Tools::sortByString(groups, ProductDBTable::UpperName);
        resultRecords.append(groups);
    }
    else // Только товары:
    {
        DBRecordList groups = selectSubgroups(Tools::toString(currentGroupCode));
        groups.append(currentGroup);
        foreach (DBRecord gi, groups)
        {
            if(gi.isEmpty()) continue;
            DBRecordList groupProducts = selectProductsByGroup(gi[ProductDBTable::Code].toString(), false);
            foreach (DBRecord pi, groupProducts)
                if(appManager->isProductInShowcase(pi) && !products.contains(pi)) products.append(pi);
        }
    }

    // Сортировка товаров:
    const bool increment = appManager->status.isShowcaseSortIncrement;
    switch(appManager->status.showcaseSort)
    {
    case ShowcaseSort_Code:  Tools::sortByInt(products, ProductDBTable::Code, increment); break;
    case ShowcaseSort_Code2: Tools::sortByInt(products, ProductDBTable::Code2, increment); break;
    case ShowcaseSort_Name:  Tools::sortByString(products, ProductDBTable::UpperName, increment); break;
    }
    resultRecords.append(products);
    return resultRecords;
}


