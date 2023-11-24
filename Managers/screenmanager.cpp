#include <QDebug>
#include "screenmanager.h"

ScreenManager::ScreenManager(const QSize &size, QObject* parent): QObject(parent), screenSize(size)
{
    qDebug() << "@@@@@ ScreenManager::ScreenManager " << screenSize.width() << screenSize.height();
    double hk = ((double)screenSize.height()) / DEFAULT_SCREEN_HEIGHT;
    double wk = ((double)screenSize.width()) / DEFAULT_SCREEN_WIDTH;
    screenScale = wk < hk ? wk : hk;
}
