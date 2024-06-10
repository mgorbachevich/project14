#include "searchpanelmodel3.h"
#include "productdbtable.h"
#include "appmanager.h"
#include "tools.h"

DBRecord emptyRecord;

QHash<int, QByteArray> SearchPanelModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "code";
    roles[SecondRole] = "name";
    roles[ThirdRole] = "price";
    return roles;
}

DBRecord& SearchPanelModel3::productByIndex(const int index)
{
    return index >= 0 && index < products.count() ? products[index] : emptyRecord;
}

bool SearchPanelModel3::isHierarchyRoot()
{
    return Tools::stringToInt(hierarchyLastCode()) == 0;
}

bool SearchPanelModel3::hierarchyUp()
{
    if (groupHierarchy.count() < 1) return false;
    groupHierarchy.removeLast();
    return true;
}

bool SearchPanelModel3::hierarchyDown(DBRecord& group)
{
    if (group.empty()) return false;
    for (int i = 0; i < groupHierarchy.count(); i++)
    {
        if (groupHierarchy[i][0] == group[0]) return false;
    }
    groupHierarchy.append(group);
    return true;
}

QString SearchPanelModel3::hierarchyLastCode()
{
    return groupHierarchy.empty() ? "0" : groupHierarchy.last()[ProductDBTable::Code].toString();
}

QString SearchPanelModel3::hierarchyTitle()
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

void SearchPanelModel3::update(const DBRecordList &newProducts, const int filterIndex)
{
    Tools::debugLog("@@@@@ SearchPanelModel3::update " + QString::number(filterIndex));
    items.clear();
    products.clear();
    products.append(newProducts);
    beginResetModel();
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        QString code;
        QString price;
        QString name = ri[ProductDBTable::Name].toString();
        switch(filterIndex)
        {
        case SearchFilterIndex_Code2:
            code = QString("№%1").arg(ri[ProductDBTable::Code2].toString());
            price = appManager->priceAsString(ri);
            break;
        case SearchFilterIndex_Barcode:
            code = QString("%1").arg(ri[ProductDBTable::Barcode].toString());
            price = appManager->priceAsString(ri);
            break;
        case SearchFilterIndex_Code:
        case SearchFilterIndex_Name:
        default: // Hierarchy
            if(!ProductDBTable::isGroup(ri))
            {
                code = QString("#%1").arg(ri[ProductDBTable::Code].toString());
                price = appManager->priceAsString(ri);
            }
            break;
        }
        Tools::debugLog(QString("@@@@@ SearchPanelModel3::update %1 %2 %3").arg(code, name, price));
        QStringList data;
        data << code << name << price;
        addItem(data);
    }
    endResetModel();
}

