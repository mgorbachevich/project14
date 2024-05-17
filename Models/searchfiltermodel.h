#ifndef SEARCHFILTERMODEL_H
#define SEARCHFILTERMODEL_H

#include "baselistmodel.h"

class SearchFilterModel : public BaseListModel
{
    Q_OBJECT

public:
    enum FilterIndex
    {
        Code = 0,
        Number = 1,
        Barcode = 2,
        Name = 3
    };

    explicit SearchFilterModel(AppManager *parent): BaseListModel(parent) {}
    void update();

    FilterIndex index = FilterIndex::Code;
    QString filterValue = "";
};

#endif // SEARCHFILTERMODEL_H
