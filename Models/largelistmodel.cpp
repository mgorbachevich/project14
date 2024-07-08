#include "largelistmodel.h"
#include "tools.h"

bool LargeListModel::onFlickTo(const int row)
{
    if(isWaiting || row == descriptor.firstVisibleRow) return false;
    Tools::debugLog(log("@@@@@ LargeListModel::onFlickTo(1)"));
    if(row < descriptor.firstVisibleRow) // up
    {
        if(row >= 0)
        {
            descriptor.firstVisibleRow = row;
            Tools::debugLog(log("@@@@@ LargeListModel::onFlickTo(2)"));
            return false; // Загрузка не нужна
        }
        if(descriptor.firstLoadRow == 0)
        {
            descriptor.firstVisibleRow = 0;
            Tools::debugLog(log("@@@@@ LargeListModel::onFlickTo(3)"));
            return false; // Загрузка не нужна
        }
        descriptor.firstLoadRow += row;
        if(descriptor.firstLoadRow < 0) descriptor.firstLoadRow = 0;
        descriptor.firstVisibleRow = 0;
    }
    else if(row > descriptor.firstVisibleRow) // down
    {
        if(row + descriptor.visibleRowCount <= descriptor.loadRowCout)
        {
            descriptor.firstVisibleRow = row;
            Tools::debugLog(log("@@@@@ LargeListModel::onFlickTo(4)"));
            return false; // Загрузка не нужна
        }
        descriptor.firstLoadRow += row;
        descriptor.firstVisibleRow = 0;
    }
    Tools::debugLog(log("@@@@@ LargeListModel::onFlickTo(5)"));
    return true; // Загрузка нужна
}

QString LargeListModel::log(const QString& title)
{
    return QString("%1 visible: %2, %3, load: %4, %5, param: %6").arg(
                title,
                Tools::toString(descriptor.firstVisibleRow),
                Tools::toString(descriptor.visibleRowCount),
                Tools::toString(descriptor.firstLoadRow),
                Tools::toString(descriptor.loadRowCout),
                descriptor.param);
}

