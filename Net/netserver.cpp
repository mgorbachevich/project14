#include <QDebug>
#include <QJsonArray>
#include "netserver.h"
#include "tools.h"
#include "appmanager.h"
#include "externalmessager.h"
#include "resourcedbtable.h"
#include "netactionresult.h"

NetServer::NetServer(AppManager* parent) : ExternalMessager(parent)
{
    Tools::debugLog("@@@@@ NetServer::NetServer");
}

NetServer::~NetServer()
{
    Tools::debugLog("@@@@@ NetServer::~NetServer");
    stop();
}

void NetServer::stop()
{
    if (server != nullptr)
    {
        Tools::debugLog("@@@@@ NetServer::stop");
        delete server;
        server = nullptr;
    }
}

void NetServer::start(const int port)
{
    Tools::debugLog("@@@@@ NetServer::start " + QString::number(port));
    stop();
    server = new QHttpServer();
    if(server->listen(QHostAddress::Any, port) == 0)
        Tools::debugLog("@@@@@ NetServer::start ERROR");
    else
    {
        server->route("/deleteData", [this] (const QHttpServerRequest &request)
        {
            return onRoute(RouterRule_Delete, request);
        });
        server->route("/getData", [this] (const QHttpServerRequest &request)
        {
            return onRoute(RouterRule_Get, request);
        });
        server->route("/setData", [this] (const QHttpServerRequest &request)
        {
            return onRoute(RouterRule_Set, request);
        });
        server->route("/command", [this] (const QHttpServerRequest &request)
        {
            return onRoute(RouterRule_Command, request);
        });

        server->afterRequest([](QHttpServerResponse &&response)
        {
            Tools::debugLog("@@@@@ NetServer::start afterRequest");
            response.setHeader("Content-Type", "application/json");
            return std::move(response);
        });
    }
}

QString NetServer::onRoute(const RouterRule rule, const QHttpServerRequest &request)
{
    Tools::debugLog("@@@@@ NetServer::onRoute");
    appManager->status.isNet = true;
    NetActionResult result(rule);
    switch(rule)
    {
    case RouterRule_Delete:
        appManager->status.isRefreshNeeded = true;
    case RouterRule_Get:
        result = parseGetRequest(rule, request.query().toString().toUtf8());
        break;
    case RouterRule_Set:
        appManager->status.downloadedRecordCount = 0;
    case RouterRule_Command:
        appManager->status.isRefreshNeeded = true;
        result = parseSetRequest(rule, request.body());
        break;
    }
    appManager->onNetResult(result);
    appManager->status.isNet = false;
    return result.requestReply;
}

QString NetServer::toJsonString(const QByteArray& request)
{
    Tools::debugLog("@@@@@ NetServer::toJsonString");
    QByteArray text;
    const int i1 = request.indexOf("{");
    const int i2 = request.lastIndexOf("}");
    if (i2 > i1) text = request.mid(i1, i2 - i1 + 1);
    return QString(text);
}

QByteArray NetServer::parseHeaderItem(const QByteArray& header, const QByteArray& item, const QByteArray& title)
{
    if(!header.contains(title) || !header.contains(item)) return "";
    const int i1 = header.indexOf(item);
    if(i1 < 0) return "";
    const int i2 = header.indexOf('\"', i1);
    if(i2 < 0) return "";
    const int i3 = header.indexOf('\"', i2 + 1);
    if(i3 <= i2) return "";
    return header.mid(i2 + 1, i3 - i2 - 1);
}

bool NetServer::parseCommand(const QByteArray& request)
{
    QString json = toJsonString(request);
    Tools::debugLog("@@@@@ NetServer::parseCommand " + json);
    QJsonObject jo = Tools::toJsonObject(json);
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
        param = Tools::sortIncrement(true);
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
            param = Tools::toString(ja[0].toInt());
        }
        else if(method == "progress")
        {
            nc = NetCommand_Progress;
            param = Tools::toString(ja[0].toInt());
        }
    }
    if(nc != NetCommand_None)
    {
        emit netCommand(nc, param);
        return true;
    }
    return false;
}

NetActionResult NetServer::parseGetRequest(const RouterRule rule, const QByteArray &request)
{
    // Parse list of codes to upload:
    Tools::debugLog(QString("@@@@@ NetServer::parseGetRequest %1").arg(QString(request)));

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
            Tools::debugLog(QString("@@@@@ NetServer::parseGetRequest: p1=%1, p2=%2").arg(QString::number(p1), QString::number(p2)));
            if(p2 > p1)
            {
                p1 += s1.length();
                tableName = request.mid(p1, p2 - p1);
                Tools::debugLog("@@@@@ NetServer::parseGetRequest table " + tableName);
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
                Tools::debugLog("@@@@@ NetServer::parseGetRequest table " + tableName);
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
                    codesOnly = (1 == Tools::toInt(QString(request.mid(p3, p4 - p3))));
                }
            }
        }
    }
    if(rule == RouterRule_Get) return appManager->netUpload(tableName, codes, codesOnly);
    if(!codesOnly)             return appManager->netDelete(tableName, codes);

    NetActionResult result(rule);
    result.errorCode = Error_Log_WrongRequest;
    result.description = "Некорректный сетевой запрос";
    result.requestReply = result.makeEmptyJson();
    return result;
}

NetActionResult NetServer::parseSetRequest(const RouterRule rule, const QByteArray &request)
{
    Tools::debugLog("@@@@@ NetServer::parseSetRequest " + QString::number(request.length()));
    NetActionResult result(rule);
    // Singlepart:
    if(request.indexOf("{") == 0)
    {
        Tools::debugLog("@@@@@ NetServer::parseSetRequest Singlepart");
        if(!parseCommand(request))
        {
            result.errorCode = Error_Log_WrongRequest;
            result.description = "Неверный сетевой запрос";
        }
        result.requestReply = result.makeEmptyJson();
        return result;
    }

    // Multipart:
    Tools::debugLog("@@@@@ NetServer::parseSetRequest Multipart");
    QByteArray boundary = request.mid(0, request.indexOf(EOL));
    Tools::debugLog("@@@@@ NetServer::parseSetRequest Boundary " + QString(boundary));

    QList<int> boundaryIndeces;
    int bi = 0;
    while(bi >= 0)
    {
        boundaryIndeces.append(bi);
        bi = request.indexOf(QByteArrayView(boundary), bi + boundary.size() + 2);
    }
    // Tools::debugLog("@@@@@ NetServer::parseSetRequest Boundary indices ");
    const int partCount = boundaryIndeces.count() - 1;
    Tools::debugLog("@@@@@ NetServer::parseSetRequest Part count " + QString::number(partCount));

    QHash<DBTable*, DBRecordList> downloadedRecords;
    for(int partIndex = 0; partIndex < partCount; partIndex++)
    {
        // Считаю что хедеров не больше 2:
        QByteArrayList headers;
        const int eoll = QByteArray(EOL).length();
        const int i1 = boundaryIndeces[partIndex] + boundary.size() + eoll;
        const int i2 = request.indexOf(EOL, i1) + eoll;
        const int i3 = request.indexOf(EOL, i2) + eoll;
        headers.append(request.mid(i1, i2 - i1 - eoll));
        headers.append(request.mid(i2, i3 - i2 - eoll));
        if(partIndex == 0) // Value
        {
            for(int hi = 0; hi < headers.count(); hi++)
            {
                QByteArray& header = headers[hi];
                // Content-Disposition: form-data; name="value"
                QByteArray nameItemValue = parseHeaderItem(header, "name");
                if(nameItemValue != "value") continue;

                Tools::debugLog(QString("@@@@@ NetServer::parseSetRequest. Part %1, header %2 = %3").
                        arg(QString::number(partIndex), QString::number(hi), header));
                QString text = toJsonString(request.mid(i3 + eoll, boundaryIndeces[partIndex + 1] - i3 - eoll));
                Tools::debugLog("@@@@@ NetServer::parseSetRequest. Text " + text);
                result.recordCount += appManager->onParseSetRequest(text, downloadedRecords);
            }
        }
        else // Resources
        {
            for(int hi = 0; hi < headers.count(); hi++)
            {
                QByteArray& header = headers[hi];
                QList tables = downloadedRecords.keys();
                for (DBTable* table : tables)
                {
                    Tools::debugLog(QString("@@@@@ NetServer::parseSetRequest. Part %1, header %2 = %3").
                            arg(QString::number(partIndex), QString::number(hi), header));
                    Tools::debugLog("@@@@@ NetServer::parseSetRequest. Table " + table->name);
                    DBRecordList tableRecords = downloadedRecords.value(table);
                    bool isNewRecord = false;
                    for(int ri = 0; ri < tableRecords.count(); ri++)
                    {
                        // Например, Content-Disposition: form-data; name="file1"; filename=":/Images/image.png"
                        QByteArray byteSource = parseHeaderItem(header, "filename");
                        QString source = QString(byteSource.replace('\\', '/'));
                        if(source.isEmpty()) continue;
                        DBRecord& record = tableRecords[ri];
                        const QString& recordCode = record[0].toString();
                        QString fullPath;
                        QString localFilePath;
                        QByteArray fileData;

                        if(table->name == DBTABLENAME_MESSAGES ||
                           table->name == DBTABLENAME_MESSAGEFILES ||
                           table->name == DBTABLENAME_PICTURES ||
                           table->name == DBTABLENAME_MOVIES ||
                           table->name == DBTABLENAME_SOUNDS)
                        {
                            // New resource file name (tableName/recordCode.extension):
                            const int dotIndex = source.lastIndexOf(".");
                            const QString extension = dotIndex < 0 ? "" : source.mid(dotIndex + 1, source.length());
                            localFilePath = QString("%1/%2.%3").arg(table->name, recordCode, extension);
                            // Write data file:
                            fileData = request.mid(i3 + eoll, boundaryIndeces[partIndex + 1] - i3 - eoll * 2);
                            Tools::debugLog("@@@@@ NetServer::parseSetRequest. File data length " + QString::number( fileData.length()));
                            fullPath = Tools::downloadPath(localFilePath);
                            Tools::debugLog("@@@@@ NetServer::parseSetRequest. Full file path " + fullPath);
                        }
                        else if(table->name == DBTABLENAME_LABELS)
                        {
                            // New resource file name = tableName/recordCode/source:
                            localFilePath = QString("%1/%2/%3").arg(table->name, recordCode, source).replace(":", "").replace("//", "/");
                            fullPath = Tools::downloadPath(localFilePath);
                            Tools::debugLog("@@@@@ NetServer::parseSetRequest. Resource path " + localFilePath);
                            Tools::debugLog("@@@@@ NetServer::parseSetRequest. Full path " + fullPath);

                            // Write data file:
                            fileData = request.mid(i3 + eoll, boundaryIndeces[partIndex + 1] - i3 - eoll * 2);
                            Tools::debugLog("@@@@@ NetServer::parseSetRequest. File data length " + QString::number(fileData.length()));
                        }
                        if(!fullPath.isEmpty())
                        {
                            if(Tools::writeBinaryFile(fullPath, fileData))
                            {
                                if(record[ResourceDBTable::Value].toString().isEmpty())
                                {
                                    record[ResourceDBTable::Source] = source;
                                    record[ResourceDBTable::Value] = localFilePath;
                                    isNewRecord = true;

                                    QString s = "@@@@@ NetServer::parseSetRequest: ";
                                    s += " Table = " + table->name;
                                    s += " Record = " + table->toString(record);
                                    Tools::debugLog(s);
                                }
                            }
                            else
                            {
                                result.errorCount++;
                                result.errorCode = Error_Log_WrongRecord;
                                record = table->createRecord(recordCode);
                                Tools::debugLog("@@@@@ NetServer::parseSetRequest. Write file ERROR");
                            }
                        }
                        else
                        {
                            result.errorCount++;
                            result.errorCode = Error_Log_UnknownTable;
                            record = table->createRecord();
                            Tools::debugLog("@@@@@ NetServer::parseSetRequest. Unknown table ERROR");
                            continue;
                        }
                    }
                    if(isNewRecord)
                    {
                        downloadedRecords.remove(table);
                        downloadedRecords.insert(table, tableRecords);
                    }
                }
            }
        }
    }
    appManager->netDownload(downloadedRecords, result.successCount, result.errorCount);
    appManager->status.downloadedRecordCount += result.successCount;
    result.description = QString("Загружено записей %1 из %2").arg(
                QString::number(result.successCount),
                QString::number(result.recordCount));
    showToast(result.description);
    result.requestReply = result.makeEmptyJson();
    return result;
}

