#include "externalmessager.h"
#include "appmanager.h"

void ExternalMessager::showMessage(const QString& title, const QString& text)
{
    AppManager::instance().showMessage(title, text);
}

void ExternalMessager::showAttention(const QString& text)
{
    AppManager::instance().showAttention(text);
}

void ExternalMessager::showToast(const QString& text, const int delaySec)
{
    AppManager::instance().showToast(text, delaySec);
}
