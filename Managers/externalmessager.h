#ifndef EXTERNALMESSAGER_H
#define EXTERNALMESSAGER_H

#include <QObject>
#include "constants.h"
#include "loner.h"

class AppManager;

class ExternalMessager : public QObject, public Loner<ExternalMessager>
{
    Q_OBJECT
    friend class Loner<ExternalMessager>;

private:
    ExternalMessager(QObject *parent = nullptr) : QObject(parent) {}

public:
    void showAttention(const QString&);
    void showToast(const QString&, const int delaySec = SHOW_SHORT_TOAST_SEC);

protected:
    void showMessage(const QString&, const QString&);
};

#endif // EXTERNALMESSAGER_H



