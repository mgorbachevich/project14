#ifndef DBTABLE_H
#define DBTABLE_H

#include <QString>
#include <QStringList>
#include <QSqlQuery>
#include "constants.h"
#include "dbtablecolumn.h"

class DBTable : public QObject
{
    Q_OBJECT

public:
    DBTable(const QString& name, QObject *parent) : QObject(parent), name(name) {}
    static QString toJsonString(DBTable*, const DBRecord&);
    static QString toJsonString(DBTable*, const DBRecordList&);
    QString columnTitle(const int index) { return columns[index].title; }
    QString columnName(const int index) { return columns[index].name; }
    QString columnType(const int index) { return columns[index].type; }
    DBRecord createRecord();
    void addColumn(const QString& title, const QString& name, const QString& type);
    virtual int columnCount() { return 0; }
    virtual const DBRecordList checkList(const DBRecordList&);
    virtual const DBRecord checkRecord(const DBRecord&);
    static bool isEqual(const DBRecord&, const DBRecord&);

    QString name;

protected:
    QList<DBTableColumn> columns;
};

#endif // DBTABLE_H
