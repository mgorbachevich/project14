#include "settingspanelmodel.h"
#include "settingdbtable.h"
#include "settings.h"

void SettingsPanelModel::update(Settings& settings)
{
    qDebug() << "@@@@@ SettingsPanelModel::update currentGroupCode =" << settings.currentGroupCode;
    beginResetModel();
    items.clear();
    QList<int> groupItemCodes = settings.getCurrentGroupItemCodes();
    std::sort(groupItemCodes.begin(), groupItemCodes.end());
    qDebug() << "@@@@@ SettingsPanelModel::update groupItemCodes =" << groupItemCodes;
    for (int i = 0; i < groupItemCodes.count(); i++)
    {
        int itemCode = groupItemCodes[i];
        DBRecord* ri = settings.getItemByCode(itemCode);
        if(ri != nullptr)
            addItem(ri->at(SettingDBTable::Name).toString(), ri->at(SettingDBTable::Value).toString());
    }
    endResetModel();
}

