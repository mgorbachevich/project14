#include "hierarchymodel.h"
#include "tools.h"
#include "productdbtable.h"

DBRecord productEmptyRecord;

DBRecord& HierarchyModel::productByIndex(const int index)
{
    //Tools::debugLog("@@@@@ HierarchyModel::productByIndex");
    return index >= 0 && index < products.count() ? products[index] : productEmptyRecord;
}

bool HierarchyModel::isHierarchyRoot()
{
    //Tools::debugLog("@@@@@ HierarchyModel::isHierarchyRoot");
    return Tools::toInt(hierarchyLastCode()) == 0;
}

bool HierarchyModel::hierarchyUp()
{
    //Tools::debugLog("@@@@@ HierarchyModel::hierarchyUp");
    if (groupHierarchy.count() < 1) return false;
    groupHierarchy.removeLast();
    return true;
}

bool HierarchyModel::hierarchyDown(DBRecord& group)
{
    //Tools::debugLog("@@@@@ HierarchyModel::hierarchyDown");
    if (group.empty() || hierarchyContains(group)) return false;
    groupHierarchy.append(group);
    return true;
}

bool HierarchyModel::hierarchyContains(DBRecord& group)
{
    foreach (DBRecord gi, groupHierarchy) if (gi[0] == group[0]) return true;
    return false;
}

QString HierarchyModel::hierarchyLastCode()
{
    //Tools::debugLog("@@@@@ HierarchyModel::hierarchyLastCode");
    return groupHierarchy.empty() ? "0" : groupHierarchy.last()[ProductDBTable::Code].toString();
}

DBRecord HierarchyModel::hierarchyLastGroup()
{
    //Tools::debugLog("@@@@@ HierarchyModel::hierarchyLastGroup");
    return groupHierarchy.empty() ? productEmptyRecord : groupHierarchy.last();
}

QString HierarchyModel::hierarchyTitle()
{
    //Tools::debugLog("@@@@@ HierarchyModel::hierarchyTitle");
    if(groupHierarchy.count() < 1) return ALL_GOODS;
    QString s;
    for (int i = 0; i < groupHierarchy.count(); i++)
    {
        if (i > 0) s += " / ";
        s += groupHierarchy[i][ProductDBTable::Name].toString();
    }
    return s;
}


