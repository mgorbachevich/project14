#ifndef SHOWCASE_H
#define SHOWCASE_H

#include "jsonarrayfile.h"
#include "tools.h"

class Showcase : public JsonArrayFile
{
public:
    enum ShowcaseField
    {
        ShowcaseField_Code = 0,
        COLUMN_COUNT
    };

    Showcase(AppManager*);
    bool insertOrReplaceRecord(const DBRecord&);
    void removeByCode(const QString&);
    static QString getProductCode(const DBRecord& r) { return r.isEmpty() ? "" : r[ShowcaseField_Code].toString(); }
    QString getProductCode(const int);
    DBRecord createRecord(const QString&);

protected:
    void sort() { Tools::sortByString(items, ShowcaseField_Code); }
};

#endif // SHOWCASE_H
