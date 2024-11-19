#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include "dbtable.h"
#include "externalmessager.h"

#define DB_VERSION "1.10"

class Users;
class Settings;

class DataBase : public ExternalMessager
{
    Q_OBJECT

public:
    explicit DataBase(AppManager*);
    ~DataBase();

    void clearLog();
    DBRecordList getAllLabels();
    //QString getLabelPathByName(const QString&);
    QString getLabelPathById(const QString&);
    QString getProductMessage(const DBRecord&);
    DBTable* getTable(const QString&) const;
    QList<DBTable*> getTables() { return tables; };
    bool isStarted() { return started; }
    QString netDelete(const QString&, const QString&);
    void netDownload(QHash<DBTable*, DBRecordList> records, int& successCount, int& errorCount);
    QString netUpload(Settings*, Users*, const QString&, const QString&, const bool codesOnly = false);
    void saveLog(const int, const int, const QString&);
    void saveTransaction(const DBRecord&);
    QString version() { return DB_VERSION; }
    static bool copyDBFile(const QString&, const QString&);
    static bool renameDBFile(const QString&, const QString&);
    static bool removeDBFile(const QString&);
    static bool isDBFileExists(const QString&);
    bool executeSQL(const QSqlDatabase&, const QString&);
    DBRecord selectByCode(const QString&, const QString&);
    DBRecordList selectShowcaseProducts(const DBRecord&, const bool);
    DBRecordList selectShowcaseResources(const DBRecordList&);
    DBRecordList selectLog();
    DBRecordList selectProductByNumber(const QString&);
    DBRecordList selectProductsByInputCode(const QString&, const QString&, const int, const int);
    DBRecordList selectProductsByGroup(const QString&, const bool includeGroups = true);
    DBRecordList selectProductsByFilter(const int, const QString&, const int, const int);

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
    DBRecordList selectSubgroups(const QString&);

    bool started = false;
    int removeOldLogRecordsCounter = 0;
    QList<DBTable*> tables;
    QSqlDatabase productDB;
    QSqlDatabase logDB;

signals:
    void dbStarted();

public slots:
    void onAppStart();
};

#endif // DATABASE_H
