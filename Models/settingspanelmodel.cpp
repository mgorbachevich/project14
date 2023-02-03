#include "settingspanelmodel.h"

void SettingsPanelModel::update()
{
    qDebug() << "@@@@@ SettingsPanelModel::update";
    clear();
    addItem("Name1", "Value1");
    addItem("Name2", "Value2");
    addItem("Name3", "Value3");
}
