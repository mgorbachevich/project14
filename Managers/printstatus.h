#ifndef PRINTSTATUS_H
#define PRINTSTATUS_H

class PrintStatus
{
public:
    PrintStatus() { clear(); }
    void clear() { manualPrintEnabled = false; calculateMode = true; pieces = 1; }

    bool manualPrintEnabled = false;
    bool calculateMode = true;
    int pieces = 1;
};

#endif // PRINTSTATUS_H
