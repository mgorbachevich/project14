#ifndef TOOLS_H
#define TOOLS_H

#include <QDir>
#include <QJsonObject>
#include <QApplication>
#include "netparams.h"

class Tools
{
public:
    explicit Tools(QApplication* a) { app = a; }

    static QString boolToString(const bool value) { return QVariant(value).toString(); }
    static QString moneyToText(const double& value, const int pointPosition) { return QString("%1").arg(value, 0, 'f', pointPosition); }
    static double priceToDouble(const QString&, const int);
    static QString dataFilePath(const QString& fileName) { return getDataStorageDir().filePath(fileName); }
    static QDir getDataStorageDir();
    static bool fileExistsInAppPath(const QString&);
    static QString readTextFile(const QString&);
    static QString jsonToString(const QJsonObject&);
    static QJsonObject stringToJson(const QString&);
    static int stringToInt(const QString&, const int defaultValue = 0);
    static int stringToInt(const QVariant &v, const int defaultValue = 0) { return stringToInt(v.toString(), defaultValue); }
    static double stringToDouble(const QString&, const double defaultValue = 0);
    static quint64 currentDateTimeToUInt() { return QDateTime::currentMSecsSinceEpoch(); }
    static QString dateTimeFromUInt(quint64 v) { return QDateTime::fromMSecsSinceEpoch(v).toString("dd.MM.yyyy HH:mm:ss"); }
    static NetParams getNetParams();
    //static void memoryCheck();
    static bool writeBinaryFile(const QString&, const QByteArray& data);
    static QByteArray readBinaryFile(const QString&);
    static QString appFilePath(const QString&, const QString&); // Read/Write
    static QString qmlFilePath(const QString&, const QString&); // Read only

private:
    static QApplication* app;
};

#endif // TOOLS_H
