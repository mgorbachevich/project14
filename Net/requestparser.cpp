#include <QDebug>
#include <QByteArrayView>
#include <QHash>
#include <QJsonObject>
#include <QJsonArray>
#include "requestparser.h"
#include "appmanager.h"
#include "resourcedbtable.h"
#include "tools.h"
#include "requestparser.h"

RequestParser::RequestParser(AppManager* parent) : ExternalMessager(parent)
{
    Tools::debugLog("@@@@@ RequestParser::RequestParser");
}

QString RequestParser::toJsonString(const QByteArray& request)
{
    Tools::debugLog("@@@@@ RequestParser::toJsonString");
    QByteArray text;
    const int i1 = request.indexOf("{");
    const int i2 = request.lastIndexOf("}");
    if (i2 > i1) text = request.mid(i1, i2 - i1 + 1);
    return QString(text);
}

QString RequestParser::parseGetRequest(const NetAction action, const QByteArray &request)
{
    // Parse list of codes to upload:
    Tools::debugLog(QString("@@@@@ RequestParser::parseGetRequest %1 %2").arg(QString::number(action), QString(request)));
    QByteArray tableName;
    QByteArray codes;
    const QByteArray s1 = "source=";
    int p1 = request.indexOf(s1);
    bool codesOnly = false;
    if(p1 >= 0)
    {
        int p2 = request.indexOf("&");
        if(p2 < 0)
        {
            int p2 = request.length();
            Tools::debugLog(QString("@@@@@ RequestParser::parseGetRequest: p1=%1, p2=%2").arg(QString::number(p1), QString::number(p2)));
            if(p2 > p1)
            {
                p1 += s1.length();
                tableName = request.mid(p1, p2 - p1);
                Tools::debugLog("@@@@@ RequestParser::parseGetRequest table " + tableName);
            }
        }
        else
        {
            const QByteArray s2 = "codes=";
            const QByteArray s3 = "code=";
            const QByteArray s4 = "codelist=";
            if(p2 > p1)
            {
                p1 += s1.length();
                tableName = request.mid(p1, p2 - p1);
                Tools::debugLog("@@@@@ RequestParser::parseGetRequest table " + tableName);
                if (request.contains(s2))
                {
                    int p3 = request.indexOf("[") + 1;
                    int p4 = request.indexOf("]");
                    codes = request.mid(p3, p4 - p3);
                }
                else if (request.contains(s3))
                {
                    int p3 = request.indexOf(s3);
                    int p4 = request.length();
                    p3 += s3.length();
                    codes = request.mid(p3, p4 - p3);
                }
                else if (request.contains(s4))
                {
                    int p3 = request.indexOf(s4);
                    int p4 = request.length();
                    p3 += s4.length();
                    codesOnly = (1 == Tools::stringToInt(QString(request.mid(p3, p4 - p3))));
                }
            }
        }
    }
    switch(action)
    {
    case NetAction_Delete:
        if(!codesOnly) return appManager->db->netDelete(tableName, codes);
        break;
    case NetAction_Upload:
        return appManager->db->netUpload(tableName, codes, codesOnly);
    default: break;
    }
    return makeResultJson(LogError_WrongRequest, "Некорректный запрос", "", "");
}

QString RequestParser::makeResultJson(const int errorCode, const QString& description, const QString& tableName, const QString& data)
{
    if(tableName.isEmpty() || data.isEmpty())
        return QString("{\"result\":\"%1\",\"description\":\"%2\"}").
                arg(QString::number(errorCode), description);
    else
        return QString("{\"result\":\"%1\",\"description\":\"%2\",\"data\":{\"%3\":%4}}").
            arg(QString::number(errorCode), description, tableName, data);
}

QString RequestParser::makeResultJson(const int errorCode, const QString& description, const QString& tableName, const QStringList& values)
{
    QString data;
    if(values.count() > 0)
    {
        data += QString("%1").arg(values[0]);
        for(int i = 1; i < values.count(); i++) data += QString(",%1").arg(values[i]);
    }
    return QString("{\"result\":\"%1\",\"description\":\"%2\",\"codelist\":{\"%3\":[%4]}}").
        arg(QString::number(errorCode), description, tableName, data);
}

QByteArray RequestParser::parseHeaderItem(const QByteArray& header, const QByteArray& item, const QByteArray& title)
{
    if(!header.contains(title) || !header.contains(item)) return "";
    const int i1 = header.indexOf(item);
    if(i1 < 0) return "";
    const int i2 = header.indexOf("\"", i1);
    if(i2 < 0) return "";
    const int i3 = header.indexOf("\"", i2 + 1);
    if(i3 <= i2) return "";
    return header.mid(i2 + 1, i3 - i2 - 1);
}

QString RequestParser::parseSetRequest(const QByteArray &request)
{
    Tools::debugLog("@@@@@ RequestParser::parseSetRequest " + QString::number(request.length()));
    int successCount = 0;
    int errorCount = 0;
    int errorCode = 0;
    QString description = "Ошибок нет";

    // Singlepart:
    if(request.indexOf("{") == 0)
    {
        Tools::debugLog("@@@@@ RequestParser::parseSetRequest Singlepart");
        if(!parseCommand(request))
        {
            errorCode = LogError_WrongRequest;
            description = "Неверный запрос";
        }
        return makeResultJson(errorCode, description, "", "");
    }

    // Multipart:
    Tools::debugLog("@@@@@ RequestParser::parseSetRequest Multipart");
    QByteArray boundary = request.mid(0, request.indexOf(EOL));
    Tools::debugLog("@@@@@ RequestParser::parseSetRequest Boundary " + QString(boundary));
    QList<int> boundaryIndeces;
    int bi = 0;
    while(bi >= 0)
    {
        boundaryIndeces.append(bi);
        bi = request.indexOf(QByteArrayView(boundary), bi + boundary.size() + 2);
    }
    Tools::debugLog("@@@@@ RequestParser::parseSetRequest Boundary indices ");
    const int partCount = boundaryIndeces.count() - 1;
    Tools::debugLog("@@@@@ RequestParser::parseSetRequest Part count " + QString::number(partCount));
    QHash<DBTable*, DBRecordList> downloadedRecords;
    for(int partIndex = 0; partIndex < partCount; partIndex++)
    {
        // Считаю что хедеров не больше 2:
        QByteArrayList headers;
        const int d = QByteArray(EOL).length();
        const int i1 = boundaryIndeces[partIndex] + boundary.size() + d;
        const int i2 = request.indexOf(EOL, i1) + d;
        const int i3 = request.indexOf(EOL, i2) + d;
        headers.append(request.mid(i1, i2 - i1 - d));
        headers.append(request.mid(i2, i3 - i2 - d));
        if(partIndex == 0) // Value
        {
            for(int hi = 0; hi < headers.count(); hi++)
            {
                QByteArray& header = headers[hi];
                Tools::debugLog(QString("@@@@@ RequestParser::parseSetRequest. Part %1, header %2 = %3").
                        arg(QString::number(partIndex), QString::number(hi), header));

                // Content-Disposition: form-data; name="value"
                QByteArray nameItemValue = parseHeaderItem(header, "name");
                if(nameItemValue == "value")
                {
                    QString text = toJsonString(request.mid(i3 + d, boundaryIndeces[partIndex + 1] - i3 - d));
                    Tools::debugLog("@@@@@ RequestParser::parseSetRequest. Text " + text);
                    if(!text.isEmpty())
                    {
                        appManager->db->onParseSetRequest(text);
                        for (DBTable* t : appManager->db->getTables())
                        {
                            if(t == nullptr) continue;
                            DBRecordList tableRecords = t->parse(text);
                            if(tableRecords.count() == 0) continue;
                            Tools::debugLog(QString("@@@@@ RequestParser::parseSetRequest. Table %1, records %2").
                                    arg(t->name, QString::number(tableRecords.count())));
                            downloadedRecords.insert(t, tableRecords);
                        }
                    }
                    Tools::debugLog("@@@@@ RequestParser::parseSetRequest. Record count " + QString::number( downloadedRecords.count()));
                }
            }
        }
        else // Resources
        {
            for(int hi = 0; hi < headers.count(); hi++)
            {
                QByteArray& header = headers[hi];
                Tools::debugLog(QString("@@@@@ RequestParser::parseSetRequest. Part %1, header %2 = %3").
                        arg(QString::number(partIndex), QString::number(hi), header));
                QList tables = downloadedRecords.keys();
                for (DBTable* table : tables)
                {
                    const int fieldIndex = table->columnIndex("field");
                    if(fieldIndex < 0) continue;
                    Tools::debugLog("@@@@@ RequestParser::parseSetRequest. Table " + table->name);
                    DBRecordList tableRecords = downloadedRecords.value(table);
                    for(int ri = 0; ri < tableRecords.count(); ri++)
                    {
                        DBRecord& record = tableRecords[ri];
                        // Например, Content-Disposition: form-data; name="file1"; filename=":/Images/image.png"
                        QByteArray nameItemValue = parseHeaderItem(header, "name");
                        if(nameItemValue != record[fieldIndex].toString().toUtf8()) continue;
                        QByteArray fileNameItemValue = parseHeaderItem(header, "filename");
                        QString source = QString(fileNameItemValue);

                        // New resource file name (tableName/recordCode.extension):
                        const int dotIndex = source.lastIndexOf(".");
                        QString extension =  dotIndex < 0 ? "" : source.mid(dotIndex + 1, source.length());
                        QString localFilePath = QString("%1/%2.%3").arg(table->name, record[0].toString(), extension);
                        //qDebug() << "@@@@@ RequestParser::parseSetRequest. Record =" << DBTable::toJsonString(table, record);

                        // Write data file:
                        QByteArray fileData = request.mid(i3 + d, boundaryIndeces[partIndex + 1] - i3 - d * 2);
                        Tools::debugLog("@@@@@ RequestParser::parseSetRequest. File data length " + QString::number( fileData.length()));
                        QString fullFilePath = Tools::downloadPath(localFilePath);
                        Tools::debugLog("@@@@@ RequestParser::parseSetRequest. Full file path " + fullFilePath);
                        if(Tools::writeBinaryFile(fullFilePath, fileData))
                        {
                            successCount++;
                            record[ResourceDBTable::Source] = source;
                            record[ResourceDBTable::Value] = localFilePath;
                        }
                        else
                        {
                            errorCount++;
                            errorCode = LogError_WrongRecord;
                            record[ResourceDBTable::Source] = "";
                            record[ResourceDBTable::Value] = "";
                            Tools::debugLog("@@@@@ RequestParser::parseSetRequest. Write file ERROR");
                        }
                    }
                    downloadedRecords.remove(table);
                    downloadedRecords.insert(table, tableRecords);
                }
            }
        }
    }
    appManager->db->netDownload(downloadedRecords, successCount, errorCount);
    description = QString("Загружено записей %1 из %2").
            arg(QString::number(successCount), QString::number(successCount + errorCount));
    return makeResultJson(errorCode, description, "", "");
}

bool RequestParser::parseCommand(const QByteArray& request)
{
    QString json = toJsonString(request);
    Tools::debugLog("@@@@@ RequestParser::parseCommand " + json);
    QJsonObject jo = Tools::stringToJson(json);
    QString method = jo["method"].toString("");
    if (method.isEmpty()) return false;
    QJsonValue jsonParams = jo["params"];
    if (!jsonParams.isArray()) return false;
    QJsonArray ja = jsonParams.toArray();
    NetCommand nc = NetCommand_None;
    QString param = "";
    if(method == "stopLoad")
    {
        nc = NetCommand_StopLoad;
    }
    else if(ja.size() > 0)
    {
        if(method == "message")
        {
            nc = NetCommand_Message;
            param = ja[0].toString("");
        }
        else if(method == "startLoad")
        {
            nc = NetCommand_StartLoad;
            param = Tools::intToString(ja[0].toInt());
        }
        else if(method == "progress")
        {
            nc = NetCommand_Progress;
            param = Tools::intToString(ja[0].toInt());
        }
    }
    if(nc != NetCommand_None)
    {
        appManager->onNetCommand(nc, param);
        return true;
    }
    return false;
}
