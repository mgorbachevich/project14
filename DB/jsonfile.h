#ifndef JSONFILE_H
#define JSONFILE_H

#include <QObject>
#include <QJsonObject>
#include <QVariantList>

class AppManager;

class JsonFile : public QObject
{
    Q_OBJECT

public:
    JsonFile(const QString&, AppManager*);
    QString getAndClearMessage();
    virtual bool read() { return false; }
    virtual bool write();
    virtual void clear() {}

protected:
    virtual QJsonObject toJson() { return QJsonObject(); }
    virtual QString toString();

    AppManager* appManager;
    QHash<int, QString> fields;
    QString fileName;
    QString message;
};

#endif // JSONFILE_H
