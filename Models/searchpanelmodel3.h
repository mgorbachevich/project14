#ifndef SEARCHPANELMODEL3_H
#define SEARCHPANELMODEL3_H

#include "baselistmodel3.h"
#include "constants.h"

class SearchPanelModel3 : public BaseListModel3
{
    Q_OBJECT

public:
    explicit SearchPanelModel3(AppManager *parent): BaseListModel3(parent) {}
    QHash<int, QByteArray> roleNames() const override;
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

#endif // SEARCHPANELMODEL3_H
