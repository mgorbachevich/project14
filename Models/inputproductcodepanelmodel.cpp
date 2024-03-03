#include "inputproductcodepanelmodel.h"
#include "productdbtable.h"

void InputProductCodePanelModel::update(const DBRecordList &products)
{
    qDebug() << "@@@@@ InputProductCodePanelModel::update";
    QStringList ss;
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        ss << ri[ProductDBTable::Code].toString() + PRODUCT_STRING_DELIMETER + ri[ProductDBTable::Name].toString();
    }
    setStringList(ss);
}
