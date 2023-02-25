#include <QDebug>
#include <QNetworkReply>
#include <QUrl>
#include <QSslSocket>
#include "httpclient.h"
#include "constants.h"
#include "logdbtable.h"

HTTPClient::HTTPClient(QObject *parent): QObject(parent)
{
    qDebug() << "@@@@@ HTTPClient::HTTPClient";
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &HTTPClient::onReply, Qt::DirectConnection);
}

void HTTPClient::sendGet(QString url)
{
    qDebug() << "@@@@@ HTTPClient::sendGet " << url;
    QNetworkRequest request;
    // https://stackoverflow.com/questions/48107322/qt-http-post-way-protocol-is-unknown
    request.setUrl(QUrl::fromUserInput(url));
    //request.setUrl(QUrl(url));
    manager->get(request);
}

void HTTPClient::onReply(QNetworkReply *reply)
{
    // Получен ответ на запрос:
    if (reply == nullptr)
    {
        qDebug() << "@@@@@ HTTPClient::onReply ERROR null";
    }
    else
    {
        QString answer = reply->readAll();
        qDebug() << "@@@@@ HTTPClient::onReply " << answer;
#ifdef HTTP_CLIENT_TEST
        emit showMessageBox("Client-Server test", answer);
#endif
        if (reply->error())
        {
            qDebug() << "@@@@@ HTTPClient::onReply ERROR " << reply->errorString();
            emit log(LogDBTable::LogType_Error, "HTTPClient " + reply->errorString());
        }
        else
            emit newData(answer);
    }
}

