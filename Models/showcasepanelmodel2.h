#ifndef SHOWCASEPANELMODEL2_H
#define SHOWCASEPANELMODEL2_H

#include "baselistmodel2.h"
#import "constants.h"

class ShowcasePanelModel2 : public BaseListModel2
{
    Q_OBJECT

public:
    explicit ShowcasePanelModel2(AppManager *parent): BaseListModel2(parent) {}
    void updateProducts(const DBRecordList&);
    void updateImages(const QStringList&);
    DBRecord& productByIndex(const int);

private:
    DBRecordList products;
};

#endif // SHOWCASEPANELMODEL2_H