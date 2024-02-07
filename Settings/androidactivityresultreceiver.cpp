#include "androidactivityresultreceiver.h"
#include "constants.h"

#ifdef Q_OS_ANDROID //--------------------------------------------------------
AndroidActivityResultReceiver::AndroidActivityResultReceiver()
{
    qDebug("@@@@@ AndroidActivityResultReceiver::AndroidActivityResultReceiver");
}

void AndroidActivityResultReceiver::handleActivityResult(int requestCode, int resultCode, const QJniObject &data)
{
    qDebug() << "@@@@@ AndroidActivityResultReceiver::handleActivityResult " << requestCode << resultCode << data.toString();
    switch (requestCode)
    {
    case SettingCode_WiFi:
        qDebug() << "@@@@@ AndroidActivityResultReceiver::handleActivityResult SettingCode_WiFi";
        break;
    case SettingCode_Ethernet:
    default:
        qDebug() << "@@@@@ AndroidActivityResultReceiver::handleActivityResult: Unknown requestCode";
        break;
    }
}
#endif // Q_OS_ANDROID --------------------------------------------------------
