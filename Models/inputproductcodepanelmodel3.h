#ifndef INPUTPRODUCTCODEPANELMODEL3_H
#define INPUTPRODUCTCODEPANELMODEL3_H

#include "baselistmodel3.h"
#include "constants.h"

class InputProductCodePanelModel3 : public BaseListModel3
{
    Q_OBJECT

public:
    explicit InputProductCodePanelModel3(AppManager *parent): BaseListModel3(parent) {}
    QHash<int, QByteArray> roleNames() const override;
    void update(const DBRecordList&);
};

#endif // INPUTPRODUCTCODEPANELMODEL3_H
