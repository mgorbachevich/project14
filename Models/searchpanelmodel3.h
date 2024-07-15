#ifndef SEARCHPANELMODEL3_H
#define SEARCHPANELMODEL3_H

#include "largelistmodel.h"
#include "constants.h"

class SearchPanelModel3 : public LargeListModel
{
    Q_OBJECT

public:
    explicit SearchPanelModel3(const int maxRowCount, AppManager *parent):
        LargeListModel(maxRowCount, parent) {}

    Q_INVOKABLE bool onFlickTo(const int) override;
    QHash<int, QByteArray> roleNames() const override;
    DBRecord &productByIndex(const int);
    void setHierarchyRoot(const bool v) { if(v) groupHierarchy.clear(); }
    bool isHierarchyRoot();
    bool hierarchyUp();
    bool hierarchyDown(DBRecord&);
    QString hierarchyLastCode();
    QString hierarchyTitle();
    int productCount() { return products.count(); }
    void update(const DBRecordList&, const int);
    void refresh() { descriptor.reset(""); isRoot = true; }

    int filterIndex = SearchFilterIndex_Code;
    bool isHierarchy = true;
    bool isRoot = true;

private:
    DBRecordList products;
    DBRecordList groupHierarchy;
};

#endif // SEARCHPANELMODEL3_H
