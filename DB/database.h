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

class DataBase : public QObject
{
    Q_OBJECT

public:
    enum Selector
    {
        None = 0,
        GetShowcaseProducts,
        GetShowcaseResources,
        GetImageByResourceCode,
        GetMessageByResourceCode,
        GetProductsByGroupCode,
        GetProductsByGroupCodeIncludeGroups,
        GetProductsByFilteredCode,
        //GetProductsByFilteredCodeIncludeGroups,
        GetProductsByFilteredBarcode,
        //GetProductsByFilteredBarcodeIncludeGroups,
        GetItemsByCodes,
        GetUserNames,
        GetAuthorizationUserByName,
        GetSettingsItemByCode,
        UpdateSettings,
        GetLog,
        RefreshCurrentProduct,
        ReplaceSettingsItem,
        ChangeSettings,
    };

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
    void updateSettingsRecord(const DataBase::Selector, const DBRecord&);
    void select(const DataBase::Selector, const DBRecordList&);
    void select(const DataBase::Selector, const QString&);

    Settings& settings;
    QList<DBTable*> tables;

protected:
    bool init();
    bool open(QSqlDatabase&, const QString&);
    bool addAndOpen(QSqlDatabase&, const QString&, const bool open = true);
    bool createTable(const QSqlDatabase& db, DBTable*);
    void close(QSqlDatabase& db) { if(db.isOpen()) db.close(); }
    bool copyDBFiles(const QString&, const QString&);
    void removeTempDb();

    bool selectById(const QSqlDatabase&, const QString&, const QString&, DBRecord&);
    bool removeAll(const QSqlDatabase&, DBTable*);
    bool insertRecord(const QSqlDatabase&, DBTable*, const DBRecord&);
    void selectAll(const QSqlDatabase&, DBTable*, DBRecordList&);
    bool selectById(const QSqlDatabase&, DBTable*, const QString&, DBRecord&);
    bool executeSQL(const QSqlDatabase&, const QString&);
    bool executeSelectSQL(const QSqlDatabase&, DBTable*, const QString&, DBRecordList&);
    bool removeRecord(const QSqlDatabase&, DBTable*, const QString&);
    void removeOldLogRecords();

    bool opened = false;
    QSqlDatabase productDB;
    QSqlDatabase tempDB;
    QSqlDatabase settingsDB;
    QSqlDatabase logDB;

signals:
    void requestResult(const DataBase::Selector, const DBRecordList&, const bool);
    void started();
    void showMessage(const QString&, const QString&);

public slots:
    void onStart();
};

#endif // DATABASE_H
