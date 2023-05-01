#ifndef VIEWLOGPANELMODEL_H
#define VIEWLOGPANELMODEL_H

#include "baselistmodel.h"
#include "constants.h"

class ViewLogPanelModel : public BaseListModel
{
    Q_OBJECT

public:
    ViewLogPanelModel(AppManager *parent): BaseListModel(parent) {}
    void update(const DBRecordList&);
};

#endif // VIEWLOGPANELMODEL_H
