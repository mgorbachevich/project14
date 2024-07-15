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
        QString topText;
        QString bottomText;
        QString image = images[i];
        QString code = "#" + pi[ProductDBTable::Code].toString();
        QString code2 = "№" + pi[ProductDBTable::Code2].toString();
        //QString barcode = pi[ProductDBTable::Barcode].toString();
        QString name = pi[ProductDBTable::Name].toString();

        switch(appManager->settings->getIntValue(SettingCode_ShowcaseProductTopText, true))
        {
        case ShowcaseProductText_Code:
            topText = code;
            break;
        case ShowcaseProductText_Code2:
            topText = code2;
            break;
        case ShowcaseProductText_Sort:
            topText = (appManager->status.lastProductSort == ShowcaseSort_Code2) ? code2 : code;
            break;
        case ShowcaseProductText_Name:
            topText = name.left(name.indexOf(" "));
            break;
        }

        switch(appManager->settings->getIntValue(SettingCode_ShowcaseProductBottomText, true))
        {
        case ShowcaseProductText_Code:
            bottomText = code;
            break;
        case ShowcaseProductText_Code2:
            bottomText = code2;
            break;
        case ShowcaseProductText_Sort:
            bottomText = (appManager->status.lastProductSort == ShowcaseSort_Code2) ? code2 : code;
            break;
        case ShowcaseProductText_Name:
            bottomText = name.left(name.indexOf(" "));
            break;
        }
        //if(image == DUMMY_IMAGE_FILE && bottomText.isEmpty()) bottomText = name.left(name.indexOf(" "));

        Tools::debugLog(QString("@@@@@ ShowcasePanelModel3::updateImages %1 %2 %3").arg(topText, bottomText, image));
        QStringList data;
        data << topText << bottomText << image;
        addItem(data);
    }
    endResetModel();
}

