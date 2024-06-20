#ifndef DBTABLE_H
#define DBTABLE_H

#include <QObject>
#include <QVariant>
#include "constants.h"
#include "dbtablecolumn.h"

class DataBase;
class QJsonObject;

class DBTable : public QObject
{
    Q_OBJECT

public:
    DBTable(const QString &name, QObject *parent): QObject(parent), name(name) {}
    static QString toJsonString(DBTable*, const DBRecord&);
    static QString toJsonString(DBTable*, const DBRecordList&);
    QString columnTitle(const int index) const;
    QString columnName(const int index) const;
    QString columnType(const int index) const;
    DBRecord createRecord(const QString code = "");
    void addColumn(const QString& title, const QString& name, const QString& type);
    virtual int columnCount() { return 0; }
    virtual const DBRecordList checkList(DataBase*, const DBRecordList&);
    virtual const DBRecord checkRecord(const DBRecord&);
    static bool isEqual(const DBRecord&, const DBRecord&);
    int columnIndex(const QString&);
    DBRecordList parse(const QString&);

    QString name;

protected:
    void parseColumn(DBRecord&, const QJsonObject&, const int);

    QList<DBTableColumn> columns;
};

#endif // DBTABLE_H
