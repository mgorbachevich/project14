#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <QByteArray>
#include <constants.h>
#include "externalmessager.h"

class DataBase;

class RequestParser : public ExternalMessager
{
    Q_OBJECT

public:
    explicit RequestParser(AppManager*);
    QString parseGetRequest(const NetAction, const QByteArray&);
    QString parseSetRequest(const QByteArray&);
    static QString makeResultJson(const int, const QString&, const QString&, const QString&);
    static QString makeResultJson(const int, const QString&, const QString&, const QStringList&);

private:
    QString toJsonString(const QByteArray&);
    QByteArray parseHeaderItem(const QByteArray&, const QByteArray&, const QByteArray& title = "Content-Disposition");
    bool parseCommand(const QByteArray&);
};

#endif // REQUESTPARSER_H
