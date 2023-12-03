#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include "settings.h"
#include "dbtable.h"

#define DB_VERSION "1.3"

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
    QString getProductMessageById(const QString&);
    void selectAll(DBTable*, DBRecordList&);
    bool removeAll(DBTable*);
    QString version() { return DB_VERSION; }
    void saveLog(const int, const int, const QString&);
    bool insertRecord(DBTable*, const DBRecord&);
    QString uploadRecords(const QString&, const QString&);
    QString deleteRecords(const QString&, const QString&);
    void downloadRecords(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount);
    bool selectById(DBTable*, const QString&, DBRecord&);
    void saveTransaction(const DBRecord&);
    void updateRecord(const DataBase::Selector, const DBRecord&);
    void onSelectByList(const DataBase::Selector, const DBRecordList&);
    void selectByParam(const DataBase::Selector, const QString&);
    void onDownloadFinished();
    void onDeleteFinished();

    Settings& settings;
    QList<DBTable*> tables;

protected:
    bool init();
    bool open(QSqlDatabase&, const QString&);
    bool addAndOpen(QSqlDatabase&, const QString&, const bool open = true);
    bool createTable(DBTable*);
    bool executeSQL(const QSqlDatabase&, const QString&);
    bool executeSelectSQL(DBTable*, const QString&, DBRecordList&);
    bool selectById(const QString&, const QString&, DBRecord&);
    bool removeRecord(DBTable*, const QString&);
    void removeOldLogRecords();
    void removeTempDb();
    bool insertRecord(const QSqlDatabase&, DBTable*, const DBRecord&);
    void close(QSqlDatabase& db) { if(db.isOpen()) db.close(); }
    bool copyDBFiles(const QString&, const QString&);

    bool opened = false;
    QSqlDatabase productDB;
    QSqlDatabase tempDB;
    QSqlDatabase settingsDB;
    QSqlDatabase logDB;

signals:
    void requestResult(const DataBase::Selector, const DBRecordList&, const bool);
    void started();
    void deleteFinished();
    void downloadFinished();
    void showMessage(const QString&, const QString&);

public slots:
    void onStart();
};

#endif // DATABASE_H
