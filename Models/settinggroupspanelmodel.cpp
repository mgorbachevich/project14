#include "settinggroupspanelmodel.h"
#include "settinggroupdbtable.h"
#include "settings.h"

void SettingGroupsPanelModel::update(const Settings& settings)
{
    qDebug() << "@@@@@ SettingGroupsPanelModel::update";
    QStringList ss;
    for (int i = 0; i < settings.groups.count(); i++)
        ss << settings.groups.at(i)[SettingGroupDBTable::Name].toString();
    setStringList(ss);
}

