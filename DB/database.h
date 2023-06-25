#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "settings.h"

#define DB_VERSION "1.0"
#define DBTABLENAME_SHOWCASE "showcase"
#define DBTABLENAME_PRODUCTS "products"
#define DBTABLENAME_LABELFORMATS "labels"
#define DBTABLENAME_SETTINGS "settings"
#define DBTABLENAME_SETTINGGROUPS "settinggroups"
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
        GetCurrentProduct,
        GetUserNames,
        GetAuthorizationUserByName,
        GetSettingsItemByCode,
        GetSettings,
        GetLog,
        ReplaceSettingsItem,
    };

    explicit DataBase(const QString&, Settings&, QObject*);
    ~DataBase() { db.close(); }
    virtual bool start();
    DBTable* getTableByName(const QString&);
    void selectAll(DBTable*, DBRecordList&);
    bool removeAll(DBTable*);
    QString version();
    void saveLog(const int, const int, const QString&);
    bool insertRecord(DBTable*, const DBRecord&);

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
    bool removeRecord(DBTable*, const DBRecord&);
    void clearLog();

    bool opened = false;
    QSqlDatabase db;

signals:
    void requestResult(const DataBase::Selector, const DBRecordList&, const bool);
    void started();
    void downloadFinished(const int);
    void downloadResult(const qint64, const QString&);
    void loadResult(const qint64, const QString&);

public slots:
    void onStart() { start(); }
    void onSelect(const DataBase::Selector, const QString&);
    void onSelectByList(const DataBase::Selector, const DBRecordList&);
    void onUpdateRecord(const DataBase::Selector, const DBRecord&);
    void onTransaction(const DBRecord&);
    void onLog(const int, const int, const QString&);
    void onUpload(const qint64, const QString&, const QString&);
    void onDownload(const qint64, const QString&);
};

#endif // DATABASE_H
