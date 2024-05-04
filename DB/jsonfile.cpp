#include <QJsonDocument>
#include "jsonfile.h"
#include "tools.h"

JsonFile::JsonFile(const QString &file, AppManager* parent):
    QObject((QObject*)parent), appManager(parent), fileName(file)
{
    Tools::debugLog("@@@@@ JsonFile::JsonFile " + fileName);
}

QString JsonFile::getAndClearMessage()
{
    QString s = message;
    message = "";
    return s;
}

bool JsonFile::write()
{
    bool ok = Tools::writeTextFile(fileName, toString());
    Tools::debugLog(QString("@@@@@ JsonFile::write %1 %2").arg(fileName, Tools::boolToString(ok)));
    if(!ok) message = "Ошибка записи файла " + fileName;
    else message = "Файл записан " + fileName;
    return ok;
}

QString JsonFile::toString()
{
    QJsonDocument doc(toJson());
    return doc.toJson(QJsonDocument::Indented);  // QJsonDocument::Compact or QJsonDocument::Indented
}


