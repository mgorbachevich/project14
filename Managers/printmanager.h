#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <QObject>
#include <QVariant>
#include "Slpa100u.h"
#include "constants.h"

class DataBase;

class PrintManager : public QObject
{
    Q_OBJECT

public:
    enum PrintParam
    {
        PrintParam_Error = 0,
    };

    explicit PrintManager(QObject*);
    void start();
    void stop();
    QString version();
    void print(DataBase*, const DBRecord&, const DBRecord&, const QString&, const QString&, const QString&);

private:
    Slpa100u* slpa = nullptr;
    bool started = false;
    int error = 0;

signals:
    void printed(const DBRecord&);
    void paramChanged(const int, const QString&, const QString&);

public slots:
    void onStatusChanged(uint16_t);
    void onErrorStatusChanged(int);
};

#endif // PRINTMANAGER_H
