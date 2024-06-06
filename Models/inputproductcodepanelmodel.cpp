#include "inputproductcodepanelmodel.h"
#include "productdbtable.h"
#include "tools.h"

void InputProductCodePanelModel::update(const DBRecordList &products)
{
    Tools::debugLog("@@@@@ InputProductCodePanelModel::update");
    QStringList ss;
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        ss << "#" + ri[ProductDBTable::Code].toString() + LIST_ROW_DELIMETER + ri[ProductDBTable::Name].toString();
    }
    setStringList(ss);
}
