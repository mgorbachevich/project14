#include <QRegularExpression>
#include "Slpa100uProtocolUsb.h"

Slpa100uProtocolUsb::Slpa100uProtocolUsb(QObject *parent)
    : Slpa100uProtocol{parent}
{

}

bool Slpa100uProtocolUsb::checkUri(const QString &uri)
{
    static QString RegexStr("^usb:\\/\\/([\\w\\-\\/\\.]+)(\\?(\\w+=\\w+)(\\&(\\w+=\\w+))*)?$");
    return uri.contains(QRegularExpression(RegexStr,QRegularExpression::CaseInsensitiveOption));
}

int Slpa100uProtocolUsb::executeCommand(prncommand cmd, const QByteArray &out, QByteArray &in)
{
    static const QList<prncommand> withAnswer    = {cmFeed, cmPrint, cmGetStatus, cmGetOffset2, cmGetVersion, cmGetPE, cmGetLAB, cmMemoryTest, cmPrintTest, cmErrorCnt, cmResetCnt, cmProtectionCnt};
    static const QList<prncommand> withoutAnswer = {cmBufferSize, cmBufferData, cmClearBuffer, cmSetBrightness, cmSetOffset, cmSetPaperType, cmSensorControl, cmSetOffset2, cmReset, cmProtectionMode};

    bool isWithAnswer = withAnswer.contains(cmd);
    bool isWithoutAnswer = withoutAnswer.contains(cmd);
    if (!isWithAnswer && !isWithoutAnswer) return -15;
    if (!io) return -20;
    QByteArray buf;
    int res = 0;

    io->clear();
    buf += cmd;
    buf += out;
    buf += QByteArray(64-buf.size(),0);
    if (!io->write(buf)) res = -4;

    in.clear();
    // length
    if (!res && isWithAnswer) {
        if (io->read(in, 11)) {
            if (cmd != in[0]) res = -16;
            else res = in[1];
            in.remove(0,2);
        }
        else res = -5;
        if (res < 0) in.clear();
    }
    return res;
}
