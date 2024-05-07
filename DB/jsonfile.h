#ifndef JSONFILE_H
#define JSONFILE_H

#include <QObject>
#include <QJsonObject>
#include <QVariantList>
#include "externalmessager.h"

class AppManager;

class JsonFile : public ExternalMessager
{
public:
    JsonFile(const QString&, AppManager*);
    virtual bool read() { return false; }
    virtual bool write();
    virtual void clear() {}

protected:
    virtual QJsonObject toJson() { return QJsonObject(); }
    virtual QString toString();

    QHash<int, QString> fields;
    QString fileName;
};

#endif // JSONFILE_H
