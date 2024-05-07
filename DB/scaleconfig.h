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
    ScaleConfigField_ModelName = 1, // Comment
    ScaleConfigField_SerialNumber = 2,
    ScaleConfigField_VerificationDate = 3,
    ScaleConfigField_FactorySettingsPassword = 4,
    ScaleConfigField_SystemSettingsPassword = 5,
};

class ScaleConfig : public JsonFile
{
public:
    ScaleConfig(AppManager*);
    bool read();
    void clear() { data.clear(); }
    QVariant get(ScaleConfigField field) { return data[field]; }
    void set(ScaleConfigField field, const QVariant& v) { data[field] = v; }

protected:
    QJsonObject toJson();
    DBRecord parse(const QString&);

    DBRecord data;
};

#endif // SCALECONFIG_H
