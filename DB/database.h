#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include "dbtable.h"
#include "externalmessager.h"

#define DB_VERSION "1.7"

#define DBTABLENAME_SHOWCASE "showcase"
#define DBTABLENAME_PRODUCTS "products"
#define DBTABLENAME_LABELFORMATS "labels"
#define DBTABLENAME_MESSAGES "messages"
#define DBTABLENAME_MESSAGEFILES "messagefiles"
#define DBTABLENAME_PICTURES "pictures"
#define DBTABLENAME_MOVIES "movies"
#define DBTABLENAME_SOUNDS "sounds"
#define DBTABLENAME_LOG "log"
#define DBTABLENAME_TRANSACTIONS "transactions"

enum DBSelector
{
    DBSelector_None = 0,
    DBSelector_GetShowcaseProducts,
    DBSelector_GetShowcaseResources,
    DBSelector_GetImageByResourceCode,
    DBSelector_GetMessageByResourceCode,
    DBSelector_GetProductsByGroupCode,
    DBSelector_GetProductsByGroupCodeIncludeGroups,
    DBSelector_GetProductsByFilteredCode,
    DBSelector_GetProductsByFilteredNumber,
    DBSelector_GetProductsByFilteredBarcode,
    DBSelector_GetProductsByFilteredName,
    DBSelector_GetItemsByCodes,
    DBSelector_GetAuthorizationUsers,
    DBSelector_GetLog,
    DBSelector_RefreshCurrentProduct,
    DBSelector_SetProductByInputCode,
    DBSelector_GetProductsByInputCode,
    DBSelector_GetProductByInputCode,
};

class DataBase : public ExternalMessager
{
    Q_OBJECT

public:
    explicit DataBase(AppManager*);
    ~DataBase();
    DBTable* getTable(const QString&) const;
    QString version() { return DB_VERSION; }
    void afterNetAction();
    QString netUpload(const QString&, const QString&);
    QString netDelete(const QString&, const QString&);
    void netDownload(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount);
    QString getProductMessageById(const QString&);
    void onParseSetRequest(const QString&);
    void saveLog(const int, const int, const QString&);
    void saveTransaction(const DBRecord&);
    void select(const DBSelector, const DBRecordList&);
    void select(const DBSelector, const QString&);
    void clearLog();
    bool isStarted() { return started; }
    QList<DBTable*> getTables() { return tables; };

protected:
    bool open(QSqlDatabase&, const QString&);
    bool addAndOpen(QSqlDatabase&, const QString&, const bool open = true);
    bool createTable(const QSqlDatabase& db, DBTable*);
    void close(QSqlDatabase& db) { if(db.isOpen()) db.close(); }
    bool copyDBFiles(const QString&, const QString&);
    void removeTempDb();
    bool removeAll(const QSqlDatabase&, DBTable*);
    bool insertRecord(const QSqlDatabase&, DBTable*, const DBRecord&);
    void selectAll(const QSqlDatabase&, DBTable*, DBRecordList&);
    bool selectById(const QSqlDatabase&, const QString&, const QString&, DBRecord&);
    bool selectById(const QSqlDatabase&, DBTable*, const QString&, DBRecord&);
    bool executeSQL(const QSqlDatabase&, const QString&, const bool log = true);
    bool executeSelectSQL(const QSqlDatabase&, DBTable*, const QString&, DBRecordList&);
    bool removeRecord(const QSqlDatabase&, DBTable*, const QString&);
    void removeOldLogRecords();

    bool started = false;
    QSqlDatabase productDB;
    QSqlDatabase tempDB;
    QSqlDatabase logDB;
    int removeOldLogRecordsCounter = 0;
    QList<DBTable*> tables;

signals:
    void requestResult(const DBSelector, const DBRecordList&, const bool);
    void dbStarted();

public slots:
    void onAppStart();
};

#endif // DATABASE_H
