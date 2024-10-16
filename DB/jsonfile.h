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
    virtual bool read() { wasRead = true; return false; }
    virtual bool write();
    virtual void clear() { wasRead = false; }
    virtual QJsonObject toJsonObject() { return QJsonObject(); }

protected:
    QHash<int, QString> fields;
    QString fileName;
    bool wasRead = false;
};

#endif // JSONFILE_H
