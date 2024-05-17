#ifndef PRODUCTPANELMODEL_H
#define PRODUCTPANELMODEL_H

#include "baselistmodel.h"
#include "constants.h"

class ProductDBTable;

class ProductPanelModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit ProductPanelModel(AppManager *parent): BaseListModel(parent) {}
    void update(const DBRecord&, const QString&, ProductDBTable*);
};

#endif // PRODUCTPANELMODEL_H
