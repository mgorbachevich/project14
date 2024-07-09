#ifndef PRINTSTATUS_H
#define PRINTSTATUS_H

#include "constants.h"

class PrintStatus
{
public:
    PrintStatus() {}
    void onNewProduct() {}
    void onResetProduct() { pieces = 1; }

    bool manualPrintEnabled = false;
    bool calculateMode = false;
    int pieces = 1;
    AutoProntMode autoPrintMode = AutoProntMode_Off;
};

#endif // PRINTSTATUS_H
