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
    ~EquipmentManager() { removeWM(); removePM(); }
    void create();
    void start();
    void stop();
    void setSystemDateTime(const QDateTime&);
    void pause(const bool v) { pauseWM(v); pausePM(v); }
    QString daemonVersion() const;
    QString MODVersion() const { return Wm100::getVersionString(); }

    // Weight Manager:
    QString WMVersion() const;
    void setWMParam(const int);
    double getWeight() const { return WMStatus.weight; }
    double getTare() const { return WMStatus.tare; }
    bool isWMError() const { return WMErrorCode != 0 || isWMStateError(WMStatus); }
    bool isWMOverloaded() const { return isWMFlag(WMStatus, 6); }
    bool isWeightFixed() const { return isWMFlag(WMStatus, 0); }
    bool isZeroFlag() const { return isWMFlag(WMStatus, 1); }
    bool isTareFlag() const { return isWMFlag(WMStatus, 3); }
    EquipmentMode getWMMode() const { return WMMode; }
    QString getWMErrorDescription(const int) const;
    bool isWM();
    QString getWMDescription();
    int setExternalDisplay(const DBRecord&);

    // Print Manager:
    QString PMVersion() const;
    int print(DataBase*, const DBRecord&, const DBRecord&, const QString&);
    bool isPMError() const { return PMErrorCode != 0 || isPMStateError(PMStatus); }
    QString getPMErrorDescription(const int) const;
    EquipmentMode getPMMode() const { return PMMode; }
    bool isPM();
    void feed();

private:
    QString makeBarcode(const DBRecord&, const QString&, const QString&);
    QString parseBarcode(const QString&, const QChar, const QString&);
    QString getWMDescriptionNow();

    // Weight Manager:
    void createWM();
    void removeWM();
    void stopWM();
    int startWM();
    void pauseWM(const bool);
    bool isWMFlag(Wm100Protocol::channel_status, int) const;
    bool isWMStateError(Wm100Protocol::channel_status) const;

    // Print Manager:
    void createPM();
    void removePM();
    int startPM();
    void stopPM();
    void pausePM(const bool);
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
    QString labelPath;

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
