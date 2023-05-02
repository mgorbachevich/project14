#include <QDebug>
#include <QHostInfo>
#include <QNetworkInterface>
#include "tcpserver.h"
#include "socketthread.h"
#include "logdbtable.h"

// https://www.youtube.com/watch?v=ab1GoP_W5vw
// https://www.youtube.com/watch?v=lwvtv9r8uaM
// https://evileg.com/ru/post/108/

void TCPServer::start(const int port)
{
    //while (isListening()) {}
    if (!listen(QHostAddress::Any, port))
    {
        qDebug() << "@@@@@ TCPServer::HTTPServer ERROR: " << errorString();
        emit log(LogDBTable::LogType_Error, "TCPServer " + errorString());
    }
}

void TCPServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "@@@@@ TCPServer::incomingConnection";
    SocketThread* thread = new SocketThread(this, socketDescriptor, db);
    connect(thread, &SocketThread::finished, thread, &QObject::deleteLater);
    connect(thread, &SocketThread::selectFromDB, db, &DataBase::onSelect);
    connect(thread, &SocketThread::download, db, &DataBase::onDownload);
    connect(db, &DataBase::dbResult, thread, &SocketThread::onDBResult);
    thread->start();
}

NetParams TCPServer::getNetParams()
{
    // https://stackoverflow.com/questions/13835989/get-local-ip-address-in-qt
    NetParams np;
    np.localHostName =  QHostInfo::localHostName();
    QList<QHostAddress> hosts = QHostInfo::fromName(np.localHostName).addresses();
    foreach (const QHostAddress& address, hosts)
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false)
            np.localHostIP = address.toString();
    }
    foreach (const QNetworkInterface& networkInterface, QNetworkInterface::allInterfaces())
    {
        foreach (const QNetworkAddressEntry& entry, networkInterface.addressEntries())
        {
            if (entry.ip().toString() == np.localHostIP)
            {
                np.localMacAddress = networkInterface.hardwareAddress();
                np.localNetMask = entry.netmask().toString();
                break;
            }
        }
    }
    qDebug() << "@@@@@ TCPServer::getNetParams " << np.localHostName << np.localMacAddress << np.localHostIP << np.localNetMask;
    return np;
}

