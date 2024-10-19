#ifndef EXTERNALMESSAGER_H
#define EXTERNALMESSAGER_H

#include <QObject>
#include "constants.h"

class AppManager;

class ExternalMessager : public QObject
{
    Q_OBJECT

public:
    ExternalMessager(AppManager*);
    void showAttention(const QString&);
    void showToast(const QString&, const int delaySec = SHOW_SHORT_TOAST_SEC);

protected:
    void showMessage(const QString&, const QString&);

    AppManager* appManager;
};

#endif // EXTERNALMESSAGER_H
