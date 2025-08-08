#ifndef SCALECONFIG_H
#define SCALECONFIG_H

#include <QObject>
#include <QVariantList>
#include "constants.h"
#include "jsonfile.h"

enum ScaleConfigField
{
    ScaleConfigField_Model = 0,
    ScaleConfigField_ModelName, // Comment
    ScaleConfigField_SerialNumber,
    ScaleConfigField_NetName,
    ScaleConfigField_VerificationDate,
    ScaleConfigField_FactorySettingsPassword,
    ScaleConfigField_SystemSettingsPassword,
    ScaleConfigField_License,
};

class ScaleConfig : public JsonFile
{
public:
    explicit ScaleConfig(QObject*);
    bool read();
    void clear() { data.clear(); }
    QVariant get(ScaleConfigField field) { return data[field]; }
    void set(ScaleConfigField field, const QString& v) { data[field] = v; }
    QJsonObject toJsonObject();

protected:
    DBRecord parse(const QString&);

    DBRecord data;
};

#endif // SCALECONFIG_H
