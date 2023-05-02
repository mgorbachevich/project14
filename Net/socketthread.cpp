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
    state = State::Disconnected;
    quit();
}

void SocketThread::onAboutToClose()
{
    qDebug() << "@@@@@ SocketThread::onAboutToClose";
    switch (state)
    {
    case State::Get:
    {
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
                waitForDBResponse();
                //socket->disconnectFromHost();
            }
            else
            {
                qDebug() << "@@@@@ SocketThread::onAboutToClose: Bad GET request";
            }
        }
        break;
    }
    case State::Post:
    {
        int p1 = request.indexOf("{");
        int p2 = request.lastIndexOf("}") + 1;
        if (p1 >= 0 && p2 > p1)
        {
            emit download(DataBase::Download, request.mid(p1, p2 - p1));
            waitForDBResponse();
            //socket->disconnectFromHost();
        }
        else
        {
            qDebug() << "@@@@@ SocketThread::onAboutToClose: Bad POST request";
        }
        break;
    }
    default: break;
    }
}

void SocketThread::run()
{
    qDebug() << "@@@@@ SocketThread::run";
    socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &SocketThread::onReadyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::disconnected, this, &SocketThread::onDisconnected, Qt::DirectConnection);
    connect(socket, &QTcpSocket::aboutToClose, this, &SocketThread::onAboutToClose, Qt::DirectConnection);
    exec();
}

void SocketThread::waitForDBResponse()
{
    qDebug() << "@@@@@ SocketThread::waitForDBResponse";
    QTime end = QTime::currentTime().addMSecs(WAIT_FOR_RESPONSE_MSEC);
    while (response.isEmpty() && QTime::currentTime() < end)
        QThread::msleep(WAIT_FOR_RESPONSE_SLEEP_MSEC);
    qDebug() << "@@@@@ SocketThread::waitForDBResponse: response =" << response;
    socket->write(response.toLatin1());
    response = "";
}

void SocketThread::onReadyRead()
{
    qDebug() << "@@@@@ SocketThread::onReadyRead";
    int bytesAvailable = socket->bytesAvailable();
    qDebug() << "@@@@@ SocketThread::onReadyRead: bytesAvailable =" << bytesAvailable;
    QString read = QString(socket->readAll());
    qDebug() << "@@@@@ SocketThread::onReadyRead: read =" << read;

    if (read.contains("GET") && read.contains("/getData"))
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: GET";
        state = State::Get;
        request += read;
    }
    else if (read.contains("POST") && read.contains("/setData"))
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: POST";
        state = State::Post;
        request += read;
    }
    else if (state == State::Post)
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: POST content";
        request += read;
    }
}

void SocketThread::onDBResult(const DataBase::Selector selector, const DBRecordList& records, const bool ok)
{
    qDebug() << "@@@@@ SocketThread::onDBResult " << selector;
    response = "";
    if (!ok)
    {
        qDebug() << "@@@@@ SocketThread::onDBResult: ERROR";
        return;
    }

    switch(selector)
    {
    case DataBase::GetProductsByCodes:
    {
        DBTable* table = db->getTableByName(DBTABLENAME_PRODUCTS);
        QString head = "HTTP/1.1 200 OK\r\n\r\n%1";
        QString body = "{\"result\":\"0\",\"description\":\"ошибок нет\",\"data\":{\"products\":%1}}";
        response = head.arg(body.arg(DBTable::toJsonString(table, records)));
        //qDebug() << "@@@@@ SocketThread::onDBResult: response =" << response;
        break;
    }

    case DataBase::Download:
    {
        QString head = "HTTP/1.1 200 OK\r\n\r\n%1";
        QString body = "{\"result\":\"0\",\"description\":\"ошибок нет\"}}";
        response = head.arg(body);
        qDebug() << "@@@@@ SocketThread::onDBResult: response =" << response;
        break;
    }

    default:break;
    }
}


