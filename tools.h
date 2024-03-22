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
    static QString moneyToText(const double&, const int);
    static double priceToDouble(const QString&, const int);
    static QString readTextFile(const QString&);
    static QString jsonToString(const QJsonObject&);
    static QString intToString(const int);
    static QJsonObject stringToJson(const QString&);
    static int stringToInt(const QString&, const int defaultValue = 0);
    static int stringToInt(const QVariant &v, const int defaultValue = 0);
    static double stringToDouble(const QString&, const double defaultValue = 0);
    static quint64 currentDateTimeToUInt();
    static QString dateTimeFromUInt(quint64, const QString&, const QString&, const QString&);
    static QString dateFromUInt(quint64, const QString&);
    static QString timeFromUInt(quint64, const QString&);
    static NetParams getNetParams();
    //static void memoryCheck();
    static bool writeBinaryFile(const QString&, const QByteArray& data);
    static QByteArray readBinaryFile(const QString&);
    static bool isFileExistInDownloadPath(const QString&);
    static QString qmlFilePath(const QString&); // Read only
    static QString dataBaseFilePath(const QString&);
    static QString downloadFilePath(const QString&);
    static bool copyFile(const QString&, const QString&);
    static bool removeFile(const QString&);
    static bool isFile(const QString&);
    static void pause(const int, const QString& comment = "");
    static void sound(const QString&, const int);
    static void sortByInt(DBRecordList&, const int, const bool increase = true);
    static void sortByString(DBRecordList&, const int, const bool increase = true);
    static void debugLog(const QString&);
    static void removeDebugLog();
    static bool isEnvironment(const EnvironmentType);

private:
    static QString makeFullPath(const QString&, const QString&);

    static QApplication* app;
};

#endif // TOOLS_H
