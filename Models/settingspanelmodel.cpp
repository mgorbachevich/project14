#include "settingspanelmodel.h"
#include "settingdbtable.h"

void SettingsPanelModel::update(DBRecordList* newSettings)
{
    qDebug() << "@@@@@ SettingsPanelModel::update";
    settings = newSettings;
    beginResetModel();
    items.clear();
    if (settings != nullptr)
    {
        for (int i = 0; i < settings->count(); i++)
        {
            const DBRecord& ri = settings->at(i);
            addItem(ri[SettingDBTable::Name].toString(), ri[SettingDBTable::Value].toString());
        }
    }
    endResetModel();
}

