#include "showcasedbtable.h"
#include "tools.h"

ShowcaseDBTable::ShowcaseDBTable(const QString &name, QObject *parent): DBTable(name, parent)
{
    Tools::debugLog("@@@@@ ShowcaseDBTable::ShowcaseDBTable");
    addColumn("Код", "code", "TEXT PRIMARY KEY");
}

DBRecord ShowcaseDBTable::createRecord(const QString& code)
{
    DBRecord r = DBTable::createRecord();
    r[ShowcaseDBTable::Code] = code;
    return r;
}

