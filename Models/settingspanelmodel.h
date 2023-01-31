#ifndef SETTINGSPANELMODEL_H
#define SETTINGSPANELMODEL_H

#include "baselistmodel.h"

class SettingsPanelModel : public BaseListModel
{
    Q_OBJECT

public:
    SettingsPanelModel(QObject *parent): BaseListModel(parent) {}
    void update();
};

#endif // SETTINGSPANELMODEL_H
