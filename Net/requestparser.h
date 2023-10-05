#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <QByteArray>
#include <constants.h>

class RequestParser
{
public:
    RequestParser() {}
    bool parse(const int, const QByteArray&);
    QByteArray getText() { return text; }
    QByteArray getFileName() { return fileName; }
    QByteArray getFileData() { return fileData; }
    QByteArray getTableName() { return fileName; }
    QByteArray getCodeList() { return fileData; }

private:
    bool parseText(const QByteArray&);
    bool parseGetDataRequest(const QByteArray&);
    bool parseDeleteDataRequest(const QByteArray&);
    bool parseSetDataRequest(const QByteArray&);
    void clear() { text.clear(); fileName.clear(); fileData.clear(); }

    QByteArray text;
    QByteArray fileName;
    QByteArray fileData;
};

#endif // REQUESTPARSER_H
