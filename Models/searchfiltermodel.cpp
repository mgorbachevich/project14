#include "searchfiltermodel.h"
#include "tools.h"

void SearchFilterModel::update()
{
    Tools::debugLog("@@@@@ SearchFilterModel::update");
    QStringList ss;
    ss << "Код" << "Штрихкод";
    setStringList(ss);
}
