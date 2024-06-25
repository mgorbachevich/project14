#include <QKeyEvent>
#include "keyemitter.h"
#include "tools.h"
#include "appmanager.h"

KeyEmitter::KeyEmitter(AppManager *parent) : QObject((QObject*)parent), appManager(parent)
{
   Tools::debugLog("@@@@@ KeyEmitter::KeyEmitter");
}

void KeyEmitter::emitKey(Qt::Key k)
{
    QObject* receiver = QGuiApplication::focusObject();
    if(receiver != nullptr)
    {
        appManager->onClick();
        emit enterKey(k);
        QKeyEvent pressEvent = QKeyEvent(QEvent::KeyPress, k, Qt::NoModifier, QKeySequence(k).toString());
        QKeyEvent releaseEvent = QKeyEvent(QEvent::KeyRelease, k, Qt::NoModifier);
        QCoreApplication::sendEvent(receiver, &pressEvent);
        QCoreApplication::sendEvent(receiver, &releaseEvent);
    }
}

void KeyEmitter::emitChar(QChar v)
{
    QObject* receiver = QGuiApplication::focusObject();
    if(receiver != nullptr)
    {
        appManager->onClick();
        emit enterChar(v);
    }
}

