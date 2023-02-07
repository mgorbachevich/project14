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

DBRecord* SettingsPanelModel::getItemByIndex(const int index)
{
    return (index >= 0 && index < settings.count()) ? &settings[index] : nullptr;
}

DBRecord* SettingsPanelModel::getItemByCode(const int code)
{
    bool ok = false;
    for (int i = 0; i < settings.count(); i++)
    {
        DBRecord& ri = settings[i];
        if (ri[SettingDBTable::Columns::Code].toInt(&ok) == code && ok)
            return &ri;
    }
    return nullptr;
}
