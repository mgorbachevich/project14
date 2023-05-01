#ifndef SETTINGSPANELMODEL_H
#define SETTINGSPANELMODEL_H

#include "baselistmodel2.h"

class Settings;

class SettingsPanelModel : public BaseListModel2
{
    Q_OBJECT

public:
    SettingsPanelModel(AppManager *parent): BaseListModel2(parent) {}
    void update(Settings&);
};

#endif // SETTINGSPANELMODEL_H
