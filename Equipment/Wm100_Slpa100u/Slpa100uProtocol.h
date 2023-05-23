#ifndef SLPA100UPROTOCOL_H
#define SLPA100UPROTOCOL_H

#include <QObject>
#include "IO/iobase.h"

class Slpa100uProtocol : public QObject
{
    Q_OBJECT
public:
    explicit Slpa100uProtocol(QObject *parent = nullptr);

public:
    enum papertype {
        ptSticker = 0,
        ptRibbon = 1
    };
    enum prncommand {
        cmFeed            = 0x40,
        cmBufferSize      = 0x41,
        cmBufferData      = 0x42,
        cmClearBuffer     = 0x43,
        cmPrint	          = 0x44,
        cmGetStatus	      = 0x45,
        cmSetBrightness   = 0x46,
        cmSetOffset	      = 0x47,
        cmSetPaperType    = 0x48,
        cmSensorControl   = 0x49,
        cmReset	          = 0x4a,
        cmSetOffset2      = 0x4b,
        cmGetOffset2      = 0x4c,
        cmGetVersion      = 0x4f,
        cmGetPE           = 0x51,
        cmGetLAB          = 0x52,
        cmMemoryTest      = 0x60,
        cmPrintTest       = 0x65,
        cmErrorCnt        = 0x70,
        cmResetCnt        = 0x71,
        cmProtectionCnt   = 0x72,
        cmProtectionMode  = 0x80
    };
    struct prnanswer {
        uint16_t printerStatus;
        uint8_t versionH;
        uint8_t versionL;
        uint8_t PE;
        uint8_t LAB;
        uint8_t TM;
        uint8_t RUL;
        uint8_t brightness;
    };
    struct memorytest{
        uint32_t address;
        uint8_t readByte;
        uint8_t expectedByte;
        uint8_t reserved1;
        uint8_t reserved2;
        uint8_t reserved3;
    };
    struct counters{
        uint16_t c1;
        uint16_t c2;
        uint16_t c3;
        uint16_t c4;
        uint8_t reserved1;
    };

public:
    virtual int open(const QString &uri);
    virtual void close();
    int cFeed(prnanswer *status);
    int cBufferSize(const uint16_t value);
    int cBufferData(const QByteArray &line);
    int cClearBuffer();
    int cPrint(prnanswer *status);
    int cGetStatus(prnanswer *status);
    int cSetBrightness(const uint8_t value);
    int cSetOffset(const uint8_t value);
    int cSetPaperType(const papertype value);
    int cSensorControl(const bool value);
    int cReset();
    int cSetOffset2(const uint8_t value);
    int cGetOffset2(uint8_t *value);
    int cGetVersion(uint8_t *versionH, uint8_t *versionL);
    int cGetPE(uint8_t *value);
    int cGetLAB(uint8_t *value);
    int cMemoryTest(memorytest *test);
    int cPrintTest(const uint16_t lines, prnanswer *status);
    int cErrorCnt(counters *cnt);
    int cResetCnt(counters *cnt);
    int cProtectionCnt(counters *cnt);
    int cProtectionMode(const bool value);

    int getPrinterVersion();

protected:
    IoBase *io = nullptr;
    QByteArray statusToByteArray(const prnanswer &st);

private:
    uint16_t labelLength = 60*8;
    uint16_t testLength = 0;
    qint64 delayTill = 0;
    prnanswer lastStatus = {0, 0, 0, 0, 0, 0, 0, 0};
    virtual int executeCommand(prncommand cmd, const QByteArray &out, QByteArray &in) = 0;
    int command(prncommand cmd, const QByteArray &out, QByteArray &in);
    int command(prncommand cmd, const QByteArray &out, prnanswer *ans);
    int command(prncommand cmd, const QByteArray &out);
    int command(prncommand cmd, prnanswer *ans = nullptr);
    int command(prncommand cmd, memorytest *ans);
    int command(prncommand cmd, counters *ans);
    void setDelay(prncommand cmd);
signals:
    void showMessage(const QString&);

};

#endif // SLPA100UPROTOCOL_H
