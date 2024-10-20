#ifndef DBTABLE_H
#define DBTABLE_H

#include <QObject>
#include <QVariant>
#include "constants.h"
#include "dbtablecolumn.h"
#include "dbindexdescriptor.h"

class DataBase;
class QJsonObject;
class QSqlDatabase;

class DBTable : public QObject
{
    Q_OBJECT

public:
    DBTable(const QString &name, QSqlDatabase& sqlDB, DataBase *parent);
    static QVariant normalize(const DBRecord&, const int);
    static QString toJsonString(DBTable*, const DBRecord&);
    static QString toJsonString(DBTable*, const DBRecordList&);
    static QJsonObject toJsonObject(DBTable*, const DBRecord&);
    static QJsonObject toJsonObject(DBTable*, const DBRecordList&);
    QString columnTitle(const int) const;
    QString columnName(const int) const;
    QString columnType(const int) const;
    DBRecord createRecord(const QString code = "");
    void addColumn(const QString& title, const QString& name, const QString& type);
    virtual int columnCount() { return 0; }
    virtual const DBRecordList checkList(const DBRecordList&);
    virtual const DBRecord checkRecord(const DBRecord&);
    static bool isEqual(const DBRecord&, const DBRecord&);
    DBRecordList parse(const QString&);
    virtual void createIndexes();
    virtual void removeIndexes();
    virtual void setColumnNotUploadable(const int c) { notUploadColumns.append(c); }
    QString toString(DBRecord&);
    void fill(DBRecord&);

    QString name;
    QSqlDatabase& sqlDB;

protected:
    void parseColumn(DBRecord&, const QJsonObject&, const int);

    DataBase* db = nullptr;
    QList<DBTableColumn> columns;
    QList<DBIndexDescriptor> indexDescriptors;
    QList<int> notUploadColumns;
};

#endif // DBTABLE_H
