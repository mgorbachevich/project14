#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QHostInfo>
#include <QProcess>
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

bool Tools::fileExistsInAppPath(const QString &fileAppPath)
{
    bool ok = QFileInfo::exists(fileAppPath) && QFileInfo(fileAppPath).isFile();
    qDebug() << "@@@@@ Tools::fileExistsInAppPath " << fileAppPath << ok;
    return ok;
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

/*
void Tools::memoryCheck()
{
#ifdef Q_OS_MAC
    QProcess p;
    p.start("sysctl", QStringList() << "kern.version" << "hw.physmem");
    p.waitForFinished();
    QString system_info = p.readAllStandardOutput();
    qDebug() << "@@@@@ Tools::memoryCheck " << system_info;
    p.close();
#endif
}
*/

bool Tools::writeBinaryFile(const QString& filePath, const QByteArray& data)
{
    qDebug() << "@@@@@ Tools::writeBinaryFile name =" << filePath;
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(data);
        file.close();
        qDebug() << "@@@@@ Tools::writeBinaryFile OK";
        return true;
    }
    qDebug() << "@@@@@ Tools::writeBinaryFile ERROR";
    return false;
}

QByteArray Tools::readBinaryFile(const QString& filePath)
{
    qDebug() << "@@@@@ Tools::readBinaryFile name =" << filePath;
    QByteArray a;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        a = file.readAll();
        file.close();
        qDebug() << "@@@@@ Tools::writeBinaryFile OK";
    }
    else
        qDebug() << "@@@@@ Tools::writeBinaryFile ERROR";
    return a;
}

QString Tools::qmlFilePath(const QString& subDir, const QString& filePath)
{
    if(filePath.isEmpty()) return "";
    QString path = "file:" + subDir + "/" + filePath;
    path.replace(":/", ":").replace("//", "/");
    qDebug() << "@@@@@ Tools::qmlFilePath path" << path;
    return path;
}

QString Tools::appFilePath(const QString& subDir, const QString& filePath)
{
    qDebug() << "@@@@@ Tools::dataFilePath";
    if(filePath.isEmpty()) return "";

    const QStringList dirs = QString(subDir + "/" + filePath).split("/");
    QDir dir;
    QString path = app->applicationDirPath();
    for(int i = 0; i < dirs.size() - 1; i++)
    {
        if(!dirs.at(i).isEmpty())
        {
            path += "/" + dirs.at(i);
            if(!dir.exists(path)) qDebug() << "@@@@@ Tools::dataFilePath mkdir " << dir.mkdir(path) << path;
        }
    }
    path += "/" + dirs.last(); // file name
    qDebug() << "@@@@@ Tools::dataFilePath filePath" << filePath;
    qDebug() << "@@@@@ Tools::dataFilePath path" << path;
    return path;
}


