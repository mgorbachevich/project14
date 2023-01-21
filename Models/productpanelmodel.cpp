#include "productpanelmodel.h"
#include "productdbtable.h"

ProductPanelModel::ProductPanelModel(QObject *parent, ProductDBTable* productTable): BaseListModel(parent)
{
    this->productTable = productTable;
}

void ProductPanelModel::update()
{
    qDebug() << "@@@@@ ProductPanelModel::update " << product[ProductDBTable::Columns::Code].toString();
    QStringList ss;
    QString s;
    s += "<b>" + product[ProductDBTable::Columns::Name].toString() +"</b>";
    ss << s;
    ss << "";
    for (int i = 0; i < product.count() && i < productTable->columnCount(); i++)
    {
        switch(i)
        {
            case ProductDBTable::Columns::Code:
            case ProductDBTable::Columns::Barcode:
            case ProductDBTable::Columns::Price:
            {
                QString s;
                s += productTable->columnTitle(i)+ ":  ";
                s += product[i].toString();
                ss << s;
                break;
            }
        }
    }
    setStringList(ss);
}
