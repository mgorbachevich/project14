#include "settings.h"
#include "tools.h"
#include "settingdbtable.h"
#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

QString Settings::getStringValue(const DBRecord& r)
{
    if(getType(r) != SettingType_List) return (r.at(SettingDBTable::Value)).toString();
    QStringList values = getValueList(r);
    int i = Tools::stringToInt((r.at(SettingDBTable::Value)).toString());
    return (i < values.count()) ? values[i].trimmed() : "";
}

QString Settings::getStringValue(const SettingCode code)
{
    DBRecord* r = getByCode(code);
    return r == nullptr ? "" : getStringValue(*r);
}

int Settings::getIntValue(const SettingCode code, const bool listIndex)
{
    DBRecord* r = getByCode(code);
    return r == nullptr ? 0 : getIntValue(*r, listIndex);
}

int Settings::getIntValue(const DBRecord& r, const bool listIndex)
{
    return (listIndex && getType(r) == SettingType_List) ?
            Tools::stringToInt((r.at(SettingDBTable::Value)).toString()) :
            Tools::stringToInt(getStringValue(r));
}

bool Settings::getBoolValue(const SettingCode code)
{
    return getIntValue(code, true) != 0;
}

DBRecord* Settings::getByCode(const int code)
{
    bool ok = false;
    for (DBRecord& r : items) if (r[SettingDBTable::Code].toInt(&ok) == code && ok) return &r;
    return nullptr;
}

DBRecord *Settings::getByIndexInCurrentGroup(const int indexInGroup)
{
    QList<int> currentGroupItemCodes = getCurrentGroupCodes();
    int ii = 0;
    for (int i = 0; i < currentGroupItemCodes.count(); i++)
    {
        DBRecord* ri = getByCode(currentGroupItemCodes[i]);
        if (ii == indexInGroup) return ri;
        ii++;
    }
    return nullptr;
}

QList<int> Settings::getCurrentGroupCodes()
{
    QList<int> codes;
    for (DBRecord& r : items)
    {
        int groupCode = r[SettingDBTable::GroupCode].toInt();
        if (groupCode == currentGroupCode) codes.append(r[SettingDBTable::Code].toInt());
    }
    return codes;
}

bool Settings::isGroup(const DBRecord& r)
{
    return getType(r) == SettingType_Group;
}

void Settings::sort()
{
    Tools::sortByInt(items, SettingDBTable::Code);
}

int Settings::getCode(const DBRecord& r)
{
    return r.count() >= SettingDBTable::Code ? r[SettingDBTable::Code].toInt() : 0;
}

int Settings::getType(const DBRecord& r)
{
    return r.count() >= SettingDBTable::Type ? r[SettingDBTable::Type].toInt() : SettingType_Group;
}

QString Settings::getName(const DBRecord& r)
{
    return r.count() >= SettingDBTable::Name ? r[SettingDBTable::Name].toString() : "";
}

QStringList Settings::getValueList(const DBRecord& r)
{
    return r.count() >= SettingDBTable::ValueList ?
        r[SettingDBTable::ValueList].toString().split(',') :
        QStringList();
}

QString Settings::getCurrentGroupName()
{
    DBRecord* r = getByCode(currentGroupCode);
    return (r != nullptr && r->count() >= SettingDBTable::Name) ?
                r->at(SettingDBTable::Name).toString() : "Настройки";
}

void Settings::update(const DBRecordList& records)
{
    Tools::debugLog("@@@@@ Settings::update ");
    items.clear();
    items.append(records);
}

bool Settings::onInputValue(const int itemCode, const QString& value)
{
    Tools::debugLog(QString("@@@@@ Settings::onInputValue %1 %2").arg(Tools::intToString(itemCode), value));
    for (DBRecord& r : items)
    {
        if (r[SettingDBTable::Code].toInt() == itemCode)
        {
            if(0 != QString::compare(r[SettingDBTable::Value].toString(), value) && checkValue(r, value))
            {
                switch(getType(r))
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

bool Settings::checkValue(const DBRecord& record, const QString& value)
{
    switch (record[SettingDBTable::Code].toInt())
    {
    case SettingCode_ScalesNumber:
        if(Tools::stringToInt(value) == 0)
        {
            message += "\n" + getName(record) + ". Неверное значение";
            return false;
        }
        if(value.trimmed().length() > 6)
        {
            message += "\n" + getName(record) + ". Длина должна быть не больше 6";
            return false;
        }
        break;
    case SettingCode_SerialScalesNumber:
        if(Tools::stringToInt(value) == 0)
        {
            message += "\n" + getName(record) + ". Неверное значение";
            return false;
        }
        if(value.trimmed().length() > 7)
        {
            message += "\n" + getName(record) + ". Длина должна быть не больше 7";
            return false;
        }
        if(getIntValue(SettingCode_ScalesName, true) == 0)
        {
            message += "\n" + getName(record) + ". Необходимо выбрать модель весов";
            return false;
        }
        break;
    case SettingCode_PrintLabelPrefixWeight:
    case SettingCode_PrintLabelPrefixPiece:
        if(value.trimmed().length() != 2)
        {
            message += "\n" + getName(record) + ". Длина должна быть равна 2";
            return false;
        }
    default:
        break;
    }
    return true;
}

int Settings::nativeSettings(const int code) // return error
{
    Tools::debugLog("@@@@@ Settings::nativeSettings " + QString::number(code));

#ifdef Q_OS_ANDROID
    switch (code)
    {
    case SettingCode_WiFi:
    case SettingCode_SystemSettings:
    case SettingCode_Equipment:
    case SettingCode_Ethernet:
    {
        //https://stackoverflow.com/questions/3872063/how-to-launch-an-activity-from-another-application-in-android
        //https://developer.android.com/training/package-visibility
        jint result = QJniObject::callStaticMethod<jint>(ANDROID_NATIVE_CLASS_NAME, "startNativeActivity",
                    "(Landroid/content/Context;I)I", QNativeInterface::QAndroidApplication::context(), code);
        Tools::debugLog("@@@@@ Settings::nativeSettings SettingCode_Equipment result " + QString::number(result));
        return result;
    }
    default:
        qDebug("@@@@@ Settings::nativeSettings: Unknown code");
        return -1;
    }
#endif // Q_OS_ANDROID

    qDebug("@@@@@ Settings::nativeSettings: Unknown param");
    return -1;
}

EquipmentUris Settings::parseEquipmentConfig(const QString& fileName)
{
    Tools::debugLog("@@@@@ Settings::parseEquipmentConfig fileName " + fileName);
    EquipmentUris eu;
    if(!QFileInfo::exists(fileName))
        Tools::debugLog("@@@@@ Settings::parseEquipmentConfig ERROR ");
    else
    {
        QString s = Tools::readTextFile(fileName);
        Tools::debugLog("@@@@@ Settings::parseEquipmentConfig read " + s);
        const QJsonObject jo = Tools::stringToJson(s);
        eu.wmUri = jo["WmUri"].toString();
        eu.printerUri = jo["PrinterUri"].toString();
        Tools::debugLog(QString("@@@@@ Settings::parseEquipmentConfig uris %1, %2").arg(eu.wmUri, eu.printerUri));
    }
    return eu;
}

QString Settings::getAndClearMessage()
{
    QString s = message;
    message = "";
    return s;
}

void Settings::onShow()
{
    Tools::debugLog("@@@@@ ASettings::onShow");
    (*getByCode(SettingCode_VerificationName))[SettingDBTable::Value] = QString("%1 %2").arg(
                getStringValue(*getByCode(SettingCode_ScalesName)),
                getStringValue(*getByCode(SettingCode_SerialScalesNumber)));
}


