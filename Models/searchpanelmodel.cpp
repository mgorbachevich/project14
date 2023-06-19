#import "searchpanelmodel.h"
#import "productdbtable.h"

void SearchPanelModel::update(const DBRecordList &newProducts, const SearchFilterModel::FilterIndex filterIndex)
{
    qDebug() << "@@@@@ SearchPanelModel::update: index =" << filterIndex;
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
                //s += "<font color='" + titleColor + "'>" + ri.field(ProductDBTable::Code).toString() + " ";
                s += ri[ProductDBTable::Code].toString() + " ";
                break;
            case SearchFilterModel::Barcode:
                //s += "<font color='" + titleColor + "'>" + ri.field(ProductDBTable::Barcode).toString() + " ";
                s += ri[ProductDBTable::Barcode].toString() + " ";
                break;
        }
        //s += "<font color='" + valueColor + "'>" + ri.field(ProductDBTable::Name).toString();
        s += ri[ProductDBTable::Name].toString();
        ss << s;
    }
    setStringList(ss);
}

DBRecord& SearchPanelModel::productByIndex(const int index)
{
    return index < products.count() ? products[index] : emptyRecord;
}

