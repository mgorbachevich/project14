#ifndef TOOLS_H
#define TOOLS_H

#include <QDir>
#include <QJsonObject>
#include <QApplication>
#include <QNetworkInterface>
#include "netentry.h"
#include "constants.h"

class Tools
{
public:
    // Type conversion:
    static bool toBool(const QString& s) { return s == "1" || s.toUpper() == "TRUE"; }
    static QByteArray toBytes(const quint32);
    static double toDouble(const QString &s) { bool ok; double v = s.toDouble(&ok); return ok ? v : 0; }
    static int toInt(const QString &s) { bool ok; int v = s.toInt(&ok); return ok ? v : 0; }
    static int toInt(const QVariant& v) { return toInt(v.toString()); }
    static QString toIntString(const bool v) { return v ? "1" : "0"; }
    static QString sortIncrement(const bool v) { return v ? "true" : "false"; }
    static QString roundToString(const double, const int);
    static QString toString(const double, const int);
    static QString toString(const int v) { return QString::number(v); }
    static QString toString(const bool v) { return QVariant(v).toString(); }
    static QString toString(const quint64 v) { return QString::number(v); }
    static QString toString(const QJsonObject&);
    static QString toString(const QStringList& sl) { return sl.join(','); }
    static QStringList toStringList(const QString& s) { return s.split(','); }
    static QJsonObject toJsonObject(const QString&);
    static QString currentThreadId();

    // Files:
    static bool copyFile(const QString&, const QString&);
    static bool renameFile(const QString&, const QString&);
    static bool removeFile(const QString&);
    static QString sharedPath(const QString&);
    static QString dbPath(const QString& localPath) { return makeDirs(localPath, true); }
    static QString downloadPath(const QString& localPath) { return makeFullPath(DOWNLOAD_SUBDIR, localPath); }
    static QString fileNameFromPath(const QString& path) { return path.mid(path.lastIndexOf("/") + 1); }
    static QString qmlFilePath(const QString&); // Read only
    static bool isFileExists(const QString &path) { return QFile::exists(path); }
    static bool isFileExistsInDownloadPath(const QString&);
    static qint64 getFileSize(const QString&);
    //static QByteArray readBinaryFile(const QString&);
    static QString readTextFile(const QString&);
    static bool writeBinaryFile(const QString&, const QByteArray&);
    static bool writeTextFile(const QString&, const QString&);
    static QString makeDirs(const QString&, const bool internalStorage = false);
    //static QString exchangePath(const QString&);
    static QString getImageFileWithQmlPath(const DBRecord&);

    // DateTime:
    static quint64 nowMsec() { return QDateTime::currentMSecsSinceEpoch(); }
    static QDateTime now() { return QDateTime::currentDateTime(); }
    static QDateTime addDateTime(const QDateTime&, const int, const qint64);
    static QDateTime dateTimeFromString(const QString&);
    static QString dateFromUInt(quint64, const QString&);
    static QString dateTimeFromUInt(quint64, const QString&, const QString&, const QString&);
    static QString timeFromUInt(quint64, const QString&);

    // Other:
    static void initApplicationPath(const QString&);
    static bool checkPermission(const QString&);
    static void debugLog(const QString&);
    static void debugMemory();
    static bool isEnvironment(const EnvironmentType);
    static int getMemory(const MemoryType);
    static void pause(const int, const QString& comment = "");
    static double round(const double, const int);
    static void sound(const QString&, const int);
    static void sortByInt(DBRecordList&, const int, const bool increase = true);
    static void sortByString(DBRecordList&, const int, const bool increase = true);
    //static void memoryCheck();
    static QString getAndroidBuild();
    static NetEntry getNetEntry();

private:
    static QString makeFullPath(const QString&, const QString&);
};

#endif // TOOLS_H
