#ifndef SEARCHPANELMODEL3_H
#define SEARCHPANELMODEL3_H

#include "hierarchymodel.h"
#include "largelistmodel.h"
#include "constants.h"

class SearchPanelModel3 : public LargeListModel, public HierarchyModel
{
    Q_OBJECT

public:
    explicit SearchPanelModel3(const int maxRowCount, AppManager *parent):
        LargeListModel(maxRowCount, parent) {}

    Q_INVOKABLE bool onFlickTo(const int) override;
    QHash<int, QByteArray> roleNames() const override;
    void update(const DBRecordList&, const int);
    void refresh() override { descriptor.reset(""); isRoot = true; }

    int filterIndex = SearchFilterIndex_Code;
    bool isHierarchy = true;
    bool isRoot = true;
};

#endif // SEARCHPANELMODEL3_H
