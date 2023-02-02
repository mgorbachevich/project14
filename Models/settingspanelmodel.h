#ifndef SETTINGSPANELMODEL_H
#define SETTINGSPANELMODEL_H

#include "editlistmodel.h"

class SettingsPanelModel : public EditListModel
{
    Q_OBJECT

public:
    SettingsPanelModel(QObject *parent): EditListModel(parent) {}
    void update();
};

#endif // SETTINGSPANELMODEL_H
