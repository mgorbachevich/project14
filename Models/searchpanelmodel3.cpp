#include <QTimer>
#include "searchpanelmodel3.h"
#include "productdbtable.h"
#include "appmanager.h"
#include "tools.h"
#include "calculator.h"

QHash<int, QByteArray> SearchPanelModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "code";
    roles[SecondRole] = "name";
    roles[ThirdRole] = "price";
    return roles;
}

bool SearchPanelModel3::onFlickTo(const int row)
{
    if(!LargeListModel::onFlickTo(row)) return false;
    isWaiting = true;
    AppManager::instance().updateSearch();
    return true;
}

void SearchPanelModel3::update(const DBRecordList &newProducts, const int filterIndex)
{
    Tools::debugLog(log("@@@@@ SearchPanelModel3::update(1)"));
    items.clear();
    products.clear();
    products.append(newProducts);
    descriptor.loadRowCout = products.count();

    beginResetModel();
    for (int i = 0; i < products.count(); i++)
    {
        DBRecord ri = products[i];
        QString code;
        QString price;
        QString name = ri[ProductDBTable::Name].toString();
       if(Calculator::isGroup(ri)) name = "<b>" + name.toUpper() + "</b>";

        switch(filterIndex)
        {
        case SearchFilterIndex_Code2:
            code = QString("№%1").arg(ri[ProductDBTable::Code2].toString());
            price = Calculator::price(ri);
            break;
        case SearchFilterIndex_Barcode:
            code = QString("%1").arg(ri[ProductDBTable::Barcode].toString());
            price = Calculator::price(ri);
            break;
        case SearchFilterIndex_Code:
        case SearchFilterIndex_Name:
        default: // Hierarchy
            if(!Calculator::isGroup(ri))
            {
                code = QString("#%1").arg(ri[ProductDBTable::Code].toString());
                price = Calculator::price(ri);
            }
            break;
        }
        //Tools::debugLog(QString("@@@@@ SearchPanelModel3::update %1 %2 %3").arg(code, name, price));
        QStringList data;
        data << code << name << price;
        addItem(data);
    }
    endResetModel();

    Tools::debugLog(log("@@@@@ SearchPanelModel3::update(2)"));
    QTimer::singleShot(WAIT_DRAWING_MSEC, this, [this]() { isWaiting = false; });
}

void SearchPanelModel3::refresh()
{
    descriptor.reset("");
    isRoot = true;
}

