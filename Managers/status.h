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
    void onStopAll() { isAlarm = false; quantity = price = amount = NO_DATA; }

    bool isNet = false;
    bool isSettings = false;
    bool isManualPrintEnabled = false;
    bool isPrintCalculateMode = false;
    bool isRefreshNeeded = false;
    bool isWaiting = false;
    bool isAlarm = false;
    bool isProductSortIncrement = true;
    int pieces = 1;
    int productSort = ShowcaseSort_Name;
    int lastProductSort = ShowcaseSort_Code; // ShowcaseSort_Code/ShowcaseSort_Code2
    int secret = 0;
    int downloadedRecords = 0;
    QString downloadDateTime;
    quint64 netActionTime = 0;
    quint64 userActionTime = 0;
    AutoPrintMode autoPrintMode = AutoPrintMode_Off;
    QString quantity = NO_DATA;
    QString price = NO_DATA;
    QString amount = NO_DATA;
};

#endif // STATUS_H
