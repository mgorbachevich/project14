#ifndef HIERARCHYMODEL_H
#define HIERARCHYMODEL_H

#include <QList>
#include "constants.h"

class HierarchyModel
{
public:
    explicit HierarchyModel() {}
    DBRecord &productByIndex(const int);
    void setHierarchyRoot(const bool v) { if(v) groupHierarchy.clear(); }
    bool isHierarchyRoot();
    bool hierarchyUp();
    bool hierarchyDown(DBRecord&);
    bool hierarchyContains(DBRecord&);
    QString hierarchyLastCode();
    DBRecord hierarchyLastGroup();
    QString hierarchyTitle();
    int productCount() { return products.count(); }
    int hierarchyCount() { return groupHierarchy.count(); }
    virtual void refresh() { setHierarchyRoot(true); }
    const DBRecordList& getProducts() { return products; }

protected:
    DBRecordList products;
    DBRecordList groupHierarchy;
};

#endif // HIERARCHYMODEL_H
