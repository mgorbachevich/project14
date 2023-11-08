#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QString>
#include "constants.h"

class DataBase;
class DBTable;
class QJsonObject;

class JSONParser
{
public:
    //static void parseAndSaveAllTables(DataBase*, const QString&, int* returnErrorCode = nullptr, QString* returnDescription = nullptr);
    static DBRecordList parseTable(DBTable*, const QString&, bool* ok = nullptr);

private:
    static void parseTableColumn(DBTable*, DBRecord&, const QJsonObject&, const int);
};

#endif // JSONPARSER_H
