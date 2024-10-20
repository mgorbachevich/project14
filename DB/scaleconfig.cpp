#include <QJsonDocument>
#include "tools.h"
#include "scaleconfig.h"

ScaleConfig::ScaleConfig(AppManager* parent) : JsonFile(SCALE_CONFIG_FILE, parent)
{
    Tools::debugLog("@@@@@ ScaleConfig::ScaleConfig ");
    fields.insert(ScaleConfigField_Model,                   "model");
    fields.insert(ScaleConfigField_ModelName,               "model_name");
    fields.insert(ScaleConfigField_SerialNumber,            "serial_number");
    fields.insert(ScaleConfigField_NetName,                 "net_name");
    fields.insert(ScaleConfigField_VerificationDate,        "verification_date");
    fields.insert(ScaleConfigField_FactorySettingsPassword, "factory_settings_password");
    fields.insert(ScaleConfigField_SystemSettingsPassword,  "system_settings_password");
    fields.insert(ScaleConfigField_License,                 "license");
}

bool ScaleConfig::read()
{
    data = parse(Tools::readTextFile(fileName));
    Tools::debugLog("@@@@@ ScaleConfig::read " + Tools::toString((int)(data.count())));
    if(data.count() <= 0) for(int i = 0; i < fields.count(); i++) data << "";

#ifdef DEFAULT_FACTORY_SETTINGS_PASSWORDS
    set(ScaleConfigField_FactorySettingsPassword, "0000");
    set(ScaleConfigField_SystemSettingsPassword, "1234");
#endif

    return data.count() > 0;
}

QJsonObject ScaleConfig::toJsonObject()
{
    QJsonObject jo;
    for(int i = 0; i < data.count() && i < fields.count(); i++)
        jo.insert(fields.value(i), data.at(i).toJsonValue());
    return jo;
}

DBRecord ScaleConfig::parse(const QString& json)
{
    const QJsonObject jo = Tools::toJsonObject(json);
    DBRecord r;
    for (int i = 0; i < fields.count(); i++) r << jo[fields.value(i)].toString("");
    return r;
}

