#ifndef SHOWCASEPANELMODEL_H
#define SHOWCASEPANELMODEL_H

#include "baselistmodel.h"
#import "constants.h"

class ShowcasePanelModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit ShowcasePanelModel(AppManager *parent): BaseListModel(parent) {}
    void updateProducts(const DBRecordList&);
    void updateImages(const QStringList&);
    DBRecord& productByIndex(const int);

private:
    DBRecordList products;
};

#endif // SHOWCASEPANELMODEL_H
