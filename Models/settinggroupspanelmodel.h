#ifndef SETTINGGROUPSPANELMODEL_H
#define SETTINGGROUPSPANELMODEL_H

#include "baselistmodel.h"

class Settings;

class SettingGroupsPanelModel : public BaseListModel
{
    Q_OBJECT

public:
    SettingGroupsPanelModel(AppManager *parent): BaseListModel(parent) {}
    void update(const Settings&);
};

#endif // SETTINGGROUPSPANELMODEL_H
