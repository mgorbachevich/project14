#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QHostInfo>
#include <QProcess>
#include <QNetworkInterface>
#include <QThread>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QApplication>
#include "tools.h"

quint64 soundTime = 0;
QMediaPlayer mediaPlayer;
QAudioOutput audioOutput;

void Tools::sound(const QString& fileName, const int volume)
{
    if(!mediaPlayer.isAvailable()) return;
    const quint64 now = Tools::currentDateTimeToUInt();
    if(now - soundTime < WAIT_SOUND_MSEC) return;
    soundTime = now;
    audioOutput.setVolume(1.0f * volume / 100.0f); // 0 .. 1.0f
    mediaPlayer.setAudioOutput(&audioOutput);
    mediaPlayer.setSource(QUrl(fileName)); // qrc:/...
    mediaPlayer.play();
}

QString Tools::readTextFile(const QString &fileName)
{
    debugLog("@@@@@ Tools::readTextFile " + fileName);
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
    debugLog(QString("@@@@@ Tools::getNetParams %1 %2 %3 %4").arg(np.localHostName, np.localMacAddress, np.localHostIP, np.localNetMask));
    return np;
}

bool Tools::writeBinaryFile(const QString& filePath, const QByteArray& data)
{
    debugLog("@@@@@ Tools::writeBinaryFile " + filePath);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        quint64 n1 = file.write(data);
        quint64 n2 = file.size();
        Tools::debugLog(QString("@@@@@ Tools::writeBinaryFile %1 %2").arg(QString::number(n1), QString::number(n2)));
        file.close();
        return true;
    }
    debugLog("@@@@@ Tools::writeBinaryFile ERROR");
    return false;
}

QByteArray Tools::readBinaryFile(const QString& path)
{
    debugLog("@@@@@ Tools::readBinaryFile " + path);
    QByteArray a;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        a = file.readAll();
        file.close();
    }
    else
        debugLog("@@@@@ Tools::writeBinaryFile ERROR");
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
            debugLog(QString("@@@@@ Tools::makeFullPath mkdir %1 %2").arg(Tools::boolToString(ok), path));
        }
    }
    path += "/" + dirs.last(); // file name
    //qDebug() << "@@@@@ Tools::makeFullPath local path " << localPath;
    //qDebug() << "@@@@@ Tools::makeFullPath full path " << path;
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
    debugLog(QString("@@@@@ Tools::isFileExistInDownloadPath %1 %2").arg(Tools::boolToString(ok), localPath));
    return ok;
}

QString Tools::qmlFilePath(const QString& localPath)
{
#ifdef Q_OS_ANDROID // --------------------------------------------------------
    QString path = QString("file:%1/%2").arg(DOWNLOAD_SUBDIR, localPath).replace(":/", ":").replace("//", "/");
#else
    QString path = QString("file:///%1").arg(downloadFilePath(localPath));
#endif // Q_OS_ANDROID --------------------------------------------------------
    //qDebug() << "@@@@@ Tools::qmlFilePath " << path;
    return localPath.isEmpty() ? "" : path;
}

QString Tools::dataBaseFilePath(const QString& localPath)
{
    return makeFullPath("", localPath);
}

bool Tools::copyFile(const QString& from, const QString& to)
{
    debugLog(QString("@@@@@ DataBase::copyFile %1 >> %2").arg(from, to));
    if (!QFile::exists(from)) return false;
    if (QFile::exists(to)) QFile::remove(to);
    return QFile::copy(from, to);
}

bool Tools::removeFile(const QString &path)
{
    debugLog("@@@@@ Tools::removeFile " + path);
    return QFile::exists(path) ? QFile::remove(path) : true;
}

QString Tools::downloadFilePath(const QString& localPath)
{
    return makeFullPath(DOWNLOAD_SUBDIR, localPath);
}

void Tools::pause(const int msec, const QString& comment)
{
    debugLog(QString("@@@@@ Tools::pause %1 %2").arg(QString::number(msec), comment));
    QThread::currentThread()->msleep(msec);
}

QString Tools::rootDir()
{
    // https://doc.qt.io/qt-6/qstandardpaths.html#StandardLocation-enum
#ifdef Q_OS_ANDROID // --------------------------------------------------------
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
    return app->applicationDirPath(); // Returns the directory that contains the application executable.
#endif // Q_OS_ANDROID --------------------------------------------------------
}

void Tools::sortByInt(DBRecordList& records, const int field, const bool increase)
{
    // https://copyprogramming.com/howto/how-to-sort-qlist-qvariant-in-qt
    int i, n;
    for (n = 0; n < records.count(); n++) for (i = n + 1; i < records.count(); i++)
    {
        if (increase && records[n][field].toInt() <= records[i][field].toInt()) continue;
        if (!increase && records[n][field].toInt() >= records[i][field].toInt()) continue;
        records.move(i, n);
        n = 0;
    }
}

void Tools::sortByString(DBRecordList& records, const int field, const bool increase)
{
    // https://copyprogramming.com/howto/how-to-sort-qlist-qvariant-in-qt
    int i, n;
    for (n = 0; n < records.count(); n++) for (i = n + 1; i < records.count(); i++)
    {
        if (increase && records[n][field].toString() <= records[i][field].toString()) continue;
        if (!increase && records[n][field].toString() >= records[i][field].toString()) continue;
        records.move(i, n);
        n = 0;
    }
}

void Tools::debugLog(const QString &s)
{
    qDebug() << s;
#ifdef DEBUG_LOG
    QFile f(dataBaseFilePath(DEBUG_LOG_NAME));
    if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
         QTextStream out(&f);
         out << dateTimeFromUInt(currentDateTimeToUInt()) << " " << s << EOL;
         f.close();
    }
#endif
}

void Tools::removeDebugLog()
{
#ifdef REMOVE_DEBUG_LOG_ON_START
    removeFile(dataBaseFilePath(DEBUG_LOG_NAME));
    debugLog("@@@@@ Tools::removeDebugLog");
#endif
}








