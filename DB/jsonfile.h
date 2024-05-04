#ifndef JSONFILE_H
#define JSONFILE_H

#include <QObject>
#include <QJsonObject>
#include <QVariantList>
#include "constants.h"

class AppManager;
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
    JsonFile(const QString& file, AppManager *parent);
    virtual bool read();
    virtual bool write();
    QString getAndClearMessage();
    virtual bool insertOrReplace(const QString&);
    virtual void clear() { items.clear(); }
    virtual DBRecordList getAll();
    virtual int count() { return items.count(); }
    DBRecord get(const int i) { return i >= 0 && i < count() ? items[i] : DBRecord(); }
    virtual DBRecord* getByCode(const int);

protected:
    virtual QJsonObject toJson();
    virtual QString toString();
    virtual bool parseDefault() { return true; }
    virtual void sort() {}
    DBRecordList parse(const QString&);
    virtual int getIndex(const int);

    AppManager* appManager;
    QString fileName;
    QString message;
    DBRecordList items;
    QHash<int, QString> fields;
    QString itemArrayName;
    QString mainObjectName;
};

#endif // JSONFILE_H
