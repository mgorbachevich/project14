#include "externalmessager.h"
#include "appmanager.h"

ExternalMessager::ExternalMessager(AppManager* parent) : QObject{parent}, appManager(parent) {}

void ExternalMessager::showMessage(const QString& title, const QString& text)
{
    appManager->showMessage(title, text);
}

void ExternalMessager::showAttention(const QString& text)
{
    appManager->showAttention(text);
}
