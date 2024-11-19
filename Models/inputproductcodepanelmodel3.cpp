#include <QTimer>
#include "inputproductcodepanelmodel3.h"
#include "productdbtable.h"
#include "tools.h"
#include "appmanager.h"

QHash<int, QByteArray> InputProductCodePanelModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "code";
    roles[SecondRole] = "name";
    return roles;
}

bool InputProductCodePanelModel3::onFlickTo(const int row)
{
    if(!LargeListModel::onFlickTo(row)) return false;
    isWaiting = true;
    appManager->updateInputCodeList();
    return true;
}

void InputProductCodePanelModel3::update(const DBRecordList &products)
{
    Tools::debugLog(log("@@@@@ InputProductCodePanelModel3::update(1)"));
    //if(products.count() <= 0) return;
    descriptor.loadRowCout = products.count();

    beginResetModel();
    items.clear();
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        QStringList data;
        if(appManager->status.showcaseLastSort == ShowcaseSort_Code2)
            data << "№" + ri[ProductDBTable::Code2].toString() << ri[ProductDBTable::Name].toString();
        else
            data << "#" + ri[ProductDBTable::Code].toString() << ri[ProductDBTable::Name].toString();
        addItem(data);
    }
    endResetModel();

    Tools::debugLog(log("@@@@@ InputProductCodePanelModel3::update(2)"));
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]() { isWaiting = false; });
}

