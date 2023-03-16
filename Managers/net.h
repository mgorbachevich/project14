#ifndef NET_H
#define NET_H

#include <QObject>

class QThread;
class HTTPServer;
class DataBase;
class AppManager;

class Net : public QObject
{
    Q_OBJECT

public:
    Net(AppManager* app, QObject *parent = nullptr): QObject(parent), appManager(app) {}
    void startServer(const int);
    void stopServer();
    void startClient(DataBase*);
    void stopClient();

private:
    QThread* clientThread = nullptr;
    HTTPServer* server = nullptr;
    AppManager* appManager = nullptr;
};

#endif // NET_H
