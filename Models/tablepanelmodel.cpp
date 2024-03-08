#include "tablepanelmodel.h"
#include "productdbtable.h"
#include "tools.h"

void TablePanelModel::update(const DBRecordList& newProducts)
{
    Tools::debugLog("@@@@@ TablePanelModel::update");
    products.clear();
    products.append(newProducts);
    QStringList ss;
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        bool isGroup = ProductDBTable::isGroup(ri);
        QString s;
        if(isGroup)
            s += "<i>";
        //s += "<font color='" + titleColor + "'>" + ri.field(ProductDBTable::Code).toString() + " ";
        //s += "<font color='" + valueColor + "'>" + ri.field(ProductDBTable::Name).toString();
        s += ri[ProductDBTable::Name].toString();
        if(isGroup)
            s += "</i>";
        ss << s;
    }
    setStringList(ss);
}

void TablePanelModel::root()
{
    groupHierarchy.clear();
}

bool TablePanelModel::groupUp()
{
    if (groupHierarchy.count() < 1) return false;
    groupHierarchy.removeLast();
    return true;
}

bool TablePanelModel::groupDown(DBRecord& group)
{
    if (group.empty()) return false;
    for (int i = 0; i < groupHierarchy.count(); i++)
    {
        if (groupHierarchy[i][0] == group[0]) return false;
    }
    groupHierarchy.append(group);
    return true;
}

QString TablePanelModel::lastGroupCode()
{
    Tools::debugLog("@@@@@ TablePanelModel::lastGroupCode");
    return groupHierarchy.empty() ? "0" : groupHierarchy.last()[ProductDBTable::Code].toString();
}

QString TablePanelModel::title()
{
    QString s = "/";
    for (int i = 0; i < groupHierarchy.count(); i++)
         s += " " + groupHierarchy[i][ProductDBTable::Name].toString() + " /";
    Tools::debugLog("@@@@@ TablePanelModel::title " + s);
    return s;
}

DBRecord &TablePanelModel::productByIndex(const int index)
{
    return index < products.count() ? products[index] : emptyRecord;
}
