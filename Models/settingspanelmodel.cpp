#include "settingspanelmodel.h"

void SettingsPanelModel::update()
{
    qDebug() << "@@@@@ SettingsPanelModel::update";
    QStringList ss;
    ss << "Код" << "Штрихкод";
    setStringList(ss);
}
