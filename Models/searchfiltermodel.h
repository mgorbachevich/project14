#ifndef SEARCHFILTERMODEL_H
#define SEARCHFILTERMODEL_H

#include "baselistmodel.h"
#include "tools.h"

class SearchFilterModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit SearchFilterModel(AppManager *parent): BaseListModel(parent)
    {
        Tools::debugLog("@@@@@ SearchFilterModel::SearchFilterModel");
        QStringList ss;
        ss << "Код #" << "Номер №" << "Штрихкод" << "Наименование";
        setStringList(ss);
    }
};

#endif // SEARCHFILTERMODEL_H
