#include "settings.h"
#include "tools.h"
#include "settingdbtable.h"
#include <QtCore/private/qandroidextras_p.h>

QString Settings::getItemStringValue(const DBRecord& r)
{
    if(getItemType(r) != SettingType_List) return (r.at(SettingDBTable::Value)).toString();
    QStringList values = getItemValueList(r);
    int i = Tools::stringToInt((r.at(SettingDBTable::Value)).toString());
    if(i < values.count()) return values[i].trimmed();
    return "";
}

QString Settings::getItemStringValue(const SettingCode code)
{
    DBRecord* r = getItemByCode(code);
    return r == nullptr ? "" : getItemStringValue(*r);
}

int Settings::getItemIntValue(const SettingCode code)
{
    DBRecord* r = getItemByCode(code);
    return r == nullptr ? 0 : getItemIntValue(*r);
}

int Settings::getItemIntValue(const DBRecord& r)
{
    return Tools::stringToInt((r.at(SettingDBTable::Value)).toString());
}

bool Settings::getItemBoolValue(const SettingCode code)
{
    return getItemIntValue(code) != 0;
}

DBRecord* Settings::getItemByCode(const int code)
{
    bool ok = false;
    for (DBRecord& r : items) if (r[SettingDBTable::Code].toInt(&ok) == code && ok) return &r;
    return nullptr;
}

DBRecord *Settings::getItemByIndexInCurrentGroup(const int indexInGroup)
{
    QList<int> currentGroupItemCodes = getCurrentGroupItemCodes();
    int ii = 0;
    for (int i = 0; i < currentGroupItemCodes.count(); i++)
    {
        DBRecord* ri = getItemByCode(currentGroupItemCodes[i]);
        if (ii == indexInGroup) return ri;
        ii++;
    }
    return nullptr;
}

QList<int> Settings::getCurrentGroupItemCodes()
{
    QList<int> codes;
    for (DBRecord& r : items)
    {
        int groupCode = r[SettingDBTable::GroupCode].toInt();
        if (groupCode == currentGroupCode) codes.append(r[SettingDBTable::Code].toInt());
    }
    qDebug() << "@@@@@ Settings::getCurrentGroupItemCodes " << codes;
    return codes;
}

bool Settings::isGroupItem(const DBRecord& r)
{
    return getItemType(r) == SettingType_Group;
}

void Settings::sort()
{
    /*
    // https://copyprogramming.com/howto/how-to-sort-qlist-qvariant-in-qt
    int i, n;
    for (n = 0; n < items.count(); n++)
    {
        for (i = n + 1; i < items.count(); i++)
        {
            if (items[n][0].toInt() > items[i][0].toInt())
            {
                items.move(i, n);
                n = 0;
            }
        }
    }
    */
    Tools::sortByInt(items, SettingDBTable::Code);
}

int Settings::getItemCode(const DBRecord& r)
{
    return r.count() >= SettingDBTable::Code ? r[SettingDBTable::Code].toInt() : 0;
}

int Settings::getItemType(const DBRecord& r)
{
    return r.count() >= SettingDBTable::Type ? r[SettingDBTable::Type].toInt() : SettingType_Group;
}

QString Settings::getItemName(const DBRecord& r)
{
    return r.count() >= SettingDBTable::Name ? r[SettingDBTable::Name].toString() : "";
}

QStringList Settings::getItemValueList(const DBRecord& r)
{
    return r.count() >= SettingDBTable::ValueList ? r[SettingDBTable::ValueList].toString().split(',') :
        QStringList();
}

QString Settings::getCurrentGroupName()
{
    DBRecord* r = getItemByCode(currentGroupCode);
    return (r != nullptr && r->count() >= SettingDBTable::Name) ? r->at(SettingDBTable::Name).toString() : "Настройки";
}

void Settings::update(const DBRecordList& records)
{
    qDebug() << "@@@@@ Settings::update ";
    items.clear();
    items.append(records);
}

bool Settings::onManualInputItemValue(const int itemCode, const QString& value)
{
    for (DBRecord& r : items)
    {
        if (r[SettingDBTable::Code].toInt() == itemCode)
        {
            if(r[SettingDBTable::Value].toString() != value)
            {
                switch(getItemType(r))
                {
                case SettingType_InputNumber:
                case SettingType_InputText:
                case SettingType_IntervalNumber:
                case SettingType_List:
                    r[SettingDBTable::Value] = value;
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}
/*
int Settings::getBoudrate(const int code)
{
    switch (code)
    {
    case WMBaudrate_2400:   return 2400;
    case WMBaudrate_4800:   return 4800;
    case WMBaudrate_9600:   return 9600;
    case WMBaudrate_19200:  return 19200;
    case WMBaudrate_38400:  return 38400;
    case WMBaudrate_57600:  return 57600;
    case WMBaudrate_115200: return 115200;
    }
    return 9600;
}
*/

bool Settings::nativeSettings(const int code)
{
    qDebug() << "@@@@@ Settings::nativeSettings " << code;

#ifdef Q_OS_ANDROID // --------------------------------------------------------
    switch (code)
    {
    case SettingCode_WiFi:
    case SettingCode_SystemSettings:
    case SettingCode_Equipment:
    {
        //https://stackoverflow.com/questions/3872063/how-to-launch-an-activity-from-another-application-in-android
        //https://developer.android.com/training/package-visibility
        jint result = QJniObject::callStaticMethod<jint>(
                    "ru.shtrih_m.project14/AndroidNative",
                    "nativeMethod",
                    "(Landroid/content/Context;I)I",
                    QNativeInterface::QAndroidApplication::context(), code);
        qDebug() << "@@@@@ Settings::nativeSettings SettingCode_Equipment result " << result;
        return true;
    }

    case SettingCode_Ethernet:
    default:
        qDebug("@@@@@ Settings::nativeSettings: Unknown code");
        return false;
    }
#endif // Q_OS_ANDROID --------------------------------------------------------

    qDebug("@@@@@ Settings::nativeSettings: Unknown param");
    return false;
}

QList<QString> Settings::parseEquipmentConfig(const QString& fileName)
{
    qDebug() << "@@@@@ Settings::parseEquipmentConfig fileName = "<< fileName;
    QList<QString> result;
    if(QFileInfo::exists(fileName))
    {
        const QJsonObject jo = Tools::stringToJson(Tools::readTextFile(fileName));
        result.append(jo["WmUri"].toString());
        result.append(jo["PrinterUri"].toString());
    }
    qDebug() << "@@@@@ Settings::parseEquipmentConfig uris = "<< result;
    return result;
}


