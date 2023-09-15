#ifndef PRINTSTATUS_H
#define PRINTSTATUS_H

class PrintStatus
{
public:
    PrintStatus() {}
    void onNewProduct() {}
    void onResetProduct() { pieces = 1; }

    bool manualPrintEnabled = false;
    bool calculateMode = false;
    int pieces = 1;
};

#endif // PRINTSTATUS_H
