#ifndef KEYEMITTER_H
#define KEYEMITTER_H

#include <QObject>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QGuiApplication>

class AppManager;

// https://stackoverflow.com/questions/62047184/how-to-simulate-key-pressed-event-in-qml

class KeyEmitter : public QObject
{
    Q_OBJECT

public:
    KeyEmitter(AppManager *parent);

    Q_INVOKABLE void emitKey(Qt::Key);
    Q_INVOKABLE void emitChar(QChar);

protected:
    AppManager* appManager;

signals:
    void enterChar(const QChar v);
    void enterKey(const Qt::Key);
};

#endif // KEYEMITTER_H
