#include <QRegularExpression>
#include <QTimer>
#include <QEventLoop>
#include "iohttp.h"

IoHttp::IoHttp(QObject *parent)
    : IoBase{parent}
{
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));
}

IoHttp::~IoHttp()
{
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));
    delete manager;
}

bool IoHttp::open(const QString &uri)
{
    if (!checkUri(uri)) return false;
    addr = uri;
    return true;
}

void IoHttp::close()
{
    addr.clear();
}

void IoHttp::clear()
{

}

bool IoHttp::write(const QByteArray &data)
{
    return false;
}

bool IoHttp::read(QByteArray &data, int64_t toRead, int timeoutRead)
{
    return false;
}

bool IoHttp::writeRead(const QByteArray &out, QByteArray &in, int64_t toRead, int timeoutRead)
{
    if (!isOpen())
    {
        error = -20;
        return false;
    }

    request.setAddress(addr + api);
    sender.setMaxWaitTime(timeoutRead);
    if (postCommand) in = sender.post(request);
                else in = sender.get(request);
    return sender.error() == Network::RequestSender::NoError;
}

bool IoHttp::isOpen()
{
    return !addr.isEmpty();
}

void IoHttp::setOption(const int index, const int option, const QString &s1, const QString &s2)
{
    switch (index)
    {
    case 0: postCommand = option != 0; break;
    case 1: api = s1; break;
    case 2:
        if (!option) request.removeParam("");
        else request.addParam(s1, s2);
        break;
    }
}

bool IoHttp::checkUri(const QString &uri)
{
    return uri.contains(QRegularExpression(getRegexStr(), QRegularExpression::CaseInsensitiveOption));
}

void IoHttp::managerFinished(QNetworkReply *reply)
{
    //setError(reply->error());
    if (reply->error() == QNetworkReply::NoError) {
        QString answer = reply->readAll();
        qDebug() << answer;
    }
    else qDebug() << reply->errorString();

}

QString IoHttp::getRegexStr()
{
    static QString RegexStr("^http:\\/\\/([\\w\\-\\/\\.]+)(\\:(\\d+))$");
    return RegexStr;
}

void IoHttp::setError(Network::RequestSender::RequestError e)
{
    switch (e)
    {
    case Network::RequestSender::NoError: error = 0; break;
    case Network::RequestSender::TimeoutError: error = -1; break;
    }
}
