#include "productpanelmodel.h"
#include "productdbtable.h"

void ProductPanelModel::update(const DBRecord& product, ProductDBTable* productTable)
{
    qDebug() << "@@@@@ ProductPanelModel::update " << product.at(ProductDBTable::Code).toString();
    QStringList ss;
    QString s;
    s += "<b>" + product.at(ProductDBTable::Name).toString() +"</b>";
    ss << s;
    ss << "";
    for (int i = 0; i < product.count() && i < productTable->columnCount(); i++)
    {
        switch(i)
        {
            case ProductDBTable::Code:
            case ProductDBTable::Barcode:
            case ProductDBTable::Price:
            {
                QString s;
                s += productTable->columnTitle(i)+ ":  ";
                s += product.at(i).toString();
                ss << s;
                break;
            }
        }
    }
    setStringList(ss);
}
