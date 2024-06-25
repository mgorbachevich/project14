#include "showcase.h"
#include "tools.h"
#include "appmanager.h"

Showcase::Showcase(AppManager *parent): JsonArrayFile(SHOWCASE_FILE, parent)
{
    Tools::debugLog("@@@@@ Showcase::Showcase");
    mainObjectName = "data";
    itemArrayName = DBTABLENAME_SHOWCASE;
    fields.insert(ShowcaseField_Code, "code");
}

bool Showcase::insertOrReplaceRecord(const DBRecord& r)
{
    return JsonArrayFile::insertOrReplaceRecord(r) && write();
}

void Showcase::removeByCode(const QString& code)
{
    JsonArrayFile::removeByCode(code);
    write();
}

QString Showcase::getProductCode(const int i)
{
    if(i >= count()) return "";
    DBRecord& r = items[i];
    if(r.isEmpty()) return "";
    return items[i][ShowcaseField_Code].toString();
}

DBRecord Showcase::createRecord(const QString& code)
{
    DBRecord r;
    for (int i = 0; i < fields.count(); i++) r << QVariant("");
    r[ShowcaseField_Code] = code;
    return r;
}



