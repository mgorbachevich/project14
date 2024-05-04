#ifndef JSONARRAYFILE_H
#define JSONARRAYFILE_H

#include <QObject>
#include <QJsonObject>
#include <QVariantList>
#include "constants.h"
#include "jsonfile.h"

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
class JsonArrayFile : public JsonFile
{
public:
    JsonArrayFile(const QString&, AppManager*);
    virtual bool read();
    virtual bool write();
    virtual bool insertOrReplace(const QString&);
    virtual void clear() { items.clear(); }
    virtual DBRecordList getAll();
    int count() { return items.count(); }
    DBRecord get(const int i) { return i >= 0 && i < count() ? items[i] : DBRecord(); }
    virtual DBRecord* getByCode(const int);

protected:
    virtual QJsonObject toJson();
    virtual bool parseDefault() { return true; }
    virtual void sort() {}
    DBRecordList parse(const QString&);
    virtual int getIndex(const int);

    DBRecordList items;
    QString itemArrayName;
    QString mainObjectName;
};

#endif // JSONARRAYFILE_H
