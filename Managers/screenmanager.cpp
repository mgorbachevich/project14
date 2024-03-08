#include <QDebug>
#include "screenmanager.h"
#include "tools.h"

ScreenManager::ScreenManager(const QSize &size, QObject* parent): QObject(parent), screenSize(size)
{
    Tools::debugLog(QString("@@@@@ ScreenManager::ScreenManager %1 %2").arg(
                        QString::number(screenSize.width()), QString::number(screenSize.height())));
    double hk = ((double)screenSize.height()) / DEFAULT_SCREEN_HEIGHT;
    double wk = ((double)screenSize.width()) / DEFAULT_SCREEN_WIDTH;
    screenScale = wk < hk ? wk : hk;
}
