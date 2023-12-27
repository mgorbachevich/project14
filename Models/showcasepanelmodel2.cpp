#import "showcasepanelmodel2.h"
#import "productdbtable.h"

DBRecord& ShowcasePanelModel2::productByIndex(const int index)
{
    return index < products.count() ? products[index] : emptyRecord;
}

void ShowcasePanelModel2::updateProducts(const DBRecordList& newProducts)
{
    qDebug() << "@@@@@ ShowcasePanelModel2::updateProducts " << newProducts.count();
    products.clear();
    products.append(newProducts);
}

void ShowcasePanelModel2::updateImages(const QStringList& images)
{
    qDebug() << "@@@@@ ShowcasePanelModel2::updateImages " << images.count();
    beginResetModel();
    items.clear();
    for (int i = 0; i < products.count() && i < images.count(); i++)
    {
        QString image = images[i];
        QString name = image == DUMMY_IMAGE_FILE ? products[i][ProductDBTable::Name].toString() : "";
        qDebug() << "@@@@@ ShowcasePanelModel2::updateImages " << name << image;
        addItem(name, image);
    }
    endResetModel();
}

