#ifndef STATUS_H
#define STATUS_H

#include "constants.h"
#include "tools.h"

class Status
{
public:
    Status() {}
    void onResetProduct() { pieces = 1; }
    void onUserAction() { userActionTime = Tools::nowMsec(); secret = 0; }
    void onStopEquipment() { isAlarm = false; labelPath = "";  clearValues(); }
    void clearValues() { quantity = price = price2 = basePrice = basePrice2 = amount = amount2 = tare = NO_DATA; }

    bool isSettingsOpened = false;
    bool isRefreshNeeded = false;
    int pieces = 1;
    QString quantity = NO_DATA;
    QString price = NO_DATA;
    QString price2 = NO_DATA;
    QString basePrice = NO_DATA; // всегда за кг
    QString basePrice2 = NO_DATA; // всегда за кг
    QString amount = NO_DATA;
    QString amount2 = NO_DATA;
    QString tare = NO_DATA;

    // User's input:
    quint64 userActionTime = 0;
    bool isWaiting = false;
    bool isAlarm = false;
    int secret = 0;

    // Net:
    bool isNet = false;
    int downloadedRecordCount = 0;
    QString downloadDateTime;
    quint64 netActionTime = 0;

    // Print:
    bool isManualPrintEnabled = false;
    bool isPrintCalculateMode = false;
    AutoPrintMode autoPrintMode = AutoPrintMode_Off;
    QString labelPath;

    // Showcase:
    bool isShowcaseSortIncrement = true;
    int showcaseSort = ShowcaseSort_Name;
    int showcaseLastSort = ShowcaseSort_Code; // ShowcaseSort_Code/ShowcaseSort_Code2
};

#endif // STATUS_H
