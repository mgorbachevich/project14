#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include "database.h"
#include "productdbtable.h"
#include "transactiondbtable.h"
#include "resourcedbtable.h"
#include "logdbtable.h"
#include "tools.h"
#include "constants.h"
#include "appmanager.h"

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

    getTable(DBTABLENAME_PRODUCTS)->setColumnNotUploadable(ProductDBTable::UpperName);
    getTable(DBTABLENAME_LABELS)->setColumnNotUploadable(ResourceDBTable::Source);
    getTable(DBTABLENAME_MESSAGES)->setColumnNotUploadable(ResourceDBTable::Name);
    getTable(DBTABLENAME_MESSAGES)->setColumnNotUploadable(ResourceDBTable::Source);
    getTable(DBTABLENAME_PICTURES)->setColumnNotUploadable(ResourceDBTable::Name);
    getTable(DBTABLENAME_PICTURES)->setColumnNotUploadable(ResourceDBTable::Source);
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
    Tools::debugLog(QString("@@@@@ DataBase::onAppStart %1 %2").arg(path, Tools::productSortIncrement(exists)));
    started = addAndOpen(productDB, path);
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
        path = Tools::dbPath(DB_LOG_NAME);
        if(REMOVE_LOG_DB_ON_START) Tools::removeFile(path);
        exists = QFile(path).exists();
        Tools::debugLog(QString("@@@@@ DataBase::onAppStart %1 %2").arg(path, Tools::productSortIncrement(exists)));
        started = addAndOpen(logDB, path);
        if(!exists && started)
        {
            started &= createTable(getTable(DBTABLENAME_LOG));
            started &= createTable(getTable(DBTABLENAME_TRANSACTIONS));
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
        for (DBTable* t : tables) t->createIndexes();
    }
    else
        message += "\nОШИБКА! Не открыта база данных";

    if(!message.isEmpty())
        QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this, message]() { showAttention(message); });
    emit dbStarted();
    Tools::debugLog(QString("@@@@@ DataBase::startDB %1").arg(Tools::productSortIncrement(started)));
}
/*
bool DataBase::open(QSqlDatabase& db, const QString& name)
{
    if (!started) return false;
    const QString path = Tools::dbPath(name);
    Tools::debugLog("@@@@@ DataBase::open " + path);
    if(!QFile(path).exists()) return false;
    return db.isOpen() ? true : db.open();
}
*/
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
    if(selectById(DBTABLENAME_MESSAGES, id, r) && r.count() > ResourceDBTable::Value)
        return r[ResourceDBTable::Value].toString();
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

bool DataBase::executeSelectSQL(DBTable* table, const QString& sql, DBRecordList& resultRecords)
{
    if(DEBUG_DB_SELECT_REPETITIONS <= 0)
        return query(table->sqlDB, sql, table, &resultRecords) && resultRecords.count() > 0;
    else
    {
        bool ok = false;
        quint64 t1 = Tools::nowMsec();
        for(int i = 0; i < DEBUG_DB_SELECT_REPETITIONS; i++)
            ok = query(table->sqlDB, sql, table, &resultRecords) && resultRecords.count() > 0;
        QString ts = Tools::toString(Tools::nowMsec() - t1);
        Tools::debugLog(QString("@@@@@ DataBase::executeSelectSQL %1 ms").arg(ts));
        //appManager->showToast(QString("%1 ms").arg(ts));
        return ok;
    }
}

bool DataBase::selectById(const QString& tableName, const QString& id, DBRecord& resultRecord)
{
    Tools::debugLog(QString("@@@@@ DataBase::selectById %1").arg(id));
    resultRecord.clear();
    DBTable* t = getTable(tableName);
    if(t == nullptr) return false;
    QString sql = QString("SELECT * FROM %1 WHERE %2 = '%3';").arg(t->name, t->columnName(0), id);
    DBRecordList records;
    if(!executeSelectSQL(t, sql, records)) return false;
    if(records.count() > 0) resultRecord.append(records.at(0));
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
    Tools::pause(DEBUG_INSERT_DB_DELAY_MSEC); // замедляю загрузку для отладки
    return executeSQL(table->sqlDB, sql);
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
    if(!executeSQL(t->sqlDB, sql)) Tools::debugLog("@@@@@ DataBase::clearLog ERROR");
}

void DataBase::select(const DBSelector selector, const DBRecordList& param)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос

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
                selectById(DBTABLENAME_PICTURES, imageCode, r);
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
        insertRecord(getTable(DBTABLENAME_TRANSACTIONS), r);
    });
}

bool DataBase::isProductInShowcase(const DBRecord& product)
{
    return appManager->showcase->getByCode(product[0].toInt()) != nullptr;
}

bool DataBase::removeProductFromShowcase(const DBRecord& product)
{
    const QString& code = product[ProductDBTable::Code].toString();
    Tools::debugLog("@@@@@ DataBase::removeProductFromShowcase " + code);
    appManager->showcase->removeByCode(code);
    return true;
}

bool DataBase::addProductToShowcase(const DBRecord& record)
{
    const QString& code = record[ProductDBTable::Code].toString();
    Tools::debugLog("@@@@@ DataBase::addProductToShowcase " + code);
    return appManager->showcase->insertOrReplaceRecord(appManager->showcase->createRecord(code));
}

QString DataBase::netDelete(const QString& tableName, const QString& codeList)
{
    // Удаление из таблицы по списку кодов

    Tools::debugLog(QString("@@@@@ DataBase::netDelete %1").arg(tableName));
    saveLog(LogType_Error, LogSource_DB, QString(" Удаление. Таблица: %1").arg(tableName));
    NetActionResult result(appManager, RouterRule_Delete);
    bool detailedLog = isLogging(LogType_Info);
    DBTable* t = getTable(tableName);
    QStringList codes = codeList.split(','); // Коды товаров через запятую

    if(t == nullptr)
    {
        result.errorCount = 1;
        result.errorCode = LogError_UnknownTable;
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
                    if (detailedLog && LOG_LOAD_RECORDS)
                    {
                        QString s = QString("Запись удалена. Таблица: %1. Код: %2").arg(tableName, codes[i]);
                        saveLog(LogType_Warning, LogSource_DB, s);
                    }
                }
                else
                {
                    result.errorCount++;
                    result.errorCode = LogError_RecordNotFound;
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
    QString resultJson = result.makeJson(tableName, DBTable::toJsonString(t, records));
    Tools::debugLog(QString("@@@@@ DataBase::netDelete result = %1").arg(resultJson));
    return resultJson;
}

QString DataBase::netUpload(const QString& tableName, const QString& codesToUpload, const bool codesOnly)
{
    // Выгрузка из таблицы

    Tools::debugLog(QString("@@@@@ DataBase::netUpload %1 %2").arg(tableName, Tools::productSortIncrement(codesOnly)));
    if(codesOnly) saveLog(LogType_Error, LogSource_DB, QString("Выгрузка. Таблица: %1 %2").arg(
                tableName, Tools::toString((int)(codesToUpload.split(',').count()))));
    else saveLog(LogType_Error, LogSource_DB, QString("Выгрузка кодов. Таблица: %1").arg(tableName));
    NetActionResult result(appManager, RouterRule_Get);

    QString json;
    if(tableName == DBTABLENAME_SETTINGS)   json = appManager->settings->toString();
    else if(tableName == DBTABLENAME_USERS) json = appManager->users->toString();
    if(!json.isEmpty())
    {
        QString resultJson = result.makeJson(json.replace(json.lastIndexOf("}"), 1, " ").replace(json.indexOf("{"), 1, " "));
        Tools::debugLog(QString("@@@@@ DataBase::netUpload result = %1").arg(resultJson));
        QString s = QString("Выгрузка завершена. Таблица: %1. Записи: %2. Ошибки: %3. Описание: %4").
                arg(tableName, QString::number(result.recordCount), QString::number(result.errorCount), result.description);
        saveLog(LogType_Error, LogSource_DB, s);
        return resultJson;
    }

    bool detailedLog = isLogging(LogType_Info);
    DBRecordList records;
    DBTable* t = getTable(tableName);
    QStringList codes = codesToUpload.split(','); // Коды через запятую
    if(t == nullptr)
    {
        result.errorCount = 1;
        result.errorCode = LogError_UnknownTable;
        result.description = "Неизвестная таблица";
    }
    else if (codesOnly) // Только коды
    {
        QStringList allCodes = selectAllCodes(t);
        QString s =  QString("Выгрузка кодов завершена. Таблица: %1. Записи: %2").
                arg(tableName, QString::number(allCodes.count()));
        saveLog(LogType_Error, LogSource_DB, s);
        const QString resultJson = result.makeJson(tableName, allCodes);
        Tools::debugLog(QString("@@@@@ DataBase::netUpload result = %1").arg(resultJson));
        return resultJson;
    }
    else if (codes.isEmpty() || codes[0].isEmpty()) // Upload all fields
    {
        selectAll(t, records);
        result.recordCount = records.count();
        if (!codesOnly && detailedLog && LOG_LOAD_RECORDS)
        {
            for (int i = 0; i < result.recordCount; i++)
                saveLog(LogType_Warning, LogSource_DB, QString("Выгружена запись. Таблица: %1. Код: %2"). arg(tableName, records[i].at(0).toString()));
        }
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
                if (detailedLog && LOG_LOAD_RECORDS)
                    saveLog(LogType_Warning, LogSource_DB, QString("Запись выгружена. Таблица: %1. Код: %2").
                        arg(tableName, r.at(0).toString()));
            }
            else
            {
                result.errorCount++;
                result.errorCode = LogError_RecordNotFound;
                result.description = "Запись не найдена";
                if (detailedLog)
                    saveLog(LogType_Error, LogSource_DB, QString("Ошибка выгрузки записи. Таблица: %1. Код ошибки: %2. Описание: %3").
                            arg(tableName, QString::number(result.errorCode), result.description));
            }
        }
    }
    QString s = QString("Выгрузка завершена. Таблица: %1. Записи: %2. Ошибки: %3. Описание: %4").
            arg(tableName, QString::number(result.recordCount), QString::number(result.errorCount), result.description);
    saveLog(LogType_Error, LogSource_DB, s);
    const QString resultJson = result.makeJson(tableName, DBTable::toJsonString(t, records));
    Tools::debugLog(QString("@@@@@ DataBase::netUpload result = %1").arg(resultJson));
    return resultJson;
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
                        arg(t->name, code, QString::number(LogError_WrongRecord));
                if (detailedLog) saveLog(LogType_Error, LogSource_DB, s);
            }
            else
            {
                successCount++;
                s = QString("Запись загружена. Таблица: %1. Код: %2").arg(t->name, code);
                if (detailedLog && LOG_LOAD_RECORDS) saveLog(LogType_Warning, LogSource_DB, s);
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

void DataBase::select(const DBSelector selector,
                      const QString& param1, const QString& param2,
                      const int offset, const int limit)
{
    // Получен запрос на поиск в БД. Ищем и отвечаем на запрос

    auto future = QtConcurrent::run([selector, param1, param2, offset, limit, this]
    {
        Tools::debugLog(QString("@@@@@ DataBase::select %1 ").arg(selector) + param1);
        DBRecordList resultRecords;

        switch(selector)
        {
        case DBSelector_GetLog: // Запрос списка записей лога:
            selectAll(getTable(DBTABLENAME_LOG), resultRecords);
            break;

        case DBSelector_GetProductByCode:
        case DBSelector_SetProductByCode:
        case DBSelector_RefreshCurrentProduct: // Запрос товара по коду
        case DBSelector_GetImageByResourceCode: // Запрос картинки из ресурсов по коду ресурса:
        case DBSelector_GetMessageByResourceCode: // Запрос сообщения (или файла сообщения?) из ресурсов по коду ресурса:
        {
            QString table = DBTABLENAME_PRODUCTS;
            switch (selector)
            {
            case DBSelector_GetImageByResourceCode:   table = DBTABLENAME_PICTURES; break;
            case DBSelector_GetMessageByResourceCode: table = DBTABLENAME_MESSAGES; break;
            default: break;
            }
            DBRecord r;
            if(selectById(table, param1, r)) resultRecords.append(r);
            break;
        }

        case DBSelector_SetProductByCode2:
        {
            DBTable* t = getTable(DBTABLENAME_PRODUCTS);
            QString sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::Code2), param1);
            sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            sql += QString(" ORDER BY %1 ASC").arg(t->columnName(ProductDBTable::Code2));
            executeSelectSQL(t, sql, resultRecords);
            break;
        }

        case DBSelector_GetShowcaseProducts: // Запрос списка товаров для отображения на экране Showcase:
        {
            for (int i = 0; i < appManager->showcase->count(); i++)
            {
                QString productCode = Showcase::getProductCode(appManager->showcase->getByIndex(i));
                DBRecord r;
                if (selectById(DBTABLENAME_PRODUCTS, productCode, r) && ProductDBTable::isForShowcase(r))
                    resultRecords.append(r);
            }
            const int sort = Tools::toInt(param1);
            const bool increase = Tools::toBool(param2);
            switch(sort)
            {
            case ShowcaseSort_Code: Tools::sortByInt(resultRecords, ProductDBTable::Code, increase); break;
            case ShowcaseSort_Code2: Tools::sortByInt(resultRecords, ProductDBTable::Code2, increase); break;
            case ShowcaseSort_Name: Tools::sortByString(resultRecords, ProductDBTable::UpperName, increase); break;
            }
            break;
        }

        case DBSelector_GetProductsByGroupCodeIncludeGroups: // Запрос списка товаров по коду группы включая группы:
        {
            DBTable* t = getTable(DBTABLENAME_PRODUCTS);
            QString sql;
            DBRecordList rs;
            // Группы:
            sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), param1);
            sql += QString(" AND %1 = '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::UpperName));
            executeSelectSQL(t, sql, rs);
            resultRecords.append(rs);
            // Товары:
            sql = QString("SELECT * FROM %1").arg(t->name);
            sql += QString(" WHERE %1 = '%2'").arg(t->columnName(ProductDBTable::GroupCode), param1);
            sql += QString(" AND %1 != '%2'").arg(t->columnName(ProductDBTable::Type), QString::number(ProductType_Group));
            sql += QString(" ORDER BY %1 ASC;").arg(t->columnName(ProductDBTable::Code));
            executeSelectSQL(t, sql, rs);
            resultRecords.append(rs);
            break;
        }

        case DBSelector_GetProductsByInputCode: // Запрос списка товаров по фрагменту кода/номера:
        {
            QString p = param1.trimmed();
            if (p.isEmpty()) break;
            const int field = Tools::toInt(param2) == ShowcaseSort_Code2 ? ProductDBTable::Code2 : ProductDBTable::Code;
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

        case DBSelector_GetProductsByFilteredCode:
        case DBSelector_GetProductsByFilteredCode2:
        case DBSelector_GetProductsByFilteredBarcode:
        case DBSelector_GetProductsByFilteredName: // Запрос списка товаров по фрагменту текста исключая группы:
        {
            QString p = param1.trimmed().toUpper();
            if (p.isEmpty()) break;
            SettingCode symbolsCode = SettingCode_SearchCodeSymbols;
            switch(selector)
            {
            case DBSelector_GetProductsByFilteredBarcode: symbolsCode = SettingCode_SearchBarcodeSymbols; break;
            case DBSelector_GetProductsByFilteredName:    symbolsCode = SettingCode_SearchNameSymbols; break;
            default: break;
            }
            if (!appManager->settings->getBoolValue(SettingCode_SearchType) &&
                    p.size() < appManager->settings->getIntValue(symbolsCode)) break;
            int field = ProductDBTable::Code;
            switch(selector)
            {
            case DBSelector_GetProductsByFilteredCode2:   field = ProductDBTable::Code2; break;
            case DBSelector_GetProductsByFilteredBarcode: field = ProductDBTable::Barcode; break;
            case DBSelector_GetProductsByFilteredName:    field = ProductDBTable::UpperName; break;
            default: break;
            }
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
        Tools::debugLog(QString("@@@@@ DataBase::select records = %1").arg(resultRecords.count()));
        emit selectResult(selector, resultRecords, true);
    });
}


