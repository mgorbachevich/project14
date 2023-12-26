#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QHostInfo>
#include <QProcess>
#include <QNetworkInterface>
#include <QThread>
//#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "tools.h"
#include "constants.h"

QString Tools::readTextFile(const QString &fileName)
{
    qDebug() << "@@@@@ Tools::readTextFile " << fileName;
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly | QFile::Text)) return "";
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
    return ok ? v : defaultValue;
}

double Tools::stringToDouble(const QString &s, const double defaultValue)
{
    bool ok;
    double v = s.toDouble(&ok);
    return ok ? v : defaultValue;
}

double Tools::priceToDouble(const QString &dbPrice, const int pointPosition)
{
    double v = dbPrice.toDouble();
    for (int i = 0; i < pointPosition; i++) v /= 10;
    return v;
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

bool Tools::writeBinaryFile(const QString& filePath, const QByteArray& data)
{
    qDebug() << "@@@@@ Tools::writeBinaryFile name =" << filePath;
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        quint64 n1 = file.write(data);
        quint64 n2 = file.size();
        qDebug() << QString("@@@@@ Tools::writeBinaryFile OK %1 %2").arg(QString::number(n1), QString::number(n2));
        file.close();
        return true;
    }
    qDebug() << "@@@@@ Tools::writeBinaryFile ERROR";
    return false;
}

QByteArray Tools::readBinaryFile(const QString& path)
{
    qDebug() << "@@@@@ Tools::readBinaryFile name =" << path;
    QByteArray a;
    QFile file(path);
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

QString Tools::makeFullPath(const QString& subDir, const QString& localPath)
{
    //qDebug() << "@@@@@ Tools::makeFullPath";
    if(localPath.isEmpty()) return "";
    const QStringList dirs = QString(subDir + "/" + localPath).split("/");
    QDir dir;
    QString path = rootDir();
    for(int i = 0; i < dirs.size() - 1; i++)
    {
        if(dirs.at(i).isEmpty()) continue;
        path += "/" + dirs.at(i);
        if(!dir.exists(path))
        {
            bool ok = dir.mkdir(path);
            qDebug() << "@@@@@ Tools::makeFullPath mkdir " << ok << path;
        }
    }
    path += "/" + dirs.last(); // file name
    //qDebug() << "@@@@@ Tools::makeFullPath local path " << localPath;
    qDebug() << "@@@@@ Tools::makeFullPath full path " << path;
    return path;
}

bool Tools::isFileExistInDownloadPath(const QString &localPath)
{
    QString path = downloadFilePath(localPath);
    bool ok = false;
    if (QFileInfo::exists(path) && QFileInfo(path).isFile())
    {
        QFile file(path);
        if(file.open(QIODevice::ReadOnly))
        {
            file.close();
            ok = true;
        }
    }
    qDebug() << "@@@@@ Tools::isFileExistInDownloadPath " << localPath << ok;
    return ok;
}

QString Tools::qmlFilePath(const QString& localPath)
{
#ifdef Q_OS_ANDROID
    QString path = QString("file:%1/%2").arg(DOWNLOAD_SUBDIR, localPath).replace(":/", ":").replace("//", "/");
#else
    QString path = QString("file:///%1").arg(downloadFilePath(localPath));
#endif
    //qDebug() << "@@@@@ Tools::qmlFilePath " << path;
    return localPath.isEmpty() ? "" : path;
}

QString Tools::dataBaseFilePath(const QString& localPath)
{
    return makeFullPath("", localPath);
}

bool Tools::copyFile(const QString& from, const QString& to)
{
    qDebug() << "@@@@@ Tools::copyFile " << from << to;
    if (!QFile::exists(from)) return false;
    if (QFile::exists(to)) QFile::remove(to);
    return QFile::copy(from, to);
}

bool Tools::removeFile(const QString &path)
{
    qDebug() << "@@@@@ Tools::removeFile " << path;
    return QFile::exists(path) ? QFile::remove(path) : true;
}

QString Tools::downloadFilePath(const QString& localPath)
{
    return makeFullPath(DOWNLOAD_SUBDIR, localPath);
}

void Tools::pause(const int msec, const QString& comment)
{
    qDebug() << "@@@@@ Tools::pause " << msec << comment;
    QThread::currentThread()->msleep(msec);
}

QString Tools::rootDir()
{
    // https://doc.qt.io/qt-6/qstandardpaths.html#StandardLocation-enum
#ifdef Q_OS_ANDROID
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
    return app->applicationDirPath(); // Returns the directory that contains the application executable.
#endif
}

void Tools::sound(const QString& fileName, const int volume)
{
    auto mp = new QMediaPlayer;
    auto ao = new QAudioOutput;
    ao->setVolume(1.0f * volume / 100.0f); // 0 .. 1.0f
    mp->setAudioOutput(ao);
    mp->setSource(QUrl(fileName)); // qrc:/...
    mp->play();
    /*
    soundEffect.setSource(QUrl(fileName));
    soundEffect.setLoopCount(1);
    soundEffect.setVolume(volume); // 0..1
    soundEffect.play();
    */
}



