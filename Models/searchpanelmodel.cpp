#include "searchpanelmodel.h"
#include "productdbtable.h"
#include "tools.h"

void SearchPanelModel::update(const DBRecordList &newProducts, const SearchFilterModel::FilterIndex filterIndex)
{
    Tools::debugLog("@@@@@ SearchPanelModel::update " + QString::number(filterIndex));
    index = filterIndex;
    products.clear();
    products.append(newProducts);
    QStringList ss;
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        QString s;
        switch(index)
        {
        case SearchFilterModel::Code:
            s += ri[ProductDBTable::Code].toString() + LIST_ROW_DELIMETER;
            break;
        case SearchFilterModel::Number:
            s += ri[ProductDBTable::Code2].toString() + LIST_ROW_DELIMETER;
            break;
        case SearchFilterModel::Barcode:
            s += ri[ProductDBTable::Barcode].toString() + LIST_ROW_DELIMETER;
            break;
        case SearchFilterModel::Name:
            break;
        }
         s += ri[ProductDBTable::Name].toString();
        ss << s;
    }
    setStringList(ss);
}

DBRecord& SearchPanelModel::productByIndex(const int index)
{
    return index < products.count() ? products[index] : emptyRecord;
}

