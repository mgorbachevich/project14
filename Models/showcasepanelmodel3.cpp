#include "showcasepanelmodel3.h"
#include "productdbtable.h"
#include "tools.h"
#include "appmanager.h"

QHash<int, QByteArray> ShowcasePanelModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "title";
    roles[SecondRole] = "name";
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
        QString title;
        QString name;
        QString image = images[i];
        switch(appManager->settings->getIntValue(SettingCode_ShowcaseProductHeader, true))
        {
        case ShowcaseProductHeader_Code:
            title = pi[ProductDBTable::Code].toString();
            break;
        case ShowcaseProductHeader_Code2:
            title = pi[ProductDBTable::Code2].toString();
            break;
        case ShowcaseProductHeader_Barcode:
            title = pi[ProductDBTable::Barcode].toString();
            break;
        }
        if(image == DUMMY_IMAGE_FILE || appManager->settings->getBoolValue(SettingCode_ShowcaseProductName))
            name = pi[ProductDBTable::Name].toString();
        Tools::debugLog(QString("@@@@@ ShowcasePanelModel3::updateImages %1 %2 %3").arg(title, name, image));
        QStringList data;
        data << title << name.left(name.indexOf(" ")) << image;
        addItem(data);
    }
    endResetModel();
}

