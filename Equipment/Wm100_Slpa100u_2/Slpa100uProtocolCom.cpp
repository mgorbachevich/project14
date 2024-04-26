#include <QThread>
#include "Slpa100uProtocolCom.h"

Slpa100uProtocolCom::Slpa100uProtocolCom(QObject *parent)
    : Slpa100uProtocol{parent}
{
    deviceInterface = diCom;
}

bool Slpa100uProtocolCom::checkUri(const QString &uri)
{
    static QString RegexStr("^serial:\\/\\/([\\w\\-\\/\\.]+)(\\?(\\w+=\\w+)(\\&(\\w+=\\w+))*)?$");
    return uri.contains(QRegularExpression(RegexStr,QRegularExpression::CaseInsensitiveOption));
}

char Slpa100uProtocolCom::crc(const QByteArray &buf)
{
    char res = 0;
    foreach (const char c, buf) res ^= c;
    return res;
}

int Slpa100uProtocolCom::executeCommand(prncommand cmd, const QByteArray &out, QByteArray &in)
{
    static const QList<prncommand> withAnswer    = {cmFeed, cmPrint, cmGetStatus, cmGetOffset2, cmGetVersion, cmGetPE, cmGetLAB, cmMemoryTest, cmPrintTest, cmErrorCnt, cmResetCnt, cmProtectionCnt};
    static const QList<prncommand> withoutAnswer = {cmBufferSize, cmBufferData, cmClearBuffer, cmSetBrightness, cmSetOffset, cmSetPaperType, cmSensorControl, cmSetOffset2, cmReset, cmProtectionMode};

    bool isWithAnswer = withAnswer.contains(cmd);
    bool isWithoutAnswer = withoutAnswer.contains(cmd);
    if (!isWithAnswer && !isWithoutAnswer) return -15;
    if (!io) return -20;

    int res = 0;
    QByteArray buf, buf1;

    for (int i = 0; i < 3; i++)
    {
        io->clear();
        buf.clear();
        buf1.clear();
        buf += ENQ;
        if (!io->write(buf) || !io->read(buf1,1) || buf1[0] != SYNC) res = -1;

        buf.clear();
        buf += cmd;
        buf += out;
        buf += crc(buf);
        buf.push_front(out.size()+1);
        buf.push_front(STX);

        if (!res)
            if (!io->write(buf)) res = -4;

        buf.clear();
        // ACK
        if (!res)
            if (!io->read(buf,1) || buf[0] != ACK) res = -5;

        if (!res && isWithAnswer) {
            if (io->read(in, 11)) {
                if (cmd != in[0]) res = -16;
                else res = in[1];
                in.remove(0,2);
            }
            else res = -5;
            if (res < 0) in.clear();
        }
        if (!res) break;
        QThread::msleep(50);
    }
    return res;
 }
