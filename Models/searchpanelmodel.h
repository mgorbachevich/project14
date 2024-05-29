#ifndef SEARCHPANELMODEL_H
#define SEARCHPANELMODEL_H

#include "baselistmodel.h"
#include "constants.h"

class SearchPanelModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit SearchPanelModel(AppManager *parent): BaseListModel(parent) {}
    DBRecord &productByIndex(const int);
    void setHierarchyRoot(const bool v) { if(v) groupHierarchy.clear(); }
    bool isHierarchyRoot();
    bool hierarchyUp();
    bool hierarchyDown(DBRecord&);
    QString hierarchyLastCode();
    QString hierarchyTitle();
    int productCount() { return products.count(); }
    void update(const DBRecordList&, const int);

    int filterIndex = SearchFilterIndex_Code;
    bool isHierarchy = true;

private:
    DBRecordList products;
    DBRecordList groupHierarchy;
};

#endif // SEARCHPANELMODEL_H
