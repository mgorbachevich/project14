#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include "settings.h"
#include "dbtable.h"

#define DB_VERSION "1.4"

#define DBTABLENAME_SHOWCASE "showcase"
#define DBTABLENAME_PRODUCTS "products"
#define DBTABLENAME_LABELFORMATS "labels"
#define DBTABLENAME_MESSAGES "messages"
#define DBTABLENAME_MESSAGEFILES "messagefiles"
#define DBTABLENAME_PICTURES "pictures"
#define DBTABLENAME_MOVIES "movies"
#define DBTABLENAME_SOUNDS "sounds"
#define DBTABLENAME_USERS "users"
#define DBTABLENAME_LOG "log"
#define DBTABLENAME_TRANSACTIONS "transactions"
#define DBTABLENAME_SETTINGS "settings"

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
    DBSelector_GetProductsByFilteredBarcode,
    DBSelector_GetItemsByCodes,
    DBSelector_GetAuthorizationUsers,
    DBSelector_GetAuthorizationUserByName,
    DBSelector_GetSettingsItemByCode,
    DBSelector_UpdateSettingsOnStart,
    DBSelector_GetLog,
    DBSelector_RefreshCurrentProduct,
    DBSelector_ReplaceSettingsItem,
    DBSelector_ChangeSettings,
    DBSelector_SetProductByInputCode,
    DBSelector_GetProductsByInputCode,
    DBSelector_GetProductByInputCode,
};

class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(Settings&, QObject*);
    ~DataBase();
    DBTable* getTable(const QString&) const;
    QString version() { return DB_VERSION; }
    void afterNetAction();
    QString netUpload(const QString&, const QString&);
    QString netDelete(const QString&, const QString&);
    void netDownload(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount);
    QString getProductMessageById(const QString&);
    void saveLog(const int, const int, const QString&);
    bool insertSettingsRecord(const DBRecord&);
    void saveTransaction(const DBRecord&);
    void updateSettingsRecord(const DBSelector, const DBRecord&);
    void select(const DBSelector, const DBRecordList&);
    void select(const DBSelector, const QString&);
    void clearLog();
    bool isOpened() { return opened; }
    QString getAndClearMessage();
    QList<DBTable*> getTables() { return tables; };


protected:
    void startDB();
    bool open(QSqlDatabase&, const QString&);
    bool addAndOpen(QSqlDatabase&, const QString&, const bool open = true);
    bool createTable(const QSqlDatabase& db, DBTable*);
    void close(QSqlDatabase& db) { if(db.isOpen()) db.close(); }
    bool copyDBFiles(const QString&, const QString&);
    void removeTempDb();
    bool removeAll(const QSqlDatabase&, DBTable*);
    bool insertRecord(const QSqlDatabase&, DBTable*, const DBRecord&, const bool select = true);
    void selectAll(const QSqlDatabase&, DBTable*, DBRecordList&);
    bool selectById(const QSqlDatabase&, const QString&, const QString&, DBRecord&);
    bool selectById(const QSqlDatabase&, DBTable*, const QString&, DBRecord&);
    bool executeSQL(const QSqlDatabase&, const QString&, const bool log = true);
    bool executeSelectSQL(const QSqlDatabase&, DBTable*, const QString&, DBRecordList&);
    bool removeRecord(const QSqlDatabase&, DBTable*, const QString&);
    void removeOldLogRecords();

    Settings& settings;
    bool opened = false;
    QSqlDatabase productDB;
    QSqlDatabase tempDB;
    QSqlDatabase settingsDB;
    QSqlDatabase logDB;
    int removeOldLogRecordsCounter = 0;
    QString message;
    QList<DBTable*> tables;

signals:
    void requestResult(const DBSelector, const DBRecordList&, const bool);
    void started();

public slots:
    void onAppStart();
};

#endif // DATABASE_H
