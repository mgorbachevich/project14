#include "showcasepanelmodel2.h"
#include "productdbtable.h"
#include "tools.h"

DBRecord& ShowcasePanelModel2::productByIndex(const int index)
{
    return index < products.count() ? products[index] : emptyRecord;
}

void ShowcasePanelModel2::updateProducts(const DBRecordList& newProducts)
{
    Tools::debugLog("@@@@@ ShowcasePanelModel2::updateProducts " + QString::number(newProducts.count()));
    products.clear();
    products.append(newProducts);
}

void ShowcasePanelModel2::updateImages(const QStringList& images)
{
    Tools::debugLog("@@@@@ ShowcasePanelModel2::updateImages " + QString::number(images.count()));
    beginResetModel();
    items.clear();
    for (int i = 0; i < products.count() && i < images.count(); i++)
    {
        QString image = images[i];
        QString name = image == DUMMY_IMAGE_FILE ? products[i][ProductDBTable::Name].toString() : "";
        Tools::debugLog(QString("@@@@@ ShowcasePanelModel2::updateImages %1 %2").arg(name, image));
        addItem(name, image);
    }
    endResetModel();
}

