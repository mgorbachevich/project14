#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QHostInfo>
#include <QProcess>
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
    const quint64 now = nowMsec();
    if(now - soundTime < WAIT_SOUND_MSEC) return;
    soundTime = now;
    audioOutput.setVolume(1.0f * volume / 100.0f); // 0 .. 1.0f
    mediaPlayer.setAudioOutput(&audioOutput);
    mediaPlayer.setSource(QUrl(fileName)); // qrc:/...
    mediaPlayer.play();
}

QString Tools::toString(const QJsonObject &jo)
{
    return QJsonDocument(jo).toJson(QJsonDocument::Indented);
}

QJsonObject Tools::toJsonObject(const QString &s)
{
    return QJsonDocument::fromJson(s.toUtf8()).object();
}

QString Tools::toString(const double value, const int pointPosition)
{
    return QString("%1").arg(value, 0, 'f', pointPosition);
}

double Tools::round(const double value, const int pointPosition)
{
    int d = 1;
    for(int i = 0; i < pointPosition; i++) d *= 10;
    return ((double)qRound(value * d)) / d;
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

QString Tools::makeDirs(const bool internalStorage, const QString& localPath)
{
    if(localPath.isEmpty()) return "";
    const QStringList dirs = QString("/" + localPath).split("/");
    QDir dir;
    QString path;

    // https://doc.qt.io/qt-6/qstandardpaths.html#StandardLocation-enum
#ifdef Q_OS_ANDROID
    if(internalStorage) path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#else
    if(internalStorage) path = app->applicationDirPath();
#endif

    for(int i = 0; i < dirs.size() - 1; i++)
    {
        if(dirs.at(i).isEmpty()) continue;
        path += "/" + dirs.at(i);
        if(!dir.exists(path))
        {
            bool ok = dir.mkdir(path);
            debugLog(QString("@@@@@ Tools::makeFullPath mkdir %1 %2").arg(Tools::sortIncrement(ok), path));
        }
    }
    path += "/" + dirs.last(); // file name
    //qDebug() << "@@@@@ Tools::makeFullPath local path " << localPath;
    //qDebug() << "@@@@@ Tools::makeFullPath full path " << path;
    return path;
}

QString Tools::readTextFile(const QString &fileName)
{
    debugLog("@@@@@ Tools::readTextFile " + fileName);
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        debugLog("@@@@@ Tools::readTextFile ERROR");
        return "";
    }
    QTextStream in(&f);
    return in.readAll();
}

bool Tools::writeTextFile(const QString &fileName, const QString& data)
{
    debugLog("@@@@@ Tools::writeTextFile " + fileName);
    QFile f(fileName);
    if (!f.open(QFile::WriteOnly | QFile::Text))
    {
        debugLog("@@@@@ Tools::writeTextFile ERROR");
        return false;
    }
    QTextStream out(&f);
    out << data;
    f.close();
    return true;
}

bool Tools::writeBinaryFile(const QString& filePath, const QByteArray& data)
{
    debugLog("@@@@@ Tools::writeBinaryFile " + filePath);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        debugLog("@@@@@ Tools::writeBinaryFile ERROR");
        return false;
    }
    quint64 n1 = file.write(data);
    quint64 n2 = file.size();
    debugLog(QString("@@@@@ Tools::writeBinaryFile %1 %2").arg(toString(n1), toString(n2)));
    file.close();
    return true;
}

QByteArray Tools::readBinaryFile(const QString& path)
{
    debugLog("@@@@@ Tools::readBinaryFile " + path);
    QByteArray a;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        debugLog("@@@@@ Tools::writeBinaryFile ERROR");
        return a;
    }
    a = file.readAll();
    file.close();
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
#endif

    for(int i = 0; i < dirs.size() - 1; i++)
    {
        if(dirs.at(i).isEmpty()) continue;
        path += "/" + dirs.at(i);
        if(!dir.exists(path))
        {
            bool ok = dir.mkdir(path);
            debugLog(QString("@@@@@ Tools::makeFullPath mkdir %1 %2").arg(Tools::sortIncrement(ok), path));
        }
    }
    path += "/" + dirs.last(); // file name
    //qDebug() << "@@@@@ Tools::makeFullPath local path " << localPath;
    //qDebug() << "@@@@@ Tools::makeFullPath full path " << path;
    return path;
}

bool Tools::isFileExistsInDownloadPath(const QString &localPath)
{
    QString path = downloadPath(localPath);
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
    debugLog(QString("@@@@@ Tools::isFileExistsInDownloadPath %1 %2").arg(Tools::sortIncrement(ok), localPath));
    return ok;
}

QString Tools::qmlFilePath(const QString& localPath)
{
#ifdef Q_OS_ANDROID
    QString path = QString("file:%1/%2").arg(DOWNLOAD_SUBDIR, localPath).replace(":/", ":").replace("//", "/");
#else
    QString path = QString("file:///%1").arg(downloadPath(localPath));
#endif

    //qDebug() << "@@@@@ Tools::qmlFilePath " << path;
    return localPath.isEmpty() ? "" : path;
}

bool Tools::copyFile(const QString& from, const QString& to)
{
    debugLog(QString("@@@@@ Tools::copyFile %1 >> %2").arg(from, to));
    if (!isFileExists(from)) return false;
    if(isFileExists(to)) QFile::remove(to);
    return QFile::copy(from, to);
}

bool Tools::renameFile(const QString& from, const QString& to)
{
    debugLog(QString("@@@@@ Tools::renameFile %1 >> %2").arg(from, to));
    if (!isFileExists(from)) return false;
    if(isFileExists(to)) QFile::remove(to);
    return QFile::rename(from, to);
}

bool Tools::removeFile(const QString &path)
{
    debugLog("@@@@@ Tools::removeFile " + path);
    return isFileExists(path) ? QFile::remove(path) : true;
}

QString Tools::sharedPath(const QString& localPath)
{
#ifdef Q_OS_ANDROID
    return ANDROID_SHARED_PATH + localPath;
#else
    return Tools::dbPath(localPath);
#endif
}

qint64 Tools::getFileSize(const QString &path)
{
    qint64 size = 0;
    QFile f(path);
    if (f.open(QIODevice::ReadOnly))
    {
        size = f.size();
        f.close();
    }
    return size;
}
/*
QString Tools::exchangePath(const QString &localPath)
{
    //https://forum.qt.io/topic/34940/does-qstandardpaths-work-with-sdcard-on-android/2
    debugLog("@@@@@ Tools::exchangePath " + localPath);
    QString result;

#ifdef Q_OS_ANDROID
    QJniObject mediaDir = QJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
    QJniObject mediaPath = mediaDir.callObjectMethod("getAbsolutePath", "()Ljava/lang/String;");
    result = mediaPath.toString() + localPath;
    QJniEnvironment env;
    if (env->ExceptionCheck()) env->ExceptionClear();
#endif

    debugLog("@@@@@ Tools::exchangePath result " + result);
    return result;
}
*/
void Tools::pause(const int msec, const QString& comment)
{
    if(msec > 0)
    {
        debugLog(QString("@@@@@ Tools::pause %1 %2").arg(toString(msec), comment));
        QThread::currentThread()->msleep(msec);
    }
}

void Tools::sortByInt(DBRecordList& records, const int field, const bool increase)
{
    // https://copyprogramming.com/howto/how-to-sort-qlist-qvariant-in-qt
    int i, n;
    for (n = 0; n < records.count(); n++) for (i = n + 1; i < records.count(); i++)
    {
        if (increase && records[n][field].toLongLong() <= records[i][field].toLongLong()) continue;
        if (!increase && records[n][field].toLongLong() >= records[i][field].toLongLong()) continue;
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
#ifdef DEBUG_LOG_FILE
    QFile f(dbPath(DEBUG_LOG_NAME));
    if (f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
         QTextStream out(&f);
         out << dateTimeFromUInt(currentDateTimeToUInt(), "%1 %2", DATE_FORMAT, TIME_MSEC_FORMAT) << " " << s.replace("\n", " ").replace("\r", " ") << EOL;
         f.close();
    }
#endif
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

bool Tools::isEnvironment(const EnvironmentType type)
{
    bool result = false;
    switch(type)
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
        result = (1 == QJniObject::callStaticMethod<jint>(ANDROID_NATIVE_CLASS_NAME,
                                                          "isNativeEnvironment", "(I)I", type));
        break;
 #endif

    default: break;
    }

    debugLog(QString("@@@@@ Tools::isEnvironment %1 %2").arg(toString(type), sortIncrement(result)));
    return result;
}

int Tools::getMemory(const MemoryType type)
{
    //debugLog(QString("@@@@@ Tools::getMemory %1").arg(intToString(type)));

#ifdef Q_OS_ANDROID
    return QJniObject::callStaticMethod<jint>(ANDROID_NATIVE_CLASS_NAME, "getMemory", "(I)I", type);
#endif

    return -3;
}

void Tools::debugMemory()
{
#ifdef Q_OS_ANDROID
    debugLog(QString("@@@@@ Tools::debugMemory %1 %2 %3").arg(
                 Tools::toString(getMemory(MemoryType_Max)),
                 Tools::toString(getMemory(MemoryType_Available)),
                 Tools::toString(getMemory(MemoryType_Used))));
#endif
}

bool Tools::checkPermission(const QString& permission)
{
    bool ok = true;

#ifdef Q_OS_ANDROID
    if (QtAndroidPrivate::checkPermission(permission).result() == QtAndroidPrivate::Denied)
        ok = (QtAndroidPrivate::requestPermission(permission).result() != QtAndroidPrivate::Denied);
    debugLog(QString("@@@@@ Tools::checkPermission %1 %2").arg(permission, toString(ok)));
#endif

    return ok;
}

QByteArray Tools::toBytes(const quint32 v)
{
    QByteArray ba;
    QDataStream stream(&ba, QIODevice::WriteOnly);
    stream << v;
    return ba;
}

QString Tools::getAndroidBuild()
{
#ifdef Q_OS_ANDROID
    debugLog("@@@@@ Tools::getAndroidBuild");
    return (QJniObject::callStaticMethod<jstring>(ANDROID_NATIVE_CLASS_NAME, "getAndroidBuild")).toString();
#endif
    return "?";
}

NetEntry Tools::getNetEntry()
{
    // https://amin-ahmadi.com/2016/03/22/how-to-find-local-ip-addresses-in-qt/
    debugLog("@@@@@ Tools::getNetEntry");
    NetEntry result;
    QList<QNetworkInterface> nis = QNetworkInterface::allInterfaces();
    foreach(const QNetworkInterface& ni, nis)
    {
#ifdef DEBUG_NET_ENTRIES
        debugLog(QString("@@@@@ Tools::getNetEntry QNetworkInterface:"
                         "  name=%1"
                         "  humanReadableName=%2"
                         "  hardwareAddress=%3"
                         "  type=%4"
                         "  flags=%5"
                         "  isValid=%6").arg(
                     ni.name(),
                     ni.humanReadableName(),
                     ni.hardwareAddress(),
                     QVariant::fromValue(ni.type()).toString(),
                     QVariant::fromValue(ni.flags().toInt()).toString(),
                     QVariant::fromValue(ni.isValid()).toString()));
#endif
        if(ni.isValid())
        {
            QList<QNetworkAddressEntry> naes = ni.addressEntries();
            foreach (const QNetworkAddressEntry& nae, naes)
            {
#ifdef DEBUG_NET_ENTRIES
                debugLog(QString("@@@@@ Tools::getNetEntry QNetworkAddressEntry:"
                                 "  netmask=%1"
                                 "  ip=%2"
                                 "  protocol=%3"
                                 "  isGlobal=%4"
                                 "  isBroadcast=%5"
                                 "  isLinkLocal=%6"
                                 "  isUniqueLocalUnicast=%7"
                                 "  isLoopback=%8"
                                 "  isMulticast=%9"
                                 "  isNull=%10").arg(
                             nae.netmask().toString(),
                             nae.ip().toString(),
                             QVariant::fromValue(nae.ip().protocol()).toString(),
                             QVariant::fromValue(nae.ip().isGlobal()).toString(),
                             QVariant::fromValue(nae.ip().isBroadcast()).toString(),
                             QVariant::fromValue(nae.ip().isLinkLocal()).toString(),
                             QVariant::fromValue(nae.ip().isUniqueLocalUnicast()).toString(),
                             QVariant::fromValue(nae.ip().isLoopback()).toString(),
                             QVariant::fromValue(nae.ip().isMulticast()).toString(),
                             QVariant::fromValue(nae.ip().isNull()).toString()));
#endif
                if(result.ip.isEmpty() && nae.ip().isGlobal() &&
                        QVariant::fromValue(nae.ip().protocol()).toString() == "IPv4Protocol")
                {
                    result.ip = nae.ip().toString();
                    result.type = QVariant::fromValue(ni.type()).toString();
                }
            }
        }
    }
    debugLog(QString("@@@@@ Tools::getNetEntry ip=%1 type=%2").arg(result.ip, result.type));
    return result;
}

QString Tools::getIP()
{
    debugLog("@@@@@ Tools::getIP");
    NetEntry ne = getNetEntry();
    return ne.ip.isEmpty() ? "?" : QString("%1 (%2)").arg(ne.ip, ne.type);
    /*
#ifdef Q_OS_ANDROID
    auto context = QNativeInterface::QAndroidApplication::context();
    return (QJniObject::callStaticMethod<jstring>(ANDROID_NATIVE_CLASS_NAME, "getIP1",
            "(Landroid/content/Context;)Ljava/lang/String;", context)).toString();
#else
    // https://stackoverflow.com/questions/13835989/get-local-ip-address-in-qt
    QString localHostIP;
    QString localHostName = QHostInfo::localHostName();
    QList<QHostAddress> hosts = QHostInfo::fromName(localHostName).addresses();
    foreach (const QHostAddress& address, hosts)
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false)
            localHostIP = address.toString();
    }
    return localHostIP;
#endif
    */
}

QString Tools::getSSID()
{
#ifdef Q_OS_ANDROID
    debugLog("@@@@@ Tools::getSSID");
    auto context = QNativeInterface::QAndroidApplication::context();
    return (QJniObject::callStaticMethod<jstring>(ANDROID_NATIVE_CLASS_NAME, "getSSID1",
            "(Landroid/content/Context;)Ljava/lang/String;", context)).toString();
#endif
    return "?";
}


