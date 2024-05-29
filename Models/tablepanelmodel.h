#ifndef TABLEPANELMODEL_H
#define TABLEPANELMODEL_H

#include "baselistmodel.h"
#include "constants.h"

class TablePanelModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit TablePanelModel(AppManager *parent): BaseListModel(parent) {}
    void update(const DBRecordList&);

    void root();
    bool groupUp();
    bool groupDown(DBRecord&);
    QString lastGroupCode();
    QString title();
    int productCount() { return products.count(); }
    DBRecord& productByIndex(const int);

private:
    DBRecordList products;
    DBRecordList groupHierarchy;
 };

#endif // TABLEPANELMODEL_H
