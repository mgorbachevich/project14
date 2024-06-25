#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include "dbtable.h"
#include "externalmessager.h"

#define DB_VERSION "1.9"

enum DBSelector
{
    DBSelector_None = 0,
    DBSelector_GetShowcaseProducts,
    DBSelector_GetShowcaseResources,
    DBSelector_GetImageByResourceCode,
    DBSelector_GetMessageByResourceCode,
    DBSelector_GetProductsByGroupCodeIncludeGroups,
    DBSelector_GetProductsByFilteredCode,
    DBSelector_GetProductsByFilteredCode2,
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

    bool addProductToShowcase(const DBRecord&);
    void clearLog();
    QString getProductMessageById(const QString&);
    DBTable* getTable(const QString&) const;
    QList<DBTable*> getTables() { return tables; };
    bool isProductInShowcase(const DBRecord&);
    bool isStarted() { return started; }
    QString netDelete(const QString&, const QString&);
    void netDownload(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount);
    QString netUpload(const QString&, const QString&, const bool codesOnly = false);
    bool removeProductFromShowcase(const DBRecord&);
    void saveLog(const int, const int, const QString&);
    void saveTransaction(const DBRecord&);
    void select(const DBSelector, const DBRecordList&);
    void select(const DBSelector, const QString& param1, const QString& param2 = "");
    QString version() { return DB_VERSION; }
    static bool copyDBFile(const QString&, const QString&);
    static bool renameDBFile(const QString&, const QString&);
    static bool removeDBFile(const QString&);
    static bool isDBFileExists(const QString&);

private:
    bool addAndOpen(QSqlDatabase&, const QString&, const bool open = true);
    void close(QSqlDatabase& db) { if(db.isOpen()) db.close(); }
    bool createTable(const QSqlDatabase& db, DBTable*);
    bool executeSelectSQL(const QSqlDatabase&, DBTable*, const QString&, DBRecordList&);
    bool executeSQL(const QSqlDatabase&, const QString&);
    bool insertRecord(const QSqlDatabase&, DBTable*, const DBRecord&);
    bool isLogging(const int);
    bool open(QSqlDatabase&, const QString&);
    bool query(const QSqlDatabase&, const QString&, DBTable*, DBRecordList*);
    bool removeAll(const QSqlDatabase&, DBTable*);
    bool removeRecord(const QSqlDatabase&, DBTable*, const QString&);
    void selectAll(const QSqlDatabase&, DBTable*, DBRecordList&);
    QStringList selectAllCodes(const QSqlDatabase&, DBTable*);
    bool selectById(const QSqlDatabase&, const QString&, const QString&, DBRecord&);
    bool selectById(const QSqlDatabase&, DBTable*, const QString&, DBRecord&);

    bool started = false;
    QSqlDatabase productDB;
    QSqlDatabase logDB;
    int removeOldLogRecordsCounter = 0;
    QList<DBTable*> tables;

signals:
    void selectResult(const DBSelector, const DBRecordList&, const bool);
    void dbStarted();

public slots:
    void onAppStart();
};

#endif // DATABASE_H
