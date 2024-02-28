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
    static QString boolToString(const bool value) { return QVariant(value).toString(); }
    static QString moneyToText(const double& value, const int pointPosition) { return QString("%1").arg(value, 0, 'f', pointPosition); }
    static double priceToDouble(const QString&, const int);
    static QString readTextFile(const QString&);
    static QString jsonToString(const QJsonObject&);
    static QJsonObject stringToJson(const QString&);
    static int stringToInt(const QString&, const int defaultValue = 0);
    static int stringToInt(const QVariant &v, const int defaultValue = 0) { return stringToInt(v.toString(), defaultValue); }
    static double stringToDouble(const QString&, const double defaultValue = 0);
    static quint64 currentDateTimeToUInt() { return QDateTime::currentMSecsSinceEpoch(); }
    static QString dateTimeFromUInt(quint64 v) { return QString("%1 %2").arg(dateFromUInt(v), timeFromUInt(v)); }
    static QString dateFromUInt(quint64 v) { return QDateTime::fromMSecsSinceEpoch(v).toString("dd.MM.yyyy"); }
    static QString timeFromUInt(quint64 v) { return QDateTime::fromMSecsSinceEpoch(v).toString("HH:mm:ss"); }
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
    static void pause(const int, const QString& comment = "");
    static void sound(const QString&, const int);
    static void sortByInt(DBRecordList&, const int);
    static void sortByString(DBRecordList&, const int);

private:
    static QString makeFullPath(const QString&, const QString&);
    static QString rootDir();

    static QApplication* app;
};

#endif // TOOLS_H
