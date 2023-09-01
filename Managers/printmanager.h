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
    explicit PrintManager(QObject*, const bool demo);
    int start(const QString&);
    void stop();
    QString version();
    void print(DataBase*, const DBRecord&, const DBRecord&, const QString&, const QString&, const QString&);
    bool isError() { return errorCode != 0 || isStateError(status); }
    bool isDemoMode() { return demoMode; }
    void feed();
    QString getErrorDescription(const int);

private:
    bool isFlag(uint16_t v, int shift) { return (v & (0x00000001 << shift)) != 0; }
    bool isStateError(uint16_t);

    Slpa100u* slpa = nullptr;
    bool started = false;
    int errorCode = 0;
    uint16_t status = 0;
    QString message;
    bool demoMode = false;

signals:
    void printed(const DBRecord&);
    void paramChanged(const int, const int);

public slots:
    void onStatusChanged(uint16_t);
    void onErrorStatusChanged(int);
};

#endif // PRINTMANAGER_H
