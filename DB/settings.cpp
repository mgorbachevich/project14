#include <QJsonArray>
#include <QJsonDocument>
#include "settings.h"
#include "tools.h"
#include "appmanager.h"
#include "resourcedbtable.h"

#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

Settings::Settings(AppManager *parent): JsonArrayFile(SETTINGS_FILE, parent)
{
    Tools::debugLog("@@@@@ Settings::Settings");
    itemArrayName = DBTABLENAME_SETTINGS;
    fields.insert(SettingField_Code,      "code");
    fields.insert(SettingField_Type,      "type");
    fields.insert(SettingField_GroupCode, "group_code");
    fields.insert(SettingField_Name,      "name");
    fields.insert(SettingField_Value,     "value");
    fields.insert(SettingField_ValueList, "value_list");
    fields.insert(SettingField_Comment,   "comment");
    scaleConfig = new ScaleConfig(parent);

    modelNames.append(QPair<QString, QString>("Не задано",         "Unknown"));
    modelNames.append(QPair<QString, QString>("Штрих Принт 6 ФA1", "Shtrih_Print_6_FA1"));
    modelNames.append(QPair<QString, QString>("Штрих Принт 6 ФA2", "Shtrih_Print_6_FA2"));
    modelNames.append(QPair<QString, QString>("Штрих Принт 6 MA1", "Shtrih_Print_6_MA1"));
    modelNames.append(QPair<QString, QString>("Штрих Принт 6 MA2", "Shtrih_Print_6_MA2"));
    modelNames.append(QPair<QString, QString>("Штрих Принт 6 СА",  "Shtrih_Print_6_SA"));
}

bool Settings::checkValue(const DBRecord& record, const QString& value)
{
    switch (getCode(record))
    {
    case SettingCode_ScalesNumber:
        if(Tools::toInt(value) == 0)
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
    case SettingCode_SerialNumber:
        if(Tools::toInt(value) == 0)
        {
            showAttention(getName(record) + ". Неверное значение");
            return false;
        }
        if(value.trimmed().length() > 7)
        {
            showAttention(getName(record) + ". Длина должна быть не больше 7");
            return false;
        }
        if(getIntValue(SettingCode_Model, true) == 0)
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
    default: break;
    }
    return true;
}

bool Settings::isGroup(const DBRecord &r)
{
    return getType(r) == SettingType_Group || getType(r) == SettingType_GroupWithPassword;
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
            Tools::toInt((r.at(SettingField_Value)).toString()) :
                Tools::toInt(getStringValue(r));
}

QString Settings::getStringValue(const DBRecord& r)
{
    if(getType(r) != SettingType_List) return (r.at(SettingField_Value)).toString();
    QStringList values = getValueList(r);
    int i = Tools::toInt((r.at(SettingField_Value)).toString());
    return (i < values.count()) ? values[i].trimmed() : "";
}

QString Settings::getStringValue(const SettingCode code)
{
    DBRecord* r = getByCode(code);
    return r == nullptr ? "" : getStringValue(*r);
}

QStringList Settings::getValueList(const DBRecord &r)
{
    return Tools::toStringList(r[SettingField_ValueList].toString());
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
        Tools::debugLog("@@@@@ Settings::nativeSettings: Unknown code");
        showAttention("ОШИБКА! Неизвестный параметр");
        break;
    }
#else
    Tools::debugLog("@@@@@ Settings::nativeSettings: Unknown param");
    showAttention("Не поддерживается");
#endif
}

void Settings::appendItemToJson(DBRecord& r, QJsonArray& ja)
{
    QJsonObject ji;
    ji.insert(fields.value(SettingField_Name), r.at(SettingField_Name).toJsonValue());
    ji.insert(fields.value(SettingField_Code), r.at(SettingField_Code).toJsonValue());
    ji.insert(fields.value(SettingField_Value), r.at(SettingField_Value).toJsonValue());
    ja.append(ji);
}

void Settings::setConfigValue(const ScaleConfigField f, const QString &v)
{
    Tools::debugLog(QString("@@@@@ Settings::setConfigValue %1 %2").arg(Tools::toString((int)f), v));
    scaleConfig->set(f, v);
}

bool Settings::setValue(const int itemCode, const QString& value)
{
    Tools::debugLog(QString("@@@@@ Settings::setValue %1 %2").arg(Tools::toString(itemCode), value));
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

bool Settings::write()
{
    Tools::debugLog("@@@@@ Settings::write");
    toConfig();
    bool ok = scaleConfig->write();
    QJsonObject data;
    data.insert("data", toJsonObject());
    ok &= Tools::writeTextFile(fileName, Tools::toString(data));
    Tools::debugLog(QString("@@@@@ Settings::write %1 %2").arg(fileName, Tools::sortIncrement(ok)));
    showToast(ok ? "Настройки сохранены" : "ОШИБКА СОХРАНЕНИЯ НАСТРОЕК!");
    wasRead = false;
    return ok;
}

bool Settings::read()
{
    Tools::debugLog("@@@@@ Settings::read");
    if(wasRead) return items.count() > 0;
    wasRead = true;
    items = parse(Tools::readTextFile(DEFAULT_SETTINGS_FILE));
    DBRecordList loadedItems = parse(Tools::readTextFile(fileName));
    for (DBRecord& item : items) // Replace default values
    {
        for (DBRecord& r : loadedItems)
        {
            if(getCode(item) == getCode(r))
            {
                item[SettingField_Value] = r[SettingField_Value];
                break;
            }
        }
    }
    bool ok = items.count() > 0;
    if(ok)
    {
        QString s;
        for(int i = 0; i < modelNames.size(); i++)
        {
            if(i > 0) s += ",";
            s += modelNames[i].first;
        }
         (*getByCode(SettingCode_Model))[SettingField_ValueList] = s;
    }
    ok &= scaleConfig->read();
    if(ok) fromConfig();
    Tools::debugLog(QString("@@@@@ JsonArrayFile::read %1 %2").arg(Tools::toString(ok), Tools::toString((int)(items.count()))));
    return ok;
}

void Settings::fromConfig()
{
    Tools::debugLog("@@@@@ Settings::fromConfig");
    setValue(SettingCode_Model,            scaleConfig->get(ScaleConfigField_Model).toString());
    setValue(SettingCode_SerialNumber,     scaleConfig->get(ScaleConfigField_SerialNumber).toString());
    setValue(SettingCode_VerificationDate, scaleConfig->get(ScaleConfigField_VerificationDate).toString());
    setValue(SettingCode_License,          scaleConfig->get(ScaleConfigField_License).toString());
    setModelValues();
}

void Settings::toConfig()
{
    Tools::debugLog("@@@@@ Settings::toConfig");
    const int m = model();
    setConfigValue(ScaleConfigField_Model,            Tools::toString(m));
    setConfigValue(ScaleConfigField_ModelName,        modelNames[m].first);
    setConfigValue(ScaleConfigField_SerialNumber,     getStringValue(SettingCode_SerialNumber));
    setConfigValue(ScaleConfigField_VerificationDate, getStringValue(SettingCode_VerificationDate));
    setConfigValue(ScaleConfigField_License,          getStringValue(SettingCode_License));
    setConfigValue(ScaleConfigField_NetName,          netName());
    setModelValues();
}

void Settings::setModelValues()
{
    Tools::debugLog("@@@@@ Settings::setModelValues");
    setValue(SettingCode_InfoModelName,    scaleConfig->get(ScaleConfigField_ModelName).toString());
    setValue(SettingCode_InfoSerialNumber, scaleConfig->get(ScaleConfigField_SerialNumber).toString());
    setValue(SettingCode_NetName,          scaleConfig->get(ScaleConfigField_NetName).toString());
    setValue(SettingCode_InfoNetName,      scaleConfig->get(ScaleConfigField_NetName).toString());
    setValue(SettingCode_InfoLicense,      scaleConfig->get(ScaleConfigField_License).toString());
}

QString Settings::modelInfo()
{
    QString s = "Модель ?";
    QString mn = getStringValue(SettingCode_Model);
    if(!mn.isEmpty()) s = QString("%1  № %2").arg(mn, getStringValue(SettingCode_SerialNumber));
    return s;
}

QJsonObject Settings::getScaleConfig()
{
    if(scaleConfig != nullptr) return scaleConfig->toJsonObject();
    return QJsonObject();
}

int Settings::model()
{
    int model = getIntValue(SettingCode_Model, true);
    if(model >= modelNames.count()) model = 0;
    return model;
}

QString Settings::netName()
{
    return modelNames[model()].second + "_" + getStringValue(SettingCode_SerialNumber);
}

QString Settings::aboutInfo()
{
    QString s;
    s += QString("Идентификатор весов: %1\n").    arg(getStringValue(SettingCode_InfoNetName));
    s += QString("Версия приложения: %1\n").      arg(getStringValue(SettingCode_InfoVersion));
    s += QString("Лицензия: %1\n").               arg(getStringValue(SettingCode_InfoLicense));
    s += QString("Версия модуля драйверов: %1\n").arg(getStringValue(SettingCode_InfoMODVersion));
    s += QString("Версия демона: %1\n").          arg(getStringValue(SettingCode_InfoDaemonVersion));
    s += QString("Версия БД: %1\n").              arg(getStringValue(SettingCode_InfoDBVersion));
    s += QString("Версия весового модуля: %1\n"). arg(getStringValue(SettingCode_InfoWMVersion));
    s += QString("Версия принтера: %1\n").        arg(getStringValue(SettingCode_InfoPMVersion));
    s += QString("Версия сервера: %1\n").         arg(getStringValue(SettingCode_InfoServerVersion));
    NetEntry ne = updateNetInfo();
    s += QString("IP: %1").arg(getStringValue(SettingCode_InfoIP));
    if(ne.isEthernet())  s += QString(" (Ethernet)");
    else if(ne.isWiFi()) s += QString(" (SSID %1)").arg(getStringValue(SettingCode_InfoWiFiSSID));
    s += QString("\n");
    s += QString("Последняя загрузка: %1").       arg(getStringValue(SettingCode_InfoLastDownload));
    return s;
}

void Settings::fillLabelList(const DBRecordList& labels)
{
    Tools::debugLog("@@@@@ Settings::fillLabelList");
    DBRecord& currentLabel = *getByCode(SettingCode_PrintLabelFormat);
    QStringList values;
    for (int i = 0; i < labels.count(); i++)
    {
        const QString value = QString("#%1  %2").arg(
                labels[i][ResourceDBTable::Code].toString(),
                labels[i][ResourceDBTable::Name].toString());
        if(!values.contains(value)) values.append(value);
    }
    currentLabel[SettingField_ValueList] = Tools::toString(values);
    int n = getIntValue(SettingCode_PrintLabelFormat, true);
    if(n < 0 || n >= values.count()) currentLabel[SettingField_Value] = "0";
}

NetEntry Settings::updateNetInfo()
{
    Tools::debugLog("@@@@@ Settings::updateNetInfo");
    QString ip = "0.0.0.0";
    QString ssid = "?";
    NetEntry ne = Tools::getNetEntry();
    if(ne.isIP())
    {
        ip = ne.ip;
        if(ne.isEthernet()) ssid = "Ethernet";
        else if(ne.isWiFi() && !ne.ssid.isEmpty()) ssid = ne.ssid;
    }
    setValue(SettingCode_InfoIP, ip);
    setValue(SettingCode_InfoWiFiSSID, ssid);
    return ne;
}


