#ifndef SEARCHPANELMODEL_H
#define SEARCHPANELMODEL_H

#include "baselistmodel.h"
#import "searchfiltermodel.h"
#import "constants.h"

class SearchPanelModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit SearchPanelModel(AppManager *parent): BaseListModel(parent) {}
    void update(const DBRecordList&, const SearchFilterModel::FilterIndex);
    DBRecord &productByIndex(const int);

private:
    DBRecordList products;
};

#endif // SEARCHPANELMODEL_H
