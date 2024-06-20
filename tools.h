#ifndef TOOLS_H
#define TOOLS_H

#include <QDir>
#include <QJsonObject>
#include <QApplication>
#include "constants.h"
#include "netparams.h"

class Tools
{
public:
    explicit Tools(QApplication* a) { app = a; }

    static QString boolToString(const bool);
    static int boolToInt(const bool);
    static QString boolToIntString(const bool);
    static bool checkPermission(const QString&);
    static bool copyFile(const QString&, const QString&);
    static bool renameFile(const QString&, const QString&);
    static quint64 nowMsec() { return QDateTime::currentMSecsSinceEpoch(); }
    static QDateTime now() { return QDateTime::currentDateTime(); }
    static QString dateFromUInt(quint64, const QString&);
    static QString dateTimeFromUInt(quint64, const QString&, const QString&, const QString&);
    static QString dbPath(const QString&);
    static void debugLog(const QString&);
    static void debugMemory();
    static QString doubleToString(const double, const int);
    static QString downloadPath(const QString&);
    static QString exchangePath(const QString&);
    static QString fileNameFromPath(const QString&);
    static QString intToString(const int);
    static bool isEnvironment(const EnvironmentType);
    static bool isFileExists(const QString&);
    static bool isFileExistsInDownloadPath(const QString&);
    static QString jsonToString(const QJsonObject&);
    static qint64 getFileSize(const QString&);
    static int getMemory(const MemoryType);
    static NetParams getNetParams();
    static QString makeDirs(const bool, const QString&);
    //static void memoryCheck();
    static void pause(const int, const QString& comment = "");
    static QString qmlFilePath(const QString&); // Read only
    static QByteArray readBinaryFile(const QString&);
    static QString readTextFile(const QString&);
    static void removeDebugLog();
    static bool removeFile(const QString&);
    static double round(const double, const int);
    static void sound(const QString&, const int);
    static void sortByInt(DBRecordList&, const int, const bool increase = true);
    static void sortByString(DBRecordList&, const int, const bool increase = true);
    static double stringToDouble(const QString&, const double defaultValue = 0);
    static int stringToInt(const QString&, const int defaultValue = 0);
    static int stringToInt(const QVariant&, const int defaultValue = 0);
    static bool stringToBool(const QString&);
    static QJsonObject stringToJson(const QString&);
    static QString timeFromUInt(quint64, const QString&);
    static bool writeBinaryFile(const QString&, const QByteArray&);
    static bool writeTextFile(const QString&, const QString&);

private:
    static QString makeFullPath(const QString&, const QString&);

    static QApplication* app;
};

#endif // TOOLS_H
