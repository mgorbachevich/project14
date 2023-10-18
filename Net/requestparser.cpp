#include <QDebug>
#include <QByteArrayView>
#include "requestparser.h"

bool RequestParser::parseText(const QByteArray& request)
{
    qDebug() << "@@@@@ RequestParser::parseText";
    const int i1 = request.indexOf("{");
    const int i2 = request.lastIndexOf("}");
    if (i2 > i1)
    {
        text = request.mid(i1, i2 - i1 + 1);
        //qDebug() << "@@@@@ RequestParser::parseText " << QString(text);
    }
    return !text.isEmpty();
}

bool RequestParser::parseGetDataRequest(const QByteArray &request)
{
    // Parse list of codes to upload:
    qDebug() << "@@@@@ RequestParser::parseGetDataRequest " << QString(request);

    const QByteArray s1 = "source=";
    int p1 = request.indexOf(s1);
    if(p1 >= 0)
    {
        int p2 = request.indexOf("&");
        if(p2 < 0)
        {
            int p2 = request.length();
            qDebug() << "@@@@@ AppManager::parseGetDataRequest: p1, p2 =" << p1 << p2;
            if(p2 > p1)
            {
                p1 += s1.length();
                fileName = request.mid(p1, p2 - p1);
                qDebug() << "@@@@@ AppManager::parseGetDataRequest: table name =" << fileName;
            }
        }
        else
        {
            const QByteArray s2 = "codes=";
            const QByteArray s3 = "code=";
            if(p2 > p1 && (request.contains(s2) || request.contains(s3)))
            {
                p1 += s1.length();
                fileName = request.mid(p1, p2 - p1);
                qDebug() << "@@@@@ AppManager::parseGetDataRequest: table name =" << fileName;
                if (request.contains(s2))
                {
                    int p3 = request.indexOf("[") + 1;
                    int p4 = request.indexOf("]");
                    fileData = request.mid(p3, p4 - p3);
                }
                else if (request.contains(s3))
                {
                    int p3 = request.indexOf(s3);
                    int p4 = request.length();
                    p3 += s3.length();
                    fileData = request.mid(p3, p4 - p3);
                }
            }
        }
    }
    if(fileName.isEmpty())
    {
        qDebug() << "@@@@@ AppManager::parseGetDataRequest ERROR";
        return false;
    }
    //qDebug() << "@@@@@ AppManager::parseGetDataRequest: code list =" << fileData;
    return true;
}

bool RequestParser::parseDeleteDataRequest(const QByteArray &request)
{
    return parseGetDataRequest(request);
}

bool RequestParser::parseSetDataRequest(const QByteArray &request)
{
    qDebug() << "@@@@@ RequestParser::parseSetDataRequest";

    // Singlepart:
    if(request.indexOf("{") == 0)
    {
        qDebug() << "@@@@@ RequestParser::parseSetDataRequest. Singlepart";
        return parseText(request);
    }

    // Multipart:
    qDebug() << "@@@@@ RequestParser::parseSetDataRequest. Multipart";
    QByteArray boundary = request.mid(0, request.indexOf("\r\n"));
    qDebug() << "@@@@@ RequestParser::parseSetDataRequest. Boundary =" << QString(boundary);
    QList<int> boundaryIndeces;
    int bi = 0;
    boundaryIndeces.append(bi);
    for(int i = 1; i < 3; i++)
    {
        bi = request.indexOf(QByteArrayView(boundary), bi + boundary.size() + 2);
        if(bi >= 0) boundaryIndeces.append(bi);
    }
    qDebug() << "@@@@@ RequestParser::parseSetDataRequest. Boundary indices =" << boundaryIndeces;
    const int partCount = boundaryIndeces.count() - 1;
    qDebug() << "@@@@@ RequestParser::parseSetDataRequest. Part count =" << partCount;
    int result = 0;
    for(int i = 0; i < partCount; i++)
    {
        const int i1 = boundaryIndeces[i] + boundary.size() + 2;
        const int i2 = request.indexOf("\r\n", i1) + 2;
        const int i3 = request.indexOf("\r\n", i2) + 2;
        QByteArrayList headers;
        headers.append(request.mid(i1, i2 - i1 - 2));
        headers.append(request.mid(i2, i3 - i2 - 2));
        for(int j = 0; j < headers.count(); j++)
        {
            qDebug() << "@@@@@ RequestParser::parseSetDataRequest. Header =" << headers[j];
            if(headers[j].contains("\"value\""))
            {
                parseText(request.mid(i3 + 2, boundaryIndeces[i + 1] - i3 - 2));
                if(!text.isEmpty()) result++;
                qDebug() << "@@@@@ RequestParser::parseSetDataRequest. Text lenght =" << text.length();
            }
            else if(headers[j].contains("\"file1\"")) // Например, Content-Disposition: form-data; name="file1"; filename=":/Images/image.png"
            {
                const int j1 = headers[j].indexOf("filename");
                if(j1 < 0) continue;
                int j2 = headers[j].indexOf("\"", j1);
                if(j2 < 0) continue;
                const int j3 = headers[j].lastIndexOf("\"");
                if(j3 < 0) continue;
                const int j4 = headers[j].indexOf(":", j1);
                const int j5 = headers[j].indexOf("/", j1);
                if(j4 > j2) j2 = j4;
                if(j5 == j2 + 1) j2 = j5;
                fileName = headers[j].mid(j2 + 1, j3 - j2 - 1); // Должно получиться Images/image.png
                fileData = request.mid(i3 + 2, boundaryIndeces[i + 1] - i3 - 4);
                //qDebug() << "@@@@@ RequestParser::parseSetDataRequest indeces =" << j1 << j2 << j3 << j4 << j5;
                qDebug() << "@@@@@ RequestParser::parseSetDataRequest. File name =" << QString(fileName);
                qDebug() << "@@@@@ RequestParser::parseSetDataRequest. File data length =" << fileData.length();
                if(!fileData.isEmpty() && !fileName.isEmpty()) result++;
            }
        }
    }
    return partCount > 0 && result == partCount;
}

bool RequestParser::parse(const int requestType, const QByteArray& request)
{
    qDebug() << "@@@@@ RequestParser::parse  requestType =" << requestType;
    clear();
    bool ok = false;
    switch(requestType)
    {
    case NetRequestType_DeleteData:
        ok = parseDeleteDataRequest(request);
        break;
    case NetRequestType_GetData:
        ok = parseGetDataRequest(request);
        break;
    case NetRequestType_SetData:
        ok = parseSetDataRequest(request);
        break;
    }
    if(!ok) clear();
    return ok;
}
