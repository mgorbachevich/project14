#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include "constants.h"
#ifdef HTTP_CLIENT
#include <QObject>
#include <QNetworkAccessManager>

class QNetworkReply;

class HTTPClient : public QObject
{
    Q_OBJECT

public:
    explicit HTTPClient(QObject *parent = nullptr);
    ~HTTPClient() {}

private:
    QNetworkAccessManager manager;

    void sendGet(QString);

signals:
    void newData(const QString&);

public slots:
    void onReply(QNetworkReply*);
};

#endif // HTTP_CLIENT
#endif // HTTPCLIENT_H
