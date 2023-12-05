#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <QByteArray>
#include <constants.h>

class DataBase;

class RequestParser
{
public:
    static QString parseGetRequest(DataBase*, const QByteArray&);
    static QString parseDeleteRequest(DataBase*, const QByteArray&);
    static QString parseSetRequest(DataBase*, const QByteArray&);
    static QString makeResultJson(const int, const QString&, const QString&, const QString&);

private:
    static QString parseGetRequest(const NetAction, DataBase*, const QByteArray&);
    static QString parseJson(const QByteArray&);
    static QByteArray parseHeaderItem(const QByteArray&, const QByteArray&, const QByteArray& title = "Content-Disposition");
};

#endif // REQUESTPARSER_H
