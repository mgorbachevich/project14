#ifndef COMBOLISTMODEL_H
#define COMBOLISTMODEL_H

#include "baselistmodel.h"
#include "tools.h"

class ComboListModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit ComboListModel(AppManager *parent): BaseListModel(parent) {}
    void update(const QStringList& values)
    {
        Tools::debugLog("@@@@@ ComboListModel::update ");
        setStringList(values);
    }
};

#endif // COMBOLISTMODEL_H
