#ifndef SLPA100UPROTOCOL_H
#define SLPA100UPROTOCOL_H

#include <QObject>
#include <QRegularExpression>
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
        cmProtectionMode  = 0x80,
        cmSendDataBlock   = 0x90,
        cmLastSendDataRes = 0x91
    };
    enum deviceinterface {
        diNone,
        diCom,
        diHttp,
        diUsb,
        diDemo
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
    virtual int cFeed(prnanswer *status);
    virtual int cBufferSize(const uint16_t value);
    virtual int cBufferData(const QByteArray &line);
    virtual int cClearBuffer();
    virtual int cPrint(prnanswer *status);
    virtual int cGetStatus(prnanswer *status, const int isPE = 0);
    virtual int cSetBrightness(const uint8_t value);
    virtual int cSetOffset(const uint8_t value);
    virtual int cSetPaperType(const papertype value);
    virtual int cSensorControl(const bool value);
    virtual int cReset();
    virtual int cSetOffset2(const uint8_t value);
    virtual int cGetOffset2(uint8_t *value);
    virtual int cGetVersion(uint8_t *versionH, uint8_t *versionL);
    virtual int cGetPE(uint8_t *value);
    virtual int cGetLAB(uint8_t *value);
    virtual int cMemoryTest(memorytest *test);
    virtual int cPrintTest(const uint16_t lines, prnanswer *status);
    virtual int cErrorCnt(counters *cnt);
    virtual int cResetCnt(counters *cnt);
    virtual int cProtectionCnt(counters *cnt);
    virtual int cProtectionMode(const bool value);

    virtual int getPrinterVersion();
    deviceinterface getInterface();

protected:
    IoBase *io = nullptr;
    QByteArray statusToByteArray(const prnanswer &st);
    deviceinterface deviceInterface = diNone;

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
    void setDelay(prncommand cmd, const int res);
signals:
    void showMessage(const QString&);

};

#endif // SLPA100UPROTOCOL_H
