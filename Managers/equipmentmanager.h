#ifndef EQUIPMENTMANAGER_H
#define EQUIPMENTMANAGER_H

#include <QVariantList>
#include "wm100.h"
#include "constants.h"
#include "externalmessager.h"

class AppManager;
class DataBase;
class Slpa100u;
class LabelCreator;

class EquipmentManager : public ExternalMessager
{
    Q_OBJECT

public:
    EquipmentManager(AppManager*);
    ~EquipmentManager() { stop(); }
    void create();
    void start();
    void stop();
    void setSystemDateTime(const bool v) { isSystemDateTime = v; }
    void pause(const bool);

    // Weight Manager:
    QString WMversion() const;
    void setWMParam(const int);
    double getWeight() const { return WMStatus.weight; }
    double getTare() const { return WMStatus.tare; }
    bool isWMError() const { return WMErrorCode != 0 || isWMStateError(WMStatus); }
    bool isWeightFixed() const { return isWMFlag(WMStatus, 0); }
    bool isZeroFlag() const { return isWMFlag(WMStatus, 1); }
    bool isTareFlag() const { return isWMFlag(WMStatus, 3); }
    bool isWMDemoMode() const { return WMMode == EquipmentMode_Demo; }
    QString getWMErrorDescription(const int) const;
    bool isWM();
    QString getWMDescription();

    // Print Manager:
    QString PMversion() const;
    int print(DataBase*, const DBRecord&, const DBRecord&, const QString&, const QString&, const QString&);
    bool isPMError() const { return PMErrorCode != 0 || isPMStateError(PMStatus); }
    bool isPMDemoMode() const { return PMMode == EquipmentMode_Demo; }
    QString getPMErrorDescription(const int) const;
    bool isPM();
    void feed();

private:
    QString makeBarcode(const DBRecord&, const QString&, const QString&, const QString&);
    QString parseBarcode(const QString&, const QChar, const QString&);

    // Weight Manager:
    void createWM();
    void removeWM();
    int startWM();
    bool isWMFlag(Wm100Protocol::channel_status, int) const;
    bool isWMStateError(Wm100Protocol::channel_status) const;

    // Print Manager:
    void createPM();
    void removePM();
    int startPM();
    bool isPMFlag(uint16_t v, int shift) const { return (v & (0x00000001 << shift)) != 0; }
    bool isPMStateError(uint16_t) const;

    // Weight Manager:
    Wm100* wm = nullptr;
    bool isWMStarted = false;
    int WMErrorCode = 0;
    Wm100Protocol::channel_status WMStatus = {0, 0.0, 0.0, 0};
    QString WMUri;
    EquipmentMode WMMode = EquipmentMode_None;

    // Print Manager:
    Slpa100u* slpa = nullptr;
    LabelCreator* labelCreator = nullptr;
    bool isPMStarted = false;
    int PMErrorCode = 0;
    uint16_t PMStatus = 0;
    QString PMUri;
    EquipmentMode PMMode = EquipmentMode_None;

    bool isSystemDateTime = false;

signals:
    void printed(const DBRecord&);
    void paramChanged(const int, const int);

public slots:
    void onWMStatusChanged(Wm100Protocol::channel_status&);
    void onWMErrorStatusChanged(int);
    void onPMStatusChanged(uint16_t);
    void onPMErrorStatusChanged(int);
};

#endif // EQUIPMENTMANAGER_H
