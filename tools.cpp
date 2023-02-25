#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include "tools.h"
#include "constants.h"

QString Tools::moneyToText(const double& value, const int pointPosition)
{
    return QString("%1").arg(value, 0, 'f', pointPosition);
}

QDir Tools::getDataStorageDir()
{
    QDir dir;
    QString s = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + "/" + DATA_STORAGE_SUBDIR;
    if(!dir.exists(s)) dir.mkdir(s);
    return dir;
}

bool Tools::fileExists(const QString &fileName)
{
    qDebug() << "@@@@@ Tools::fileExists " << fileName;

    return true; // todo
   // return QFileInfo::exists(fileName);
}

QString Tools::readTextFile(const QString &fileName)
{
    qDebug() << "@@@@@ Tools::readTextFile " << fileName;

    QFile f(fileName);
    if (!f.open(QFile::ReadOnly | QFile::Text))
        return "";
    QTextStream in(&f);
    return in.readAll();
}

QString Tools::jsonToString(const QJsonObject &jo)
{
    QJsonDocument jd(jo);
    return QString(jd.toJson());
}

QJsonObject Tools::stringToJson(const QString &s)
{
    return QJsonDocument::fromJson(s.toUtf8()).object();
}

int Tools::stringToInt(const QString &s, const int defaultValue)
{
    bool ok;
    int v = s.toInt(&ok);
    if (!ok)
        return defaultValue;
    return v;
}

int Tools::stringToInt(const QVariant &v, const int defaultValue)
{
    return stringToInt(v.toString(), defaultValue);
}

double Tools::stringToDouble(const QString &s, const double defaultValue)
{
    bool ok;
    double v = s.toDouble(&ok);
    if (!ok)
        return defaultValue;
    return v;
}


