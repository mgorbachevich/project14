#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
#include <QObject>
#include <QNetworkAccessManager>

class QNetworkReply;
class QNetworkAccessManager;

class HTTPClient : public QObject
{
    Q_OBJECT

public:
    explicit HTTPClient(QObject *parent = nullptr);
    ~HTTPClient() {}

private:
    QNetworkAccessManager* manager;

    void sendGet(QString);

signals:
    void showMessageBox(const QString&, const QString&);
    void newData(const QString&);
    void log(const int, const QString&);

public slots:
    void onReply(QNetworkReply*);
    void onSendGet(const QString& url) { sendGet(url); }
};

#endif // HTTPCLIENT_H
