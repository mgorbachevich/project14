#ifndef PRODUCTPANELMODEL_H
#define PRODUCTPANELMODEL_H

#include "baselistmodel.h"
#include "constants.h"

class ProductDBTable;

class ProductPanelModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit ProductPanelModel(QObject*, ProductDBTable*);
    void update();

    DBRecord product;

private:
    ProductDBTable* productTable;
};

#endif // PRODUCTPANELMODEL_H
