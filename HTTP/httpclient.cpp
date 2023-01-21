#include <QDebug>
#include <QNetworkReply>
#include <QUrl>
#include <QSslSocket>
#include "httpclient.h"
#include "constants.h"

HTTPClient::HTTPClient(QObject *parent): QObject(parent)
{
    // https://doc.qt.io/qt-6/android-openssl-support.html
    qDebug() << "@@@@@ HTTPClient::HTTPClient Device supports OpenSSL:" << QSslSocket::supportsSsl();
    connect(&manager, &QNetworkAccessManager::finished, this, &HTTPClient::onReply);

#ifdef HTTP_TEST
    sendGet("https://www.google.com"); // todo
#endif
}

void HTTPClient::sendGet(QString url)
{
    qDebug() << "@@@@@ HTTPClient::sendGet " << url;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    manager.get(request);
}

void HTTPClient::onReply(QNetworkReply *reply)
{
    // Получен ответ на запрос:
    if (reply == nullptr)
    {
        qDebug() << "@@@@@ HTTPClient::onReply ERROR null";
    }
    else  if (reply->error())
    {
        qDebug() << "@@@@@ HTTPClient::onReply ERROR " << reply->errorString();
    }
    else
    {
        QString answer = reply->readAll();
        qDebug() << "@@@@@ HTTPClient::onReply " << answer;
        emit getHTTPAnswer(answer);
    }
}
