#ifndef TOOLS_H
#define TOOLS_H

#include <QDir>
#include <QJsonObject>
#include "netparams.h"

class Tools
{
public:
    static QString boolToString(const bool value) { return QVariant(value).toString(); }
    static QString moneyToText(const double& value, const int pointPosition) { return QString("%1").arg(value, 0, 'f', pointPosition); }
    static double priceToDouble(const QString&, const int);
    static QString getDataFilePath(const QString& fileName) { return getDataStorageDir().filePath(fileName); }
    static QDir getDataStorageDir();
    static bool fileExists(const QString&);
    static QString readTextFile(const QString&);
    static QString jsonToString(const QJsonObject&);
    static QJsonObject stringToJson(const QString&);
    static int stringToInt(const QString&, const int defaultValue = 0);
    static int stringToInt(const QVariant &v, const int defaultValue = 0) { return stringToInt(v.toString(), defaultValue); }
    static double stringToDouble(const QString&, const double defaultValue = 0);
    static qint64 currentDateTimeToInt() { return QDateTime::currentMSecsSinceEpoch(); }
    static QString dateTimeFromInt(qint64 v) { return QDateTime::fromMSecsSinceEpoch(v).toString("dd.MM.yyyy HH:mm:ss"); }
    static NetParams getNetParams();
    //static void memoryCheck();
};

#endif // TOOLS_H
