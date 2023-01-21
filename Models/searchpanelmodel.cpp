#import "searchpanelmodel.h"
#import "productdbtable.h"

void SearchPanelModel::update(const DBRecordList &newProducts, const SearchFilterModel::FilterIndex filterIndex)
{
    qDebug() << "@@@@@ SearchPanelModel::update";
    products.clear();
    products.append(newProducts);
    QStringList ss;
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        QString s;
        switch(filterIndex)
        {
            case SearchFilterModel::FilterIndex::Code:
                //s += "<font color='" + titleColor + "'>" + ri.field(ProductDBTable::Columns::Code).toString() + " ";
                s += ri[ProductDBTable::Columns::Code].toString() + " ";
                break;
            case SearchFilterModel::FilterIndex::Barcode:
                //s += "<font color='" + titleColor + "'>" + ri.field(ProductDBTable::Columns::Barcode).toString() + " ";
                s += ri[ProductDBTable::Columns::Barcode].toString() + " ";
                break;
        }
        //s += "<font color='" + valueColor + "'>" + ri.field(ProductDBTable::Columns::Name).toString();
        s += ri[ProductDBTable::Columns::Name].toString();
        ss << s;
    }
    setStringList(ss);
}

DBRecord& SearchPanelModel::productByIndex(const int index)
{
    return index < products.count() ? products[index] : *new DBRecord;
}

