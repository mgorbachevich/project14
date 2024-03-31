#include "settingspanelmodel.h"
#include "settings.h"
#include "tools.h"

void SettingsPanelModel::update(Settings& settings)
{
    Tools::debugLog("@@@@@ SettingsPanelModel::update " + Tools::intToString(settings.currentGroupCode));
    beginResetModel();
    items.clear();
    settings.sort();
    QList<int> groupItemCodes = settings.getCurrentGroupCodes();
    //qDebug() << "@@@@@ SettingsPanelModel::update groupItemCodes =" << groupItemCodes;
    for (int i = 0; i < groupItemCodes.count(); i++)
    {
        int itemCode = groupItemCodes[i];
        DBRecord* ri = settings.getByCode(itemCode);
        if(ri != nullptr)
        {
            //qDebug() << "@@@@@ SettingsPanelModel::update addItem " << settings.getItemName(*ri) << settings.getItemStringValue(*ri);
            addItem(settings.getName(*ri), settings.getStringValue(*ri));
        }
    }
    endResetModel();
}

