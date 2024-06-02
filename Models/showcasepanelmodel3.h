#ifndef SHOWCASEPANELMODEL3_H
#define SHOWCASEPANELMODEL3_H

#include "baselistmodel3.h"
#include "constants.h"

class ShowcasePanelModel3 : public BaseListModel3
{
    Q_OBJECT

public:
    explicit ShowcasePanelModel3(AppManager *parent): BaseListModel3(parent) {}
    QHash<int, QByteArray> roleNames() const override;
    void updateProducts(const DBRecordList&);
    void updateImages(const QStringList&);
    DBRecord& productByIndex(const int);

    int sort = ShowcaseSort_Name;
    bool increase = true;

private:
    DBRecordList products;
    DBRecord emptyRecord;
};

#endif // SHOWCASEPANELMODEL3_H
