#ifndef SCALECONFIG_H
#define SCALECONFIG_H

#include <QObject>
#include <QVariantList>
#include "constants.h"
#include "jsonfile.h"

class AppManager;

enum ScaleConfigField
{
    ScaleConfigField_Model = 0,
    ScaleConfigField_ModelName, // Comment
    ScaleConfigField_SerialNumber,
    ScaleConfigField_VerificationDate,
    ScaleConfigField_FactorySettingsPassword,
    ScaleConfigField_SystemSettingsPassword,
    ScaleConfigField_LastDownloadDateTime,
    ScaleConfigField_LastUploadDateTime,
    ScaleConfigField_LastDeleteDateTime,
    ScaleConfigField_LastDownloadSucceded,
    ScaleConfigField_LastDownloadTotal,
};

class ScaleConfig : public JsonFile
{
public:
    ScaleConfig(AppManager*);
    bool read();
    void clear() { data.clear(); }
    QVariant get(ScaleConfigField field) { return data[field]; }
    void set(ScaleConfigField field, const QString& v) { data[field] = v; }

protected:
    QJsonObject toJson();
    DBRecord parse(const QString&);

    DBRecord data;
};

#endif // SCALECONFIG_H
