#include "settingspanelmodel3.h"
#include "settings.h"
#include "tools.h"

QHash<int, QByteArray> SettingsPanelModel3::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[FirstRole] = "name";
    roles[SecondRole] = "value";
    return roles;
}

void SettingsPanelModel3::update(Settings& settings)
{
    Tools::debugLog("@@@@@ SettingsPanelModel3::update " + Tools::intToString(settings.getCurrentGroupCode()));
    beginResetModel();
    items.clear();
    QList<int> groupItemCodes = settings.getCurrentGroupCodes();
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
            QStringList data;
            data << color + settings.getName(*ri) << color + settings.getStringValue(*ri);
            addItem(data);
        }
    }
    endResetModel();
}

