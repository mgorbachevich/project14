#ifndef IOHTTP_H
#define IOHTTP_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "iobase.h"
#include "RequestSender.h"

class IoHttp : public IoBase
{
    Q_OBJECT
public:
    explicit IoHttp(QObject *parent = nullptr);
    virtual ~IoHttp();

    virtual bool open(const QString &uri);
    virtual void close();
    virtual void clear();
    virtual bool write(const QByteArray &data);
    virtual bool read(QByteArray &data, int64_t toRead, int timeoutRead = 0);
    virtual bool writeRead(const QByteArray &out, QByteArray &in, int64_t toRead, int timeoutRead = 0);
    virtual bool isOpen();
    virtual void setOption(const int index, const int option, const QString &s1 = "", const QString &s2 = "");

    static bool checkUri(const QString &uri);

public slots:
    void managerFinished(QNetworkReply *reply);

private:
    bool postCommand = true;
    int timeout = 1000;
    QString addr;
    QString api;
    Network::Request request;
    QNetworkAccessManager *manager = nullptr;
    Network::RequestSender sender;
    static QString getRegexStr();
    void setError(Network::RequestSender::RequestError e);
};

#endif // IOHTTP_H
