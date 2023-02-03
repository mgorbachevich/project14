#include "settingspanelmodel.h"
#include "settingdbtable.h"

void SettingsPanelModel::update(const DBRecordList& records)
{
    qDebug() << "@@@@@ SettingsPanelModel::update";
    clear();
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord ri = records[i];
        addItem(ri[SettingDBTable::Columns::Name].toString(), ri[SettingDBTable::Columns::Value].toString());
    }
}
