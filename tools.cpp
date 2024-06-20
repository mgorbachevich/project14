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
    const quint64 now = nowMsec();
    if(now - soundTime < WAIT_SOUND_MSEC) return;
    soundTime = now;
    audioOutput.setVolume(1.0f * volume / 100.0f); // 0 .. 1.0f
    mediaPlayer.setAudioOutput(&audioOutput);
    mediaPlayer.setSource(QUrl(fileName)); // qrc:/...
    mediaPlayer.play();
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

bool Tools::stringToBool(const QString &s)
{
    return s == "1" || s == "true" || s == "TRUE";
}

double Tools::stringToDouble(const QString &s, const double defaultValue)
{
    bool ok;
    double v = s.toDouble(&ok);
    return ok ? v : defaultValue;
}

QString Tools::boolToString(const bool value)
{
    return QVariant(value).toString();
}

int Tools::boolToInt(const bool value)
{
    return value ? 1 : 0;
}

QString Tools::boolToIntString(const bool value)
{
    return intToString(value ? 1 : 0);
}

QString Tools::doubleToString(const double value, const int pointPosition)
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
            debugLog(QString("@@@@@ Tools::makeFullPath mkdir %1 %2").arg(Tools::boolToString(ok), path));
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
    debugLog(QString("@@@@@ Tools::writeBinaryFile %1 %2").arg(intToString(n1), intToString(n2)));
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
            debugLog(QString("@@@@@ Tools::makeFullPath mkdir %1 %2").arg(Tools::boolToString(ok), path));
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
    debugLog(QString("@@@@@ Tools::isFileExistsInDownloadPath %1 %2").arg(Tools::boolToString(ok), localPath));
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
    removeFile(to);
    return QFile::copy(from, to);
}

bool Tools::renameFile(const QString& from, const QString& to)
{
    debugLog(QString("@@@@@ Tools::renameFile %1 >> %2").arg(from, to));
    if (!isFileExists(from)) return false;
    removeFile(to);
    return QFile::rename(from, to);
}

bool Tools::removeFile(const QString &path)
{
    debugLog("@@@@@ Tools::removeFile " + path);
    return isFileExists(path) ? QFile::remove(path) : true;
}

bool Tools::isFileExists(const QString &path)
{
    return QFile::exists(path);
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

QString Tools::dbPath(const QString& localFilePath)
{
    QString v = makeDirs("", localFilePath);
    //debugLog("@@@@@ Tools::dbPath " + v);
    return v;
}

QString Tools::downloadPath(const QString& localPath)
{
    return makeFullPath(DOWNLOAD_SUBDIR, localPath);
}

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

QString Tools::fileNameFromPath(const QString& path)
{
    return path.mid(path.lastIndexOf("/") + 1);
}

void Tools::pause(const int msec, const QString& comment)
{
    if(msec > 0)
    {
        debugLog(QString("@@@@@ Tools::pause %1 %2").arg(intToString(msec), comment));
        QThread::currentThread()->msleep(msec);
    }
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

void Tools::removeDebugLog()
{
    if(REMOVE_DEBUG_LOG_ON_START)
    {
        removeFile(dbPath(DEBUG_LOG_NAME));
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
    {
        jint jresult = QJniObject::callStaticMethod<jint>(ANDROID_NATIVE_CLASS_NAME,
                    "isNativeEnvironment", "(I)I", type);
        result = (jresult == 0);
        break;
    }
#endif

    default: break;
    }

    debugLog(QString("@@@@@ Tools::isEnvironment %1 %2").arg(intToString(type), boolToString(result)));
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
                 Tools::intToString(getMemory(MemoryType_Max)),
                 Tools::intToString(getMemory(MemoryType_Available)),
                 Tools::intToString(getMemory(MemoryType_Used))));
#endif
}

bool Tools::checkPermission(const QString& permission)
{
    bool ok = true;

#ifdef Q_OS_ANDROID
    if (QtAndroidPrivate::checkPermission(permission).result() == QtAndroidPrivate::Denied)
        ok = (QtAndroidPrivate::requestPermission(permission).result() != QtAndroidPrivate::Denied);
    debugLog(QString("@@@@@ Tools::checkPermission %1 %2").arg(permission, boolToString(ok)));
#endif

    return ok;
}
