#ifndef ANDROIDACTIVITYRESULTRECEIVER_H
#define ANDROIDACTIVITYRESULTRECEIVER_H
#include "qglobal.h"

#ifdef Q_OS_ANDROID // --------------------------------------------------------
#include <QtCore/private/qandroidextras_p.h>

class AndroidActivityResultReceiver: public QAndroidActivityResultReceiver
{
public:
    AndroidActivityResultReceiver();
    void handleActivityResult(int, int, const QJniObject&);
};
#endif // Q_OS_ANDROID --------------------------------------------------------

#endif // ANDROIDACTIVITYRESULTRECEIVER_H
