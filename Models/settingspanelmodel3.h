#ifndef SETTINGSPANELMODEL3_H
#define SETTINGSPANELMODEL3_H

#include "baselistmodel3.h"

class Settings;

class SettingsPanelModel3 : public BaseListModel3
{
    Q_OBJECT

public:
    explicit SettingsPanelModel3(QObject *parent): BaseListModel3(parent) {}
    QHash<int, QByteArray> roleNames() const override;
    void update(Settings&);
};

#endif // SETTINGSPANELMODEL3_H
