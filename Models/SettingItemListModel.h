#ifndef SETTINGITEMLISTMODEL_H
#define SETTINGITEMLISTMODEL_H

#include "baselistmodel.h"
#include "tools.h"

class SettingItemListModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit SettingItemListModel(AppManager *parent): BaseListModel(parent) {}
    void update(const QStringList& values)
    {
        Tools::debugLog("@@@@@ SettingItemListModel::update ");
        setStringList(values);
    }
};

#endif // SETTINGITEMLISTMODEL_H
