#ifndef DBTABLE_H
#define DBTABLE_H

#include <QString>
#include <QStringList>
#include <QSqlQuery>
#include "constants.h"
#include "dbtablecolumn.h"

class DataBase;

class DBTable : public QObject
{
    Q_OBJECT

public:
    DBTable(const QString& name, QObject *parent) : QObject(parent), name(name) {}
    static QString toJsonString(DBTable*, const DBRecord&);
    static QString toJsonString(DBTable*, const DBRecordList&);
    QString columnTitle(const int index) const;
    QString columnName(const int index) const;
    QString columnType(const int index) const;
    DBRecord createRecord();
    void addColumn(const QString& title, const QString& name, const QString& type);
    virtual int columnCount() { return 0; }
    virtual const DBRecordList checkList(DataBase*, const DBRecordList&);
    virtual const DBRecord checkRecord(const DBRecord&);
    static bool isEqual(const DBRecord&, const DBRecord&);
    int columnIndex(const QString&);

    QString name;

protected:
    QList<DBTableColumn> columns;
};

#endif // DBTABLE_H
