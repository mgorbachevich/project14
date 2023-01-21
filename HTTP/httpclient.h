#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
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
    void getHTTPAnswer(const QString&);

public slots:
    void onReply(QNetworkReply*);
};

#endif // HTTPCLIENT_H
