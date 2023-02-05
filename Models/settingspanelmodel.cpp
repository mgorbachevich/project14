#include "settingspanelmodel.h"
#include "settingdbtable.h"

void SettingsPanelModel::update(const DBRecordList& newSettings)
{
    qDebug() << "@@@@@ SettingsPanelModel::update";
    beginResetModel();
    settings.clear();
    settings.append(newSettings);
    items.clear();
    for (int i = 0; i < settings.count(); i++)
    {
        DBRecord& ri = settings[i];
        addItem(ri[SettingDBTable::Columns::Name].toString(), ri[SettingDBTable::Columns::Value].toString());
    }
    endResetModel();
}

DBRecord SettingsPanelModel::getSettingsItem(const int index)
{
    return (index >= 0 && index < settings.count()) ? settings[index] : *new DBRecord();
}
