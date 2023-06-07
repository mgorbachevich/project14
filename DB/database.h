#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "settings.h"

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
    DBTable* getTableByName(const QString&, const bool log = true);
    int insertRecords(DBTable*, const DBRecordList&, const bool log = true);
    void selectAll(DBTable*, DBRecordList&, const bool log = true);
    bool removeAll(DBTable*, const bool log = true);

    QString filePath;
    QList<DBTable*> tables;

protected:
    void emulation();
    bool open();
    bool createTable(DBTable*);
    bool executeSQL(const QString&, const bool log = true);
    bool executeSelectSQL(DBTable*, const QString&, DBRecordList&, const bool log = true);
    bool selectById(DBTable*, const QString&, DBRecord&, const bool log = true);
    bool removeRecord(DBTable*, const DBRecord&, const bool log = true);
    bool insertRecord(DBTable*, const DBRecord&, const bool log = true);
    void saveLog(const int type, const QString &comment, const bool reallySave = true);

    bool opened = false;
    QSqlDatabase db;
    Settings& settings;

signals:
    void requestResult(const DataBase::Selector, const DBRecordList&, const bool);
    void started();
    void downloadFinished(const int);
    void showMessageBox(const QString&, const QString&, const bool);
    void downloadResult(const qint64, const QString&);
    void loadResult(const qint64, const QString&);

public slots:
    void onStart() { start(); }
    void onSelect(const DataBase::Selector, const QString&);
    void onSelectByList(const DataBase::Selector, const DBRecordList&);
    void onUpdateRecord(const DataBase::Selector, const DBRecord&);
    void onPrinted(const DBRecord&);
    void onSaveLog(const int type, const QString &comment) { saveLog(type, comment); }
    void onUpload(const qint64, const QString&, const QString&);
    void onDownload(const qint64, const QString&);
};

#endif // DATABASE_H
