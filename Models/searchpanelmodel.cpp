#include "searchpanelmodel.h"
#include "productdbtable.h"
#include "appmanager.h"
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
        const QString& price = appManager->priceAsString(ri);
        switch(filterIndex)
        {
        case SearchFilterIndex_Code:
            ss << QString("#%1   %2   %3").arg(ri[ProductDBTable::Code].toString(), name, price);
            break;
        case SearchFilterIndex_Code2:
            ss << QString("№%1   %2   %3").arg(ri[ProductDBTable::Code2].toString(), name, price);
            break;
        case SearchFilterIndex_Barcode:
            ss << QString("%1   %2   %3").arg(ri[ProductDBTable::Barcode].toString(), name, price);
            break;
        case SearchFilterIndex_Name:
            ss << QString("#%1   %2   %3").arg( ri[ProductDBTable::Code].toString(), name, price);
            break;
        default: // Hierarchy
            if(ProductDBTable::isGroup(ri)) ss << QString("%1").arg(name);
            else ss << QString("#%1   %2   %3").arg(ri[ProductDBTable::Code].toString(), name, price);
            break;
        }
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

