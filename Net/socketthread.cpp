#include <QTcpSocket>
#include <QDebug>
#include "socketthread.h"

SocketThread::SocketThread(QObject* parent, qintptr descriptor, DataBase* dataBase) :
    QThread(parent), socketDescriptor(descriptor), db(dataBase)
{
    qDebug() << "@@@@@ SocketThread::SocketThread: descriptor =" << descriptor;
}

SocketThread::~SocketThread()
{
    qDebug() << "@@@@@ SocketThread::~SocketThread";
    if(socket != nullptr)
        delete socket;
}

void SocketThread::onDisconnected()
{
    qDebug() << "@@@@@ SocketThread::onDisconnected";
    socket->close();
    quit();
}

void SocketThread::run()
{
    qDebug() << "@@@@@ SocketThread::run";
    socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &SocketThread::onReadyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &SocketThread::onDisconnected, Qt::DirectConnection);
    exec();
}

void SocketThread::waitForResponse()
{
    QTime t = QTime::currentTime().addSecs(WAIT_FOR_RESPONSE_SEC);
    while (response.isEmpty() && QTime::currentTime() < t) { QThread::sleep(1); }
    qDebug() << "@@@@@ SocketThread::waitForResponse: response =" << response;
    socket->write(response.toLatin1());
    response = "";
}

void SocketThread::onReadyRead()
{
    qDebug() << "@@@@@ SocketThread::onReadyRead";
    QString request;
    while (socket->bytesAvailable() > 0)
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: bytesAvailable =" << socket->bytesAvailable();
        request += QString(socket->readAll());
    }
    qDebug() << "@@@@@ SocketThread::onReadyRead: request =" << request;

    if (request.contains("GET") && request.contains("/getData"))
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: GET";
        post = false;
        const QString s1 = "source=";
        const QString s2 = "codes=[";
        const QString s3 = "code=";
        int p1 = request.indexOf(s1);
        int p2 = request.indexOf("&");
        if (p1 >= 0 && p2 > p1)
        {
            if (request.contains(s2) || request.contains(s3))
            {
                p1 += s1.length();
                QString source = request.mid(p1, p2 - p1);
                QString param = "";
                DataBase::Selector selector = DataBase::None;
                if (source.contains("products"))
                    selector = DataBase::GetProductsByCodes;
                if (request.contains(s2))
                {
                    int p3 = request.indexOf(s2);
                    int p4 = request.indexOf("]");
                    p3 += s2.length();
                    param = request.mid(p3, p4 - p3);
                }
                else if (request.contains(s3))
                {
                    int p3 = request.indexOf(s3);
                    int p4 = request.length();
                    p3 += s3.length();
                    param = request.mid(p3, p4 - p3);
                }
                emit selectFromDB(selector, param);
                waitForResponse();
                //socket->disconnectFromHost();
            }
        }
        else
        {
            qDebug() << "@@@@@ SocketThread::onReadyRead: Bad request";
        }
    }
    else if (request.contains("POST") && request.contains("/setData"))
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: POST";
        post = true;
    }
    else if (post && request.contains("Content-Disposition"))
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: POST content";
        post = false;
        int p1 = request.indexOf("{") - 1;
        int p2 = request.lastIndexOf("}") + 1;
        if (p1 >= 0 && p2 > p1)
        {
            emit download(DataBase::Download, request.mid(p1, p2 - p1));
            waitForResponse();
            //socket->disconnectFromHost();
        }
        else
        {
            qDebug() << "@@@@@ SocketThread::onReadyRead: Bad request";
        }
    }
}

void SocketThread::onSelectFromDBResult(const DataBase::Selector selector, const DBRecordList& records)
{
    qDebug() << "@@@@@ SocketThread::onSelectFromDBResult " << selector;
    switch(selector)
    {
    case DataBase::GetProductsByCodes:
    {
        DBTable* table = db->getTableByName(DBTABLENAME_PRODUCTS);
        QString head = "HTTP/1.1 200 OK\r\n\r\n%1";
        QString body = "{\"result\":\"0\",\"description\":\"ошибок нет\",\"data\":{\"products\":%1}}";
        response = head.arg(body.arg(DBTable::toJsonString(table, records)));
        //qDebug() << "@@@@@ SocketThread::onSelectFromDBResult: response =" << response;
        break;
    }
    default:break;
    }
}

void SocketThread::onUpdateResult(const DataBase::Selector selector, const bool ok)
{
    qDebug() << "@@@@@ SocketThread::onUpdateResult " << selector;
    switch(selector)
    {
    case DataBase::Download:
    {
        QString head = "HTTP/1.1 200 OK\r\n\r\n%1";
        QString body = "{\"result\":\"0\",\"description\":\"ошибок нет\"}}";
        response = head.arg(body);
        qDebug() << "@@@@@ SocketThread::onUpdateResult: response =" << response;
        break;
    }
    default:break;
    }
}

