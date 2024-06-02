#include <QJsonObject>
#include <QJsonDocument>
#include "settings.h"
#include "tools.h"

#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

Settings::Settings(AppManager *parent): JsonArrayFile(SETTINGS_FILE, parent)
{
    Tools::debugLog("@@@@@ Settings::Settings");
    mainObjectName = "data";
    itemArrayName = "settings";
    fields.insert(SettingField_Code,      "code");
    fields.insert(SettingField_Type,      "type");
    fields.insert(SettingField_GroupCode, "group_code");
    fields.insert(SettingField_Name,      "name");
    fields.insert(SettingField_Value,     "value");
    fields.insert(SettingField_ValueList, "value_list");
    fields.insert(SettingField_Comment,   "comment");
    scaleConfig = new ScaleConfig(parent);
}

void Settings::apply()
{
#ifdef Q_OS_ANDROID
    Tools::debugLog("@@@@@ Settings::apply");
#endif
}

bool Settings::checkValue(const DBRecord& record, const QString& value)
{
    switch (getCode(record))
    {
    case SettingCode_ScalesNumber:
        if(Tools::stringToInt(value) == 0)
        {
           showAttention(getName(record) + ". Неверное значение");
            return false;
        }
        if(value.trimmed().length() > 6)
        {
            showAttention(getName(record) + ". Длина должна быть не больше 6");
            return false;
        }
        break;
    case SettingCode_SerialScalesNumber:
        if(Tools::stringToInt(value) == 0)
        {
            showAttention(getName(record) + ". Неверное значение");
            return false;
        }
        if(value.trimmed().length() > 7)
        {
            showAttention(getName(record) + ". Длина должна быть не больше 7");
            return false;
        }
        if(getIntValue(SettingCode_ScalesName, true) == 0)
        {
            showAttention(getName(record) + ". Необходимо выбрать модель весов");
            return false;
        }
        break;
    case SettingCode_PrintLabelPrefixWeight:
    case SettingCode_PrintLabelPrefixPiece:
        if(value.trimmed().length() != 2)
        {
            showAttention(getName(record) + ". Длина должна быть равна 2");
            return false;
        }
    default:
        break;
    }
    return true;
}

bool Settings::parseDefault() // Добавление недостающих значений по умолчанию:
{
    DBRecordList defaultRecords = parse(Tools::readTextFile(DEFAULT_SETTINGS_FILE));
    for(int i = 0; i < SettingCode_Max; i++) checkDefaultRecord(i, defaultRecords);
    return items.count() > 0;
}

bool Settings::isGroup(const DBRecord &r)
{
    return getType(r) == SettingType_Group || getType(r) == SettingType_GroupWithPassword;
}

void Settings::checkDefaultRecord(const int code, DBRecordList& defaults)
{
    for (DBRecord& r : items)    if (getCode(r) == code) return; // уже есть такая запись
    for (DBRecord& r : defaults) if (getCode(r) == code) { items.append(r); return; }
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

QList<int> Settings::getCurrentGroupCodes()
{
    getAll();
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

void Settings::nativeSettings(const int code)
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
        switch(result)
        {
        case 0: // Ошибок нет
            break;
        case -1:
            showAttention("ОШИБКА! Неизвестный параметр");
            break;
        case -2:
            showAttention("ОШИБКА! Неверный вызов");
            break;
        default:
            showAttention("ОШИБКА! Неизвестное значение");
            break;
        }
        break;
    }
    default:
        qDebug("@@@@@ Settings::nativeSettings: Unknown code");
        showAttention("ОШИБКА! Неизвестный параметр");
        break;
    }
#else
    qDebug("@@@@@ Settings::nativeSettings: Unknown param");
    showAttention("Не поддерживается");
#endif
}

bool Settings::setValue(const int itemCode, const QString& value)
{
    Tools::debugLog(QString("@@@@@ Settings::setValue %1 %2").arg(Tools::intToString(itemCode), value));
    getAll();
    for (DBRecord& r : items)
    {
        if (getCode(r) == itemCode)
        {
            if(0 != QString::compare(r[SettingField_Value].toString(), value) && checkValue(r, value))
            {
                r[SettingField_Value] = value;
                return true;
                /*
                switch(getType(r))
                {
                case SettingType_InputNumber:
                case SettingType_InputText:
                case SettingType_IntervalNumber:
                case SettingType_List:
                    r[SettingField_Value] = value;
                    return true;
                }
                */
            }
            return false;
        }
    }
    return false;
}

void Settings::update(const int groupCode)
{
    Tools::debugLog("@@@@@ Settings::update");
    currentGroupCode = groupCode;
    (*getByCode(SettingCode_VerificationName))[SettingField_Value] = QString("%1 %2").arg(
                getStringValue(*getByCode(SettingCode_ScalesName)),
                getStringValue(*getByCode(SettingCode_SerialScalesNumber)));
}

bool Settings::read()
{
    Tools::debugLog("@@@@@ Settings::read");
    bool ok = JsonArrayFile::read();
    scaleConfig->read();
    (*getByCode(SettingCode_ScalesName))[SettingField_Value] = scaleConfig->get(ScaleConfigField_Model);
    (*getByCode(SettingCode_SerialScalesNumber))[SettingField_Value] = scaleConfig->get(ScaleConfigField_SerialNumber);
    (*getByCode(SettingCode_VerificationDate))[SettingField_Value] = scaleConfig->get(ScaleConfigField_VerificationDate);
    return ok;
}

bool Settings::write()
{
    Tools::debugLog("@@@@@ Settings::write");
    scaleConfig->set(ScaleConfigField_Model, (*getByCode(SettingCode_ScalesName))[SettingField_Value]);
    scaleConfig->set(ScaleConfigField_ModelName, getStringValue(SettingCode_ScalesName));
    scaleConfig->set(ScaleConfigField_SerialNumber, (*getByCode(SettingCode_SerialScalesNumber))[SettingField_Value]);
    scaleConfig->set(ScaleConfigField_VerificationDate, (*getByCode(SettingCode_VerificationDate))[SettingField_Value]);
    scaleConfig->write();
    apply();
    return JsonArrayFile::write();
}

void Settings::sort()
{
    getAll();
    Tools::sortByInt(items, SettingField_Code);
}

