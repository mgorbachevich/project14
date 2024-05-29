#include "searchfiltermodel.h"
#include "tools.h"

SearchFilterModel::SearchFilterModel(AppManager *parent): BaseListModel(parent)
{
    Tools::debugLog("@@@@@ SearchFilterModel::SearchFilterModel");
    QStringList ss;
    ss << "Код" << "Номер" << "Штрихкод" << "Наименование";
    setStringList(ss);
}
