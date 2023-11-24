#import "showcasepanelmodel.h"

void ShowcasePanelModel::updateProducts(const DBRecordList& newProducts)
{
    qDebug() << "@@@@@ ShowcasePanelModel::updateProducts " << newProducts.count();
    products.clear();
    products.append(newProducts);
}

void ShowcasePanelModel::updateImages(const QStringList& images)
{
    qDebug() << "@@@@@ ShowcasePanelModel::updateImages " << images.count();
    setStringList(images);
}

DBRecord &ShowcasePanelModel::productByIndex(const int index)
{
    return index < products.count() ? products[index] : emptyRecord;
}

