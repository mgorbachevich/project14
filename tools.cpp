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
#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

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

QString Tools::intToString(const int v)
{
    return QString::number(v);
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

int Tools::stringToInt(const QVariant &v, const int defaultValue)
{
    return stringToInt(v.toString(), defaultValue);
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

QString Tools::moneyToText(const double& value, const int pointPosition)
{
    return QString("%1").arg(value, 0, 'f', pointPosition);
}

QString Tools::boolToString(const bool value)
{
    return QVariant(value).toString();
}

quint64 Tools::currentDateTimeToUInt()
{
    return QDateTime::currentMSecsSinceEpoch();
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
    foreach (const QNetworkInterface& ni, QNetworkInterface::allInterfaces())
    {
        foreach (const QNetworkAddressEntry& entry, ni.addressEntries())
        {
            if (entry.ip().toString() == np.localHostIP)
            {
                np.localMacAddress = ni.hardwareAddress();
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

    // https://doc.qt.io/qt-6/qstandardpaths.html#StandardLocation-enum
#ifdef Q_OS_ANDROID
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
    QString path = app->applicationDirPath(); // Returns the directory that contains the application executable.
#endif // Q_OS_ANDROID

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
#ifdef Q_OS_ANDROID
    QString path = QString("file:%1/%2").arg(DOWNLOAD_SUBDIR, localPath).replace(":/", ":").replace("//", "/");
#else
    QString path = QString("file:///%1").arg(downloadFilePath(localPath));
#endif // Q_OS_ANDROID
    //qDebug() << "@@@@@ Tools::qmlFilePath " << path;
    return localPath.isEmpty() ? "" : path;
}

bool Tools::copyFile(const QString& from, const QString& to)
{
    debugLog(QString("@@@@@ DataBase::copyFile %1 >> %2").arg(from, to));
    if (!isFile(from)) return false;
    removeFile(to);
    return QFile::copy(from, to);
}

bool Tools::removeFile(const QString &path)
{
    debugLog("@@@@@ Tools::removeFile " + path);
    return isFile(path) ? QFile::remove(path) : true;
}

bool Tools::isFile(const QString &path)
{
    return QFile::exists(path);
}

QString Tools::dataBaseFilePath(const QString& localFilePath)
{
    return makeFullPath("", localFilePath);
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

void Tools::debugLog(const QString &text)
{
    QString s = text;
    qDebug() << s.replace("\n", " ").replace("\r", " ");
#ifdef DEBUG_LOG
    QFile f(dataBaseFilePath(DEBUG_LOG_NAME));
    if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
         QTextStream out(&f);
         out << dateTimeFromUInt(currentDateTimeToUInt(), "%1 %2", "dd.MM.yyyy", "hh:mm:ss.zzz") << " " << s.replace("\n", " ").replace("\r", " ") << EOL;
         f.close();
    }
#endif
}

void Tools::removeDebugLog()
{
    if(REMOVE_DEBUG_LOG_ON_START)
    {
        removeFile(dataBaseFilePath(DEBUG_LOG_NAME));
        debugLog("@@@@@ Tools::removeDebugLog");
    }
}

QString Tools::dateTimeFromUInt(quint64 v, const QString& format, const QString& dateFormat, const QString& timeFormat)
{
    return QString(format).arg(dateFromUInt(v, dateFormat), timeFromUInt(v, timeFormat));
}

QString Tools::dateFromUInt(quint64 v, const QString& format)
{
    return QDateTime::fromMSecsSinceEpoch(v).toString(format);
}

QString Tools::timeFromUInt(quint64 v, const QString& format)
{
    return QDateTime::fromMSecsSinceEpoch(v).toString(format);
}

bool Tools::isEnvironment(const EnvironmentType interface)
{
    bool result = false;
    switch(interface)
    {
    case EnvironmentType_WiFi:
    {
        // https://stackoverflow.com/questions/58168915/qt-api-to-check-if-wifi-is-enabled-disabled
        for(const QNetworkInterface& ni : QNetworkInterface::allInterfaces())
        {
            if (ni.type() == QNetworkInterface::Wifi)
            {
                result = ni.flags().testFlag(QNetworkInterface::IsRunning);
                /*
                qDebug() << iface.humanReadableName() << "(" << iface.name() << ")"
                         << "is up:" << iface.flags().testFlag(QNetworkInterface::IsUp)
                         << "is running:" << iface.flags().testFlag(QNetworkInterface::IsRunning);
                */
            }
        }
        break;
    }

#ifdef Q_OS_ANDROID
    case EnvironmentType_Bluetooth:
    case EnvironmentType_USB:
    case EnvironmentType_SDCard:
    {
        jint jresult = QJniObject::callStaticMethod<jint>(
                    "ru.shtrih_m.shtrihprint6/AndroidNative",
                    "isNativeEnvironment", "(I)I", interface);
        result = jresult == 0;
        break;
    }
#endif // Q_OS_ANDROID

    default: break;
    }

    debugLog(QString("@@@@@ Tools::isEnvironment %1 %2").arg(QString::number(interface), Tools::boolToString(result)));
    return result;
}





