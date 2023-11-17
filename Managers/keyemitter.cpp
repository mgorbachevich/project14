#include "keyemitter.h"

void KeyEmitter::emitKey(Qt::Key k)
{
    appManager->onUserAction();
    QObject* receiver = QGuiApplication::focusObject();
    if(receiver != nullptr)
    {
        QKeyEvent pressEvent = QKeyEvent(QEvent::KeyPress, k, Qt::NoModifier, QKeySequence(k).toString());
        QKeyEvent releaseEvent = QKeyEvent(QEvent::KeyRelease, k, Qt::NoModifier);
        QCoreApplication::sendEvent(receiver, &pressEvent);
        QCoreApplication::sendEvent(receiver, &releaseEvent);
    }
}

void KeyEmitter::emitChar(QChar c)
{
    appManager->onUserAction();
    emit appManager->enterChar(c);
}
