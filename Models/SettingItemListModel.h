#ifndef SETTINGITEMLISTMODEL_H
#define SETTINGITEMLISTMODEL_H

#include "baselistmodel.h"

class SettingItemListModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit SettingItemListModel(AppManager *parent): BaseListModel(parent) {}
    void update(const QStringList& values)
    {
        qDebug() << "@@@@@ SettingItemListModel::update " << values;
        setStringList(values);
    }
};

#endif // SETTINGITEMLISTMODEL_H
