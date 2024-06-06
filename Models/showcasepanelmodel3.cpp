#include "showcasepanelmodel3.h"
#include "productdbtable.h"
#include "tools.h"
#include "appmanager.h"

QHash<int, QByteArray> ShowcasePanelModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "top";
    roles[SecondRole] = "bottom";
    roles[ThirdRole] = "image";
    return roles;
}

DBRecord& ShowcasePanelModel3::productByIndex(const int index)
{
    return index < products.count() ? products[index] : emptyRecord;
}

void ShowcasePanelModel3::updateProducts(const DBRecordList& newProducts)
{
    Tools::debugLog("@@@@@ ShowcasePanelModel3::updateProducts " + QString::number(newProducts.count()));
    products.clear();
    products.append(newProducts);
}

void ShowcasePanelModel3::updateImages(const QStringList& images)
{
    Tools::debugLog("@@@@@ ShowcasePanelModel3::updateImages");
    beginResetModel();
    items.clear();
    for (int i = 0; i < products.count() && i < images.count(); i++)
    {
        DBRecord& pi = products[i];
        QString top;
        QString bottom;
        QString image = images[i];
        QString code = "#" + pi[ProductDBTable::Code].toString();
        QString code2 = "№" + pi[ProductDBTable::Code2].toString();
        QString barcode = pi[ProductDBTable::Barcode].toString();
        QString name = pi[ProductDBTable::Name].toString();
        switch(appManager->settings->getIntValue(SettingCode_ShowcaseProductTopText, true))
        {
        case ShowcaseProductText_Code:
            top = code;
            break;
        case ShowcaseProductText_Code2:
            top = code2;
            break;
        case ShowcaseProductText_Sort:
            top = (sort == ShowcaseSort_Code2) ? code2 : code;
            break;
        case ShowcaseProductText_Name:
            top = name.left(name.indexOf(" "));
            break;
        }
        switch(appManager->settings->getIntValue(SettingCode_ShowcaseProductBottomText, true))
        {
        case ShowcaseProductText_Code:
            bottom = code;
            break;
        case ShowcaseProductText_Code2:
            bottom = code2;
            break;
        case ShowcaseProductText_Sort:
            bottom = (sort == ShowcaseSort_Code2) ? code2 : code;
            break;
        case ShowcaseProductText_Name:
            bottom = name.left(name.indexOf(" "));
            break;
        }
        if(image == DUMMY_IMAGE_FILE && top.isEmpty() && bottom.isEmpty())
            bottom = name.left(name.indexOf(" "));
        Tools::debugLog(QString("@@@@@ ShowcasePanelModel3::updateImages %1 %2 %3").arg(top, bottom, image));
        QStringList data;
        data << top << bottom << image;
        addItem(data);
    }
    endResetModel();
}

