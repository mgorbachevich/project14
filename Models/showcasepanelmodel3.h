#ifndef SHOWCASEPANELMODEL3_H
#define SHOWCASEPANELMODEL3_H

#include "baselistmodel3.h"
#include "constants.h"
#include "hierarchymodel.h"

class ShowcasePanelModel3 : public BaseListModel3, public HierarchyModel
{
    Q_OBJECT

public:
    explicit ShowcasePanelModel3(AppManager *parent): BaseListModel3(parent) {}
    QHash<int, QByteArray> roleNames() const override;
    void update(const DBRecordList&, const QStringList&);
};

#endif // SHOWCASEPANELMODEL3_H
