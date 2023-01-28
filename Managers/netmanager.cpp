#include <QDebug>
#include <QSslSocket>
#include "netmanager.h"
#include "httpclient.h"

NetManager::NetManager(QObject *parent) : QObject(parent)
{
    qDebug() << "@@@@@ NetManager::NetManager";

    // Поддержка SSL:
    // https://doc.qt.io/qt-6/android-openssl-support.html
    qDebug() << "@@@@@ NetManager::NetManager Device supports OpenSSL:" << QSslSocket::supportsSsl();

#ifdef HTTP_CLIENT
    // Инициализация HTTP клиента:
    HTTPClient* httpClient = new HTTPClient();
    httpClient->moveToThread(&netThread);
    connect(&netThread, &QThread::finished, httpClient, &QObject::deleteLater);
    connect(httpClient, &HTTPClient::newData, this, &NetManager::onNewData);
#endif

    netThread.start();
}

NetManager::~NetManager()
{
    netThread.quit();
    netThread.wait();
}

void NetManager::onNewData(const QString &json)
{
    emit newData(json);
}
