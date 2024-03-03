#ifndef INPUTPRODUCTCODEPANELMODEL_H
#define INPUTPRODUCTCODEPANELMODEL_H

#include "baselistmodel.h"
#include "constants.h"

class InputProductCodePanelModel : public BaseListModel
{
    Q_OBJECT

public:
    explicit InputProductCodePanelModel(AppManager *parent): BaseListModel(parent) {}
    void update(const DBRecordList&);
};

#endif // INPUTPRODUCTCODEPANELMODEL_H



