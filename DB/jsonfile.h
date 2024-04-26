#ifndef JSONFILE_H
#define JSONFILE_H

#include <QObject>
#include <QJsonObject>
#include <QVariantList>
#include "constants.h"

/*
{
    mainObjectName:
    {
        mainArrayName:
        [
            {
                item
            }
        ]
    }
}
*/
class JsonFile : public QObject
{
    Q_OBJECT

public:
    JsonFile(const QString& file, QObject *parent): QObject(parent), fileName(file) { }
    virtual bool read();
    virtual bool write();
    QString getAndClearMessage();
    virtual bool insertOrReplace(const QString&);

protected:
    virtual QJsonObject toJson();
    virtual QString toString();
    virtual bool parseDefault() { return true; }
    virtual void sort() {}
    DBRecordList parse(const QString&);

    QString fileName;
    QString message;
    DBRecordList items;
    QHash<int, QString> fields;
    QString itemArrayName;
    QString mainObjectName;
};

#endif // JSONFILE_H
