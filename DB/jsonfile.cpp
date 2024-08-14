#include <QJsonDocument>
#include "jsonfile.h"
#include "tools.h"

JsonFile::JsonFile(const QString &file, AppManager* parent) : ExternalMessager(parent), fileName(file)
{
    Tools::debugLog("@@@@@ JsonFile::JsonFile " + fileName);
}

bool JsonFile::write()
{
    bool ok = Tools::writeTextFile(fileName, Tools::toString(toJsonObject()));
    Tools::debugLog(QString("@@@@@ JsonFile::write %1 %2").arg(fileName, Tools::sortIncrement(ok)));
    wasRead = false;
    return ok;
}

