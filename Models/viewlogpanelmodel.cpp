#include "viewlogpanelmodel.h"
#include "tools.h"
#include "logdbtable.h"

void ViewLogPanelModel::update(const DBRecordList& records)
{
    qDebug() << "@@@@@ ViewLogPanelModel::update";
    QStringList ss;
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord ri = records[i];
        QString dateTime = Tools::dateTimeFromInt(ri[LogDBTable::DateTime].toLongLong());
        QString comment = ri[LogDBTable::Comment].toString();
        QString type = "";
        switch(ri[LogDBTable::Type].toInt())
        {
        case LogDBTable::LogType_Debug: type = "D"; break;
        case LogDBTable::LogType_Error: type = "E"; break;
        case LogDBTable::LogType_Info: type = "I"; break;
        case LogDBTable::LogType_Warning: type = "W"; break;
        case LogDBTable::LogType_Transaction: type = "T"; break;
        }
        ss << QString("%1 %2 %3").arg(dateTime, type, comment);
    }
    setStringList(ss);
}
