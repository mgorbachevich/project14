#include "inputproductcodepanelmodel3.h"
#include "productdbtable.h"
#include "tools.h"

QHash<int, QByteArray> InputProductCodePanelModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "code";
    roles[SecondRole] = "name";
    return roles;
}

void InputProductCodePanelModel3::update(const DBRecordList &products)
{
    Tools::debugLog("@@@@@ InputProductCodePanelModel3::update");
    beginResetModel();
    items.clear();
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        QStringList data;
        data << "#" + ri[ProductDBTable::Code].toString() << ri[ProductDBTable::Name].toString();
        addItem(data);
    }
    endResetModel();
}
