#include "showcasepanelmodel3.h"
#include "productdbtable.h"
#include "tools.h"
#include "appmanager.h"
#include "settings.h"
#include "calculator.h"

QHash<int, QByteArray> ShowcasePanelModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "top";
    roles[SecondRole] = "bottom";
    roles[ThirdRole] = "image";
    roles[FourthRole] = "center";
    return roles;
}

void ShowcasePanelModel3::update(const DBRecordList& newProducts, const QStringList& images)
{
    Tools::debugLog("@@@@@ ShowcasePanelModel3::update " + QString::number(newProducts.count()));
    products.clear();
    products.append(newProducts);
    beginResetModel();
    items.clear();
    for (int i = 0; i < products.count() && i < images.count(); i++)
    {
        DBRecord& pi = products[i];
        QString topText;
        QString bottomText;
        QString centerText;
        QString code = "#" + pi[ProductDBTable::Code].toString();
        QString code2 = "№" + pi[ProductDBTable::Code2].toString();
        QString name = pi[ProductDBTable::Name].toString();
        //QString barcode = pi[ProductDBTable::Barcode].toString();

        if(Calculator::isGroup(pi)) centerText = "<b>" + name.toUpper() + "</b>";
        else
        {
            switch(AppManager::instance().settings->getIntValue(SettingCode_ShowcaseProductTopText, true))
            {
            case ShowcaseProductText_Code:
                topText = code;
                break;
            case ShowcaseProductText_Code2:
                topText = code2;
                break;
            case ShowcaseProductText_Sort:
                topText = (AppManager::instance().status.showcaseLastSort == ShowcaseSort_Code2) ? code2 : code;
                break;
            case ShowcaseProductText_Name:
                topText = name.left(name.indexOf(" "));
                break;
            }
            switch(AppManager::instance().settings->getIntValue(SettingCode_ShowcaseProductBottomText, true))
            {
            case ShowcaseProductText_Code:
                bottomText = code;
                break;
            case ShowcaseProductText_Code2:
                bottomText = code2;
                break;
            case ShowcaseProductText_Sort:
                bottomText = (AppManager::instance().status.showcaseLastSort == ShowcaseSort_Code2) ? code2 : code;
                break;
            case ShowcaseProductText_Name:
                bottomText = name.left(name.indexOf(" "));
                break;
            }
        }
        //if(image == DUMMY_IMAGE_FILE && bottomText.isEmpty()) bottomText = name.left(name.indexOf(" "));

        //Tools::debugLog(QString("@@@@@ ShowcasePanelModel3::update %1 %2 %3").arg(topText, bottomText, image));
        QStringList data;
        data << topText << bottomText << images[i] << centerText;
        addItem(data);
    }
    endResetModel();
}



