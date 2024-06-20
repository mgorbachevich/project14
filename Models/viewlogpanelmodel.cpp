#include "viewlogpanelmodel.h"
#include "tools.h"
#include "logdbtable.h"

void ViewLogPanelModel::update(const DBRecordList& records)
{
    Tools::debugLog("@@@@@ ViewLogPanelModel::update");
    QStringList ss;
    for (int i = 0; i < records.count(); i++)
    {
        DBRecord ri = records[i];
        QString dateTime = Tools::dateTimeFromUInt(ri[LogDBTable::DateTime].toLongLong(), "%1 %2", DATE_FORMAT, TIME_FORMAT);
        QString comment = ri[LogDBTable::Comment].toString();
        QString source = ri[LogDBTable::Source].toString();
        QString type = ri[LogDBTable::Type].toString();
        switch(ri[LogDBTable::Type].toInt())
        {
        case LogType_Debug: type = "D"; break;
        case LogType_Error: type = "E"; break;
        case LogType_Info: type = "I"; break;
        case LogType_Warning: type = "W"; break;
        case LogType_Transaction: type = "T"; break;
        }
        ss << QString("%1   %2   %3   %4").arg(dateTime, type, source, comment);
    }
    setStringList(ss);
}
