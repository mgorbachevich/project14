#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <QObject>
#include "Slpa100u.h"

class PrintManager : public QObject
{
    Q_OBJECT

public:
    explicit PrintManager(QObject*);
    void start();
    void stop();

private:
    Slpa100u* slpa = nullptr;
    bool started = false;

signals:
    void showMessageBox(const QString&, const QString&, const bool);
    void printed();
    void log(const int, const QString&);

public slots:
    void onSettingsChanged();
    void onPrint();
};

#endif // PRINTMANAGER_H
