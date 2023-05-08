#include <QString>
#include <QTcpSocket>
#include <QDebug>
#include <QFile>
#include "socketthread.h"
#include "tools.h"

SocketThread::SocketThread(QObject* parent, qintptr descriptor, DataBase* dataBase) :
    QThread(parent), socketDescriptor(descriptor), db(dataBase)
{
    qDebug() << "@@@@@ SocketThread::SocketThread: descriptor =" << descriptor;
}

SocketThread::~SocketThread()
{
    qDebug() << "@@@@@ SocketThread::~SocketThread";
    if(socket != nullptr) delete socket;
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

void SocketThread::onDBRequestResult(const DataBase::Selector selector, const DBRecordList& records, const bool ok)
{
    qDebug() << "@@@@@ SocketThread::onDBRequestResult " << selector;
    if (!ok)
        return;

    switch(selector)
    {
    case DataBase::GetItemsByCodes:
        dbResponse.append(records);
        break;

    default:break;
    }
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
    qDebug() << "@@@@@ SocketThread::onAboutToClose request =" << request;
    switch (state)
    {
    case State::Upload:
    {
        qDebug() << "@@@@@ SocketThread::onAboutToClose: GET";
        const QString s1 = "source=";
        const QString s2 = "codes=";
        const QString s3 = "code=";
        QString codeList = "";
        QString source = "";
        DBTable* table = nullptr;
        int p1 = request.indexOf(s1);
        int p2 = request.indexOf("&");
        bool ok = p1 >= 0 && p2 > p1 && (request.contains(s2) || request.contains(s3));
        if(ok)
        {
            p1 += s1.length();
            source = request.mid(p1, p2 - p1);
            qDebug() << "@@@@@ SocketThread::onAboutToClose: source =" << source;
            table = db->getTableByName(source);
            ok = table != nullptr;
        }
        if(ok)
        {
            if (request.contains(s2))
            {
                int p3 = request.indexOf("[") + 1;
                int p4 = request.indexOf("]");
                codeList = request.mid(p3, p4 - p3);
            }
            else if (request.contains(s3))
            {
                int p3 = request.indexOf(s3);
                int p4 = request.length();
                p3 += s3.length();
                codeList = request.mid(p3, p4 - p3);
            }
            else ok = false;
        }
        if(ok)
        {
            emit selectFromDB(DataBase::GetItemsByCodes, source, codeList);

            // Wait for db response:
            int now = Tools::currentDateTimeToInt();
            while (dbResponse.count() > 0 && Tools::currentDateTimeToInt() < now + SOCKET_WAIT_FOR_RESPONSE_MSEC)
                QThread::msleep(SOCKET_WAIT_FOR_RESPONSE_SLEEP_MSEC);

            QString head = "HTTP/1.1 200 OK\r\n\r\n%1";
            QString body = "{\"result\":\"0\",\"description\":\"ошибок нет\",\"data\":{\"%1\":%2}}";
            QString response = head.arg(body.arg(source, DBTable::toJsonString(table, dbResponse)));
            qDebug() << "@@@@@ SocketThread::onAboutToClose: response =" << response;
            socket->write(response.toLatin1());
        }
        if (!ok) qDebug() << "@@@@@ SocketThread::onAboutToClose: Bad GET request";
        dbResponse.clear();
        break;
    }
    case State::Download:
    {
        qDebug() << "@@@@@ SocketThread::onAboutToClose: POST";
        int p1 = request.indexOf("{");
        int p2 = request.lastIndexOf("}") + 1;
        if (p1 >= 0 && p2 > p1)
        {
            int result = 0;
            QString description = "ошибок нет";
            QString json = request.mid(p1, p2 - p1);
            qDebug() << "@@@@@ SocketThread::onAboutToClose: json =" << json;
            emit download(json);
            QString head = "HTTP/1.1 200 OK\r\n\r\n%1";
            QString body =  QString("{\"result\":\"%1\",\"description\":\"%2\"}").arg(QVariant(result).toString(), description);
            QString response = head.arg(body);
            qDebug() << "@@@@@ SyncDataBase::onAboutToClose: response =" << response;
            socket->write(response.toLatin1());
        }
        else
            qDebug() << "@@@@@ SocketThread::onAboutToClose: Bad POST request";
        break;
    }
    default: break;
    }
    //socket->disconnectFromHost();
}

void SocketThread::onReadyRead()
{
    qDebug() << "@@@@@ SocketThread::onReadyRead";
    //int bytesAvailable = socket->bytesAvailable();
    //qDebug() << "@@@@@ SocketThread::onReadyRead: bytesAvailable =" << bytesAvailable;
    QString read = QString(socket->readAll());
    qDebug() << "@@@@@ SocketThread::onReadyRead: read =" << read;
    if (read.contains("GET") && read.contains("/getData"))
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: GET";
        state = State::Upload;
    }
    else if (read.contains("POST") && read.contains("/setData"))
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: POST";
        state = State::Download;
    }
    else if (state == State::Upload)
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: GET content";
    }
    else if (state == State::Download)
    {
        qDebug() << "@@@@@ SocketThread::onReadyRead: POST content";
    }
    else return;
    request += read;
}


