#include "searchpanelmodel.h"
#include "productdbtable.h"
#include "tools.h"

void SearchPanelModel::update(const DBRecordList &newProducts, const int filterIndex)
{
    Tools::debugLog("@@@@@ SearchPanelModel::update " + QString::number(filterIndex));
    products.clear();
    products.append(newProducts);
    QStringList ss;
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        const QString& name = ri[ProductDBTable::Name].toString();
        QString s;
        switch(filterIndex)
        {
        case SearchFilterIndex_Code:
            s += ri[ProductDBTable::Code].toString() + LIST_ROW_DELIMETER;
            s += name;
            break;
        case SearchFilterIndex_Number:
            s += ri[ProductDBTable::Code2].toString() + LIST_ROW_DELIMETER;
            s += name;
            break;
        case SearchFilterIndex_Barcode:
            s += ri[ProductDBTable::Barcode].toString() + LIST_ROW_DELIMETER;
            s += name;
            break;
        case SearchFilterIndex_Name:
            s += name;
            break;
        default: // Hierarchy
            s += ProductDBTable::isGroup(ri) ? QString("<i>%1</i>").arg(name) : name;
            break;
        }
        ss << s;
    }
    setStringList(ss);
}

DBRecord& SearchPanelModel::productByIndex(const int index)
{
    return index >= 0 && index < products.count() ? products[index] : emptyRecord;
}

bool SearchPanelModel::isHierarchyRoot()
{
    return Tools::stringToInt(hierarchyLastCode()) == 0;
}

bool SearchPanelModel::hierarchyUp()
{
    if (groupHierarchy.count() < 1) return false;
    groupHierarchy.removeLast();
    return true;
}

bool SearchPanelModel::hierarchyDown(DBRecord& group)
{
    if (group.empty()) return false;
    for (int i = 0; i < groupHierarchy.count(); i++)
    {
        if (groupHierarchy[i][0] == group[0]) return false;
    }
    groupHierarchy.append(group);
    return true;
}

QString SearchPanelModel::hierarchyLastCode()
{
    return groupHierarchy.empty() ? "0" : groupHierarchy.last()[ProductDBTable::Code].toString();
}


QString SearchPanelModel::hierarchyTitle()
{
    if(groupHierarchy.count() < 1) return "Все товары";
    QString s;
    for (int i = 0; i < groupHierarchy.count(); i++)
    {
        if (i > 0) s += " / ";
        s += groupHierarchy[i][ProductDBTable::Name].toString();
    }
    return s;
}

