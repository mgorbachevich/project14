#include "resourcedbtable.h"
#include "tools.h"

ResourceDBTable::ResourceDBTable(const QString &name, QSqlDatabase &sqlDB, DataBase *parent):
    DBTable(name, sqlDB, parent)
{
    Tools::debugLog("@@@@@ ResourceDBTable::ResourceDBTable");

    addColumn("Код",          "code",   "UNSIGNED BIG INT PRIMARY KEY");
    addColumn("Наименование", "name",   "TEXT");
    addColumn("Значение",     "value",  "TEXT");
    addColumn("Хэш",          "hash",   "TEXT");
    addColumn("Источник",     "source", "TEXT");
}
