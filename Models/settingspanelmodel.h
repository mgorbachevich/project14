#ifndef SETTINGSPANELMODEL_H
#define SETTINGSPANELMODEL_H

#include "baselistmodel2.h"
#include "constants.h"

class SettingsPanelModel : public BaseListModel2
{
    Q_OBJECT

public:
    SettingsPanelModel(QObject *parent): BaseListModel2(parent) {}
    void update(const DBRecordList&);
};

#endif // SETTINGSPANELMODEL_H
