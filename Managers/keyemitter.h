#ifndef KEYEMITTER_H
#define KEYEMITTER_H

#include <QObject>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QGuiApplication>
#include "appmanager.h"

class AppManager;

// https://stackoverflow.com/questions/62047184/how-to-simulate-key-pressed-event-in-qml

class KeyEmitter : public QObject
{
    Q_OBJECT

public:
    KeyEmitter(AppManager* app, QObject* parent = nullptr) : QObject(parent), appManager(app) {}

    Q_INVOKABLE void emitKey(Qt::Key);
    Q_INVOKABLE void emitChar(QChar);

private:
    AppManager* appManager;
};

#endif // KEYEMITTER_H
