#include <QJsonObject>
#include <QJsonDocument>
#include "tools.h"
#include "settings.h"

#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

Settings::Settings(QObject *parent): JsonFile(CONFIG_FILE, parent)
{
    mainObjectName = "data";
    itemArrayName = "settings";
    fields.insert(SettingField_Code,      "code");
    fields.insert(SettingField_Type,      "type");
    fields.insert(SettingField_GroupCode, "group_code");
    fields.insert(SettingField_Name,      "name");
    fields.insert(SettingField_Value,     "value");
    fields.insert(SettingField_ValueList, "value_list");
}

bool Settings::checkValue(const DBRecord& record, const QString& value)
{
    switch (getCode(record))
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

bool Settings::read()
{
    parse();

    // Добавление недостающих значений по умолчанию:
    DBRecordList defaultRecords = parse(Tools::readTextFile(DEFAULT_SETTINGS_FILE));
    for(int i = 0; i < SettingCode_Max; i++) checkDefaultRecord(i, defaultRecords);
    Tools::debugLog("@@@@@ JsonSettings::read " + Tools::intToString(items.count()));
    return items.count() > 0;
}

void Settings::checkDefaultRecord(const int code, DBRecordList& defaults)
{
    for (DBRecord& r : items)    if (getCode(r) == code) return; // уже есть такая запись
    for (DBRecord& r : defaults) if (getCode(r) == code) { items.append(r); return; }
}

bool Settings::getBoolValue(const SettingCode code)
{
    return getIntValue(code, true) != 0;
}

DBRecord *Settings::getByCode(const int code)
{
    bool ok = false;
    for (DBRecord& r : items) if (r[SettingField_Code].toInt(&ok) == code && ok) return &r;
    return nullptr;
}

QVariantList *Settings::getByIndexInCurrentGroup(const int indexInGroup)
{
    QList<int> groupCodes = getCurrentGroupCodes();
    int ii = 0;
    for (int i = 0; i < groupCodes.count(); i++)
    {
        DBRecord* ri = getByCode(groupCodes[i]);
        if (ii == indexInGroup) return ri;
        ii++;
    }
    return nullptr;
}

int Settings::getCode(const DBRecord& r)
{
    return r[SettingField_Code].toInt();
}

QList<int> Settings::getCurrentGroupCodes()
{
    QList<int> codes;
    for (DBRecord& r : items)
    {
        int groupCode = r[SettingField_GroupCode].toInt();
        if (groupCode == currentGroupCode) codes.append(getCode(r));
    }
    return codes;
}

QString Settings::getCurrentGroupName()
{
    DBRecord* r = getByCode(currentGroupCode);
    return r != nullptr ? r->at(SettingField_Name).toString() : "Настройки";
}

int Settings::getIntValue(const SettingCode code, const bool listIndex)
{
    DBRecord* r = getByCode(code);
    return r == nullptr ? 0 : getIntValue(*r, listIndex);
}

int Settings::getIntValue(const DBRecord& r, const bool listIndex)
{
    return (listIndex && getType(r) == SettingType_List) ?
            Tools::stringToInt((r.at(SettingField_Value)).toString()) :
                Tools::stringToInt(getStringValue(r));
}

QString Settings::getName(const DBRecord& r)
{
    return r[SettingField_Name].toString();
}

QString Settings::getStringValue(const DBRecord& r)
{
    if(getType(r) != SettingType_List) return (r.at(SettingField_Value)).toString();
    QStringList values = getValueList(r);
    int i = Tools::stringToInt((r.at(SettingField_Value)).toString());
    return (i < values.count()) ? values[i].trimmed() : "";
}

QString Settings::getStringValue(const SettingCode code)
{
    DBRecord* r = getByCode(code);
    return r == nullptr ? "" : getStringValue(*r);
}

int Settings::getType(const DBRecord& r)
{
    return r[SettingField_Type].toInt();
}

QStringList Settings::getValueList(const DBRecord& r)
{
    return r[SettingField_ValueList].toString().split(',');
}

bool Settings::isGroup(const DBRecord& r)
{
    return getType(r) == SettingType_Group;
}

int Settings::nativeSettings(const int code) // return error
{
    Tools::debugLog("@@@@@ JsonSettings::nativeSettings " + QString::number(code));

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
        Tools::debugLog("@@@@@ JsonSettings::nativeSettings SettingCode_Equipment result " + QString::number(result));
        return result;
    }
    default:
        qDebug("@@@@@ JsonSettings::nativeSettings: Unknown code");
        return -1;
    }
#endif // Q_OS_ANDROID

    qDebug("@@@@@ JsonSettings::nativeSettings: Unknown param");
    return -1;
}

bool Settings::onInputValue(const int itemCode, const QString& value)
{
    Tools::debugLog(QString("@@@@@ JsonSettings::onInputValue %1 %2").arg(Tools::intToString(itemCode), value));
    for (DBRecord& r : items)
    {
        if (getCode(r) == itemCode)
        {
            if(0 != QString::compare(r[SettingField_Value].toString(), value) && checkValue(r, value))
            {
                switch(getType(r))
                {
                case SettingType_InputNumber:
                case SettingType_InputText:
                case SettingType_IntervalNumber:
                case SettingType_List:
                    r[SettingField_Value] = value;
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

void Settings::onShow()
{
    Tools::debugLog("@@@@@ JsonSettings::onShow");
    (*getByCode(SettingCode_VerificationName))[SettingField_Value] = QString("%1 %2").arg(
                getStringValue(*getByCode(SettingCode_ScalesName)),
                getStringValue(*getByCode(SettingCode_SerialScalesNumber)));
}

EquipmentUris Settings::parseEquipmentConfig(const QString& fileName)
{
    Tools::debugLog("@@@@@ JsonSettings::parseEquipmentConfig fileName " + fileName);
    EquipmentUris eu;
    if(!QFileInfo::exists(fileName))
        Tools::debugLog("@@@@@ JsonSettings::parseEquipmentConfig ERROR ");
    else
    {
        QString s = Tools::readTextFile(fileName);
        Tools::debugLog("@@@@@ JsonSettings::parseEquipmentConfig read " + s);
        const QJsonObject jo = Tools::stringToJson(s);
        eu.wmUri = jo["WmUri"].toString();
        eu.printerUri = jo["PrinterUri"].toString();
        Tools::debugLog(QString("@@@@@ JsonSettings::parseEquipmentConfig uris %1, %2").arg(eu.wmUri, eu.printerUri));
    }
    return eu;
}

void Settings::sort()
{
    Tools::sortByInt(items, SettingField_Code);
}

