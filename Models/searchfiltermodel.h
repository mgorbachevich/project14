#ifndef SEARCHFILTERMODEL_H
#define SEARCHFILTERMODEL_H

#include "baselistmodel.h"

class SearchFilterModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit SearchFilterModel(AppManager *);
};

#endif // SEARCHFILTERMODEL_H
