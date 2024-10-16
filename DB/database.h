#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include "dbtable.h"
#include "externalmessager.h"

#define DB_VERSION "1.10"

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
    DBSelector_GetLog,
    DBSelector_RefreshCurrentProduct,
    DBSelector_SetProductByCode,
    DBSelector_GetProductsByInputCode,
    DBSelector_GetProductByCode,
    DBSelector_SetProductByCode2,
    DBSelector_GetAllLabels,
    //DBSelector_GetAuthorizationUsers,
};

class DataBase : public ExternalMessager
{
    Q_OBJECT

public:
    explicit DataBase(AppManager*);
    ~DataBase();

    bool addProductToShowcase(const DBRecord&);
    void clearLog();
    QString getLabelPathByName(const QString&);
    QString getLabelPathById(const QString&);
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
    void select(const DBSelector, const QString& param1 = "", const QString& param2 = "", const int offset = -1, const int limit = -1);
    QString version() { return DB_VERSION; }
    static bool copyDBFile(const QString&, const QString&);
    static bool renameDBFile(const QString&, const QString&);
    static bool removeDBFile(const QString&);
    static bool isDBFileExists(const QString&);
    bool executeSQL(const QSqlDatabase&, const QString&);

private:
    bool addAndOpen(QSqlDatabase&, const QString&, const bool open = true);
    void close(QSqlDatabase& db) { if(db.isOpen()) db.close(); }
    bool createTable(DBTable*);
    bool executeSelectSQL(DBTable*, const QString&, DBRecordList&);
    bool insertRecord(DBTable*, const DBRecord&);
    bool insertRecords(DBTable*, const DBRecordList&);
    bool isLogging(const int);
    bool query(const QSqlDatabase&, const QString&, DBTable*, DBRecordList*);
    bool removeAll(DBTable*);
    bool removeRecord(DBTable*, const QString&);
    void selectAll(DBTable*, DBRecordList&);
    QStringList selectAllCodes(DBTable*);
    bool selectById(const QString&, const QString&, DBRecord&);

    bool started = false;
    int removeOldLogRecordsCounter = 0;
    QList<DBTable*> tables;
    QSqlDatabase productDB;
    QSqlDatabase logDB;

signals:
    void selectResult(const DBSelector, const DBRecordList&, const bool);
    void dbStarted();

public slots:
    void onAppStart();
};

#endif // DATABASE_H
