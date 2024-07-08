#ifndef INPUTPRODUCTCODEPANELMODEL3_H
#define INPUTPRODUCTCODEPANELMODEL3_H

#include "constants.h"
#include "largelistmodel.h"

class InputProductCodePanelModel3 : public LargeListModel
{
    Q_OBJECT

public:
    explicit InputProductCodePanelModel3(const int maxRowCount, AppManager *parent):
        LargeListModel(maxRowCount, parent) {}

    Q_INVOKABLE bool onFlickTo(const int) override;
    QHash<int, QByteArray> roleNames() const override;
    void update(const DBRecordList&);
};

#endif // INPUTPRODUCTCODEPANELMODEL3_H
