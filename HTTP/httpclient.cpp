#include "httpclient.h"
#ifdef HTTP_CLIENT
#include <QDebug>
#include <QNetworkReply>
#include <QUrl>
#include <QSslSocket>

HTTPClient::HTTPClient(QObject *parent): QObject(parent)
{
    connect(&manager, &QNetworkAccessManager::finished, this, &HTTPClient::onReply);

#ifdef HTTP_CLIENT_TEST
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
        emit newData(answer);
    }
}
#endif // HTTP_CLIENT
