#ifndef SEARCHPANELMODEL_H
#define SEARCHPANELMODEL_H

#include "baselistmodel.h"
#include "searchfiltermodel.h"
#include "constants.h"

class SearchPanelModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit SearchPanelModel(AppManager *parent): BaseListModel(parent) {}
    void update(const DBRecordList&, const SearchFilterModel::FilterIndex);
    void update() { update(products, index); }
    DBRecord &productByIndex(const int);

private:
    SearchFilterModel::FilterIndex index = SearchFilterModel::Code;
    DBRecordList products;
};

#endif // SEARCHPANELMODEL_H
