#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include "dbtable.h"
#include "jsonparser.h"

#define DB_HOSTNAME "ShtrihScaleDataBase"
#define DB_FILENAME "ShtrihScale.db"

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

class DataBase : public QObject
{
    Q_OBJECT

public:
    enum Selector
    {
        None = 0,
        ShowcaseProducts,
        ShowcaseResources,
        ImageByResourceCode,
        MessageByResourceCode,
        ProductsByGroupCode,
        ProductsByGroupCodeIncludeGroups,
        ProductsByFilteredCode,
        ProductsByFilteredCodeIncludeGroups,
        ProductsByFilteredBarcode,
        ProductsByFilteredBarcodeIncludeGroups,
        UserNames,
        AuthorizationUserByName,
    };

    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();
    DBTable* getTableByName(const QString&);
    void onTableParsed(DBTable*, const DBRecordList&);

    QList<DBTable*> tables;

private:
    bool open();
    bool executeSQL(const QString&);
    void executeSelectSQL(DBTable*, const QString&, DBRecordList&);
    bool createTable(DBTable*);
    bool selectById(DBTable*, const QString&, DBRecord&);
    void selectAll(DBTable*, DBRecordList&);
    void emulation();
    bool removeRecord(DBTable*, const DBRecord&);
    bool insertRecord(DBTable*, const DBRecord&);

    bool started = false;
    QSqlDatabase db;
    JSONParser parser;

signals:
    void selectResult(const DataBase::Selector, const DBRecordList&);
    void dbStarted();

public slots:
    void onStart();
    void onSelect(const DataBase::Selector, const QString&);
    void onSelectByList(const DataBase::Selector, const DBRecordList&);
    void onNewData(const QString&);
};

#endif // DATABASE_H

