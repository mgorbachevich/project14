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
    Tools::debugLog(QString("@@@@@ JsonFile::write %1 %2").arg(fileName, Tools::boolToString(ok)));
    if(!ok) showAttention("Ошибка записи файла " + fileName);
    else if(WRITE_CONFIG_FILE_MESSAGE) showAttention("Файл записан " + fileName);
    return ok;
}

QString JsonFile::toString()
{
    QJsonDocument doc(toJson());
    return doc.toJson(QJsonDocument::Indented);  // QJsonDocument::Compact or QJsonDocument::Indented
}


