#include <QDebug>
#include <QSslSocket>
#include "net.h"
#include "httpclient.h"
#include "httpserver.h"

Net::Net(QObject *parent) : QObject(parent)
{
    qDebug() << "@@@@@ Net::Net";

    // Поддержка SSL: https://doc.qt.io/qt-6/android-openssl-support.html
    qDebug() << "@@@@@ Net::Net Device supports OpenSSL:" << QSslSocket::supportsSsl();

#ifdef HTTP_CLIENT
    // Инициализация HTTP клиента:
    HTTPClient* httpClient = new HTTPClient();
    httpClient->moveToThread(&backThread);
    connect(&backThread, &QThread::finished, httpClient, &QObject::deleteLater);
    connect(httpClient, &HTTPClient::newData, this, &Net::onNewData);
#endif

#ifdef HTTP_SERVER
    server = new HTTPServer(this);
#endif



    backThread.start();
}

Net::~Net()
{
    backThread.quit();
    backThread.wait();
}

