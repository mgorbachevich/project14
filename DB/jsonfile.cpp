#include <QJsonDocument>
#include "jsonfile.h"
#include "tools.h"

JsonFile::JsonFile(const QString &file, AppManager* parent) : ExternalMessager(parent), fileName(file)
{
    Tools::debugLog("@@@@@ JsonFile::JsonFile " + fileName);
}

bool JsonFile::write()
{
    bool ok = Tools::writeTextFile(fileName, toString());
    Tools::debugLog(QString("@@@@@ JsonFile::write %1 %2").arg(fileName, Tools::productSortIncrement(ok)));
    wasRead = false;
    return ok;
}

QString JsonFile::toString()
{
    QJsonDocument doc(toJson());
    return doc.toJson(QJsonDocument::Indented);  // QJsonDocument::Compact or QJsonDocument::Indented
}


