#include <QDebug>
#include "screenmanager.h"
#include "tools.h"

ScreenManager::ScreenManager(const QSize &size, QObject* parent): QObject(parent), screenSize(size)
{
    const double w = screenSize.width();
    const double h = screenSize.height();
    const double wk = ((double)w) / DEFAULT_SCREEN_WIDTH;
    const double hk = ((double)h) / DEFAULT_SCREEN_HEIGHT;
    Tools::debugLog(QString("@@@@@ ScreenManager::ScreenManager %1 %2 %3 %4").arg(
                    Tools::intToString(w), Tools::intToString(h), Tools::intToString(wk), Tools::intToString(hk)));
    screenScale = wk < hk ? wk : hk;
}
