#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QHostInfo>
#include <QNetworkInterface>
#include "tools.h"
#include "constants.h"

QDir Tools::getDataStorageDir()
{
    QDir dir;
    QString s = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + "/" + DATA_STORAGE_SUBDIR;
    if(!dir.exists(s)) dir.mkdir(s);
    return dir;
}

bool Tools::fileExists(const QString &fileName)
{
    qDebug() << "@@@@@ Tools::fileExists " << fileName;

    return true; // todo
   // return QFileInfo::exists(fileName);
}

QString Tools::readTextFile(const QString &fileName)
{
    qDebug() << "@@@@@ Tools::readTextFile " << fileName;

    QFile f(fileName);
    if (!f.open(QFile::ReadOnly | QFile::Text))
        return "";
    QTextStream in(&f);
    return in.readAll();
}

QString Tools::jsonToString(const QJsonObject &jo)
{
    QJsonDocument jd(jo);
    return QString(jd.toJson());
}

QJsonObject Tools::stringToJson(const QString &s)
{
    return QJsonDocument::fromJson(s.toUtf8()).object();
}

int Tools::stringToInt(const QString &s, const int defaultValue)
{
    bool ok;
    int v = s.toInt(&ok);
    if (!ok)
        return defaultValue;
    return v;
}

double Tools::stringToDouble(const QString &s, const double defaultValue)
{
    bool ok;
    double v = s.toDouble(&ok);
    if (!ok)
        return defaultValue;
    return v;
}

double Tools::priceToDouble(const QString &dbPrice, const int pointPosition)
{
    double v = dbPrice.toDouble();
    for (int i = 0; i < pointPosition; i++)
        v /= 10;
    return v;
}

NetParams Tools::getNetParams()
{
    // https://stackoverflow.com/questions/13835989/get-local-ip-address-in-qt
    NetParams np;
    np.localHostName = QHostInfo::localHostName();
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
    qDebug() << "@@@@@ Tools::getNetParams " << np.localHostName << np.localMacAddress << np.localHostIP << np.localNetMask;
    return np;
}
