#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "settings.h"
#include "dbtable.h"

#define DB_VERSION "1.1"
#define DBTABLENAME_SHOWCASE "showcase"
#define DBTABLENAME_PRODUCTS "products"
#define DBTABLENAME_LABELFORMATS "labels"
#define DBTABLENAME_SETTINGS "settings"
#define DBTABLENAME_TRANSACTIONS "transactions"
#define DBTABLENAME_MESSAGES "messages"
#define DBTABLENAME_MESSAGEFILES "messagefiles"
#define DBTABLENAME_PICTURES "pictures"
#define DBTABLENAME_MOVIES "movies"
#define DBTABLENAME_SOUNDS "sounds"
#define DBTABLENAME_USERS "users"
#define DBTABLENAME_LOG "log"

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

    explicit DataBase(const QString&, Settings&, QObject*);
    ~DataBase() { db.close(); }
    DBTable* getTableByName(const QString&) const;
    QString getProductMessageById(const QString&);
    void selectAll(DBTable*, DBRecordList&);
    bool removeAll(DBTable*);
    QString version();
    void saveLog(const int, const int, const QString&);
    bool insertRecord(DBTable*, const DBRecord&);
    QString uploadRecords(const QString&, const QString&);
    QString deleteRecords(const QString&, const QString&);
    //QString downloadRecords(const QString&, const QString&, const QByteArray&);
    void downloadRecords(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount);

    QString filePath;
    Settings& settings;
    QList<DBTable*> tables;

protected:
    void emulation();
    bool open();
    bool createTable(DBTable*);
    bool executeSQL(const QString&);
    bool executeSelectSQL(DBTable*, const QString&, DBRecordList&);
    bool selectById(DBTable*, const QString&, DBRecord&);
    bool selectById(const QString&, const QString&, DBRecord&);
    bool removeRecord(DBTable*, const QString&);
    void removeOldLogRecords();
    void downloadTableRecords(DBTable*, DBRecordList&, int&, int&);
    //int getRecordCount(DBTable*);

    bool opened = false;
    QSqlDatabase db;

signals:
    void requestResult(const DataBase::Selector, const DBRecordList&, const bool);
    void started();
    void updateDBFinished(const QString&);
    void showMessage(const QString&, const QString&);

public slots:
    void onStart();
    void onSelect(const DataBase::Selector, const QString&);
    void onSelectByList(const DataBase::Selector, const DBRecordList&);
    void onUpdateRecord(const DataBase::Selector, const DBRecord&);
    void onTransaction(const DBRecord&);
    void onLog(const int, const int, const QString&);
};

#endif // DATABASE_H
