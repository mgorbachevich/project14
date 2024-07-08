#ifndef LARGELISTMODEL_H
#define LARGELISTMODEL_H

#include "baselistmodel3.h"
#include "listviewdescriptor.h"

class LargeListModel : public BaseListModel3
{
    Q_OBJECT

public:
    LargeListModel(const int maxRowCount, AppManager *parent): BaseListModel3(parent)
    {
        descriptor.visibleRowCount = maxRowCount;
    }

    Q_INVOKABLE virtual bool onFlickTo(const int);
    Q_INVOKABLE void onStart() { descriptor.firstLoadRow = descriptor.firstVisibleRow = 0; }
    int loadLimit() const { return descriptor.visibleRowCount * 3 / 2; }
    QString log(const QString&);

    ListViewDescriptor descriptor;

protected:
    bool isWaiting = false;
};

#endif // LARGELISTMODEL_H
