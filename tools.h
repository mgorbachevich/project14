#ifndef TOOLS_H
#define TOOLS_H

#include <QDir>
#include <QJsonObject>

class Tools
{
public:
    static QString weightToText(const double& value) {  return QString("%1").arg(value, 0, 'f', 3); }
    static QString moneyToText(const double&, const int);
    static double moneyFromText(const QString& value) {  return  QString(value).toDouble();; }
    static QString getDataFilePath(const QString& fileName) { return getDataStorageDir().filePath(fileName); }
    static QDir getDataStorageDir();
    static bool fileExists(const QString&);
    static QString readTextFile(const QString&);
    static QString jsonToString(const QJsonObject&);
    static QJsonObject stringToJson(const QString&);
    static int stringToInt(const QString&, const int defaultValue = 0);
    static int stringToInt(const QVariant&, const int defaultValue = 0);
    static double stringToDouble(const QString&, const double defaultValue = 0);
};

#endif // TOOLS_H
