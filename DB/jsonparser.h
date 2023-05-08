#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QObject>
#include <QString>
#include "constants.h"

class DataBase;
class DBTable;
class QJsonValue;
class QJsonObject;

class JSONParser
{
public:
    JSONParser() {}
    int parseAllTables(DataBase*, const QString&);
    DBRecordList parseTable(DBTable*, const QString&);

private:
    QJsonValue prepare(const QString &, bool*);
    DBRecordList parseTable(DBTable*, const QJsonArray&);
    void parseTableColumn(DBTable*, DBRecord&, const QJsonObject&, const int);
};

#endif // JSONPARSER_H
