#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <QObject>
#include <QVariant>
#include "constants.h"
#include "settings.h"

class DataBase;
class Slpa100u;
class LabelCreator;

class PrintManager : public QObject
{
    Q_OBJECT

public:
    explicit PrintManager(QObject*, DataBase*, Settings&);
    ~PrintManager() { stop(); }
    int start(const QString&);
    void stop();
    QString version() const;
    void print(const DBRecord&, const DBRecord&, const QString&, const QString&, const QString&);
    bool isError() const { return errorCode != 0 || isStateError(status); }
    bool isDemoMode() const { return demo; }
    void feed();
    QString getErrorDescription(const int) const;

private:
    bool isFlag(uint16_t v, int shift) const { return (v & (0x00000001 << shift)) != 0; }
    bool isStateError(uint16_t) const;

    Slpa100u* slpa = nullptr;
    LabelCreator* labelCreator = nullptr;
    bool started = false;
    int errorCode = 0;
    uint16_t status = 0;
    QString message;
    DataBase* db;
    Settings& settings;
    bool demo = false;

signals:
    void printed(const DBRecord&);
    void paramChanged(const int, const int);

public slots:
    void onStatusChanged(uint16_t);
    void onErrorStatusChanged(int);
};

#endif // PRINTMANAGER_H
