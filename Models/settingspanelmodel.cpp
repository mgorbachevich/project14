#include "settingspanelmodel.h"
#include "settings.h"
#include "tools.h"

void SettingsPanelModel::update(Settings& settings)
{
    Tools::debugLog("@@@@@ SettingsPanelModel::update " + Tools::intToString(settings.getCurrentGroupCode()));
    beginResetModel();
    items.clear();
    QList<int> groupItemCodes = settings.getCurrentGroupCodes();
    //qDebug() << "@@@@@ SettingsPanelModel::update groupItemCodes =" << groupItemCodes;
    for (int i = 0; i < groupItemCodes.count(); i++)
    {
        int itemCode = groupItemCodes[i];
        DBRecord* ri = settings.getByCode(itemCode);
        if(ri != nullptr)
        {
            QString color = "<font color='#263228'>";
            switch (settings.getType(*ri))
            {
            case SettingType_ReadOnly:
            case SettingType_Unsed:
            case SettingType_UnsedGroup:
                color = "<font color='#78909c'>";
                break;
            }
            addItem(color + settings.getName(*ri),
                    color + settings.getStringValue(*ri));
        }
    }
    endResetModel();
}

