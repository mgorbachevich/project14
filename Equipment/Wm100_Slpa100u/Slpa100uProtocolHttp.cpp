#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
//#include <thread>
#include "Slpa100uProtocolHttp.h"

Slpa100uProtocolHttp::Slpa100uProtocolHttp(QObject *parent)
    : Slpa100uProtocol{parent}
{
    deviceInterface = diHttp;
}

bool Slpa100uProtocolHttp::checkUri(const QString &uri)
{
    static QString RegexStr("^http:\\/\\/([\\w\\-\\/\\.]+)(\\:(\\d+))$");
    return uri.contains(QRegularExpression(RegexStr,QRegularExpression::CaseInsensitiveOption));
}

int Slpa100uProtocolHttp::executeCommand(prncommand cmd, const QByteArray &out, QByteArray &in)
{
    static const QList<prncommand> withAnswer    = {cmFeed, cmPrint, cmGetStatus, cmGetOffset2, cmGetVersion, cmGetPE, cmGetLAB, cmMemoryTest, cmPrintTest, cmErrorCnt, cmResetCnt, cmProtectionCnt};
    static const QList<prncommand> withoutAnswer = {cmBufferSize, cmBufferData, cmClearBuffer, cmSetBrightness, cmSetOffset, cmSetPaperType, cmSensorControl, cmSetOffset2, cmReset, cmProtectionMode};
    bool isWithAnswer = withAnswer.contains(cmd);
    bool isWithoutAnswer = withoutAnswer.contains(cmd);
    if (!isWithAnswer && !isWithoutAnswer) return -15;
    if (!io) return -20;

    QByteArray answer;
    int res = 0;
    if (cmd == Slpa100uProtocol::cmPrint) res = printBuffer(in);
    else if (cmd == Slpa100uProtocol::cmBufferData)
    {
        buffer += out;
        if (out.size() < 56) buffer += QByteArray(56-out.size(), 0x00);
    }
    else if (cmd == Slpa100uProtocol::cmBufferSize) {
        QDataStream ds(out);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> linesToPrint;
    }
    else if (cmd == Slpa100uProtocol::cmClearBuffer) buffer.clear();
    else
    {
        res = sendCommand(cmd, out, answer, (isWithAnswer)?16:0);
        if (isWithAnswer && !res && answer.toStdString()=="ok")
        {
            res = readAnswer(cmd, out, in);
            if (!res && in.size() == 16)
            {
                if (static_cast<prncommand>(in[2]) == 0) res = -17;
                else if (cmd != static_cast<prncommand>(in[2])) res = -16;
                else res = static_cast<uchar>(in[3]);
                in = in.mid(4,9);
                if (res == -16) qDebug() << "executeCommand() получили ответ от команды =" << static_cast<uchar>(in[2]) << " ожидали = " << static_cast<uchar>(cmd);
            }
            else if (in.size() == 0)  res = -7;
            else if (in.size() != 16) res = -8;
        }
        else if (answer.toStdString() != "ok") res = -4;
    }
    return res;
}

int Slpa100uProtocolHttp::readAnswer(prncommand cmd, const QByteArray &out, QByteArray &in)
{
    int res = -1;
    QByteArray answer;
    io->setOption(0, 0); // GET
    io->setOption(1, 0, "/api/v0/spi/rxbuf");
    io->setOption(2, 0);
    io->setOption(2, 1, "format", "hextext");
    int nn = 0; bool b;
    do { b = io->writeRead(out, answer, 0, 3000); }
    while (answer == "error: spi device busy" && ++nn < 10);
    if (answer == "error: spi device busy" && nn == 10) b = -1;
    if (b)
    {
        parseReply(answer, in);
        res = 0;
        if (in.size() > 5 && getCRC16(in.mid(2, in.size()-3))) res = -13;
        qDebug() << ">> " << in.toHex(' ');
        if (in.size() == 0) qDebug() << answer;
    }
    return res;
}

int Slpa100uProtocolHttp::sendCommand(prncommand cmd, const QByteArray &out, QByteArray &in, uint32_t toRead)
{
    int res = 0;
    io->setOption(0, 1);
    io->setOption(1, 0, "/api/v0/spi/cmd16/send");
    io->setOption(2, 0);
    io->setOption(2, 1, "format", "hextext");
    QByteArray cmd16data;
    cmd16data += cmd;
    cmd16data += out;
    QString cmd16(cmd16data.toHex());
    io->setOption(2, 1, "cmd16data", cmd16.toUpper());
    io->setOption(2, 1, "rxlength", QString("%1").arg(toRead));
    int nn = 0; bool b;
    do { b = io->writeRead(out, in, 0, 3000); }
    while (in != "ok" && ++nn < 30);
    if (!b) res = -1;
    if (nn == 30) res = -30;
    qDebug() << ">> " << cmd16data.toHex(' ') << " -> " << in;
    return res;
}

int Slpa100uProtocolHttp::printBuffer(QByteArray &in)
{
    int res = 0;
    if (buffer.size() / 56 != linesToPrint) return -14;
    res = sendImage(buffer, in);
    return res;
}

int Slpa100uProtocolHttp::sendImage(const QByteArray &out, QByteArray &in)
{
    int res = 0;
    QByteArray answer;
    io->setOption(0, 1);
    io->setOption(1, 0, "/api/v0/spi/cmd16/sendimage");
    io->setOption(2, 0);
    io->setOption(2, 1, "format", "hextext");
    QString imagedata(out.toHex());
    io->setOption(2, 1, "imagedata", imagedata.toUpper());
    io->setOption(2, 1, "doprint", "true");
    bool b = io->writeRead(out, answer, 0, 3000);
    if (!b) res = -1;
    else if (answer == "error: spi device busy") res = -17;
    else
    {
        parseReply(answer, in);
        //res = static_cast<uchar>(in[3]);
        //in[3] = 0;
        if (in.size() > 5 && getCRC16(in.mid(2, in.size()-3))) res = -13;
        else res = static_cast<uchar>(in[3]);
        qDebug() << "<< " << in.toHex(' ');
        qDebug() << "<< " << answer;
        in = in.mid(4,9);
    }
    return res;
}

int Slpa100uProtocolHttp::flushBuffer(const QByteArray &out, uint16_t offset, uint16_t flags)
{
    int res = 0;
    prncommand cmd = prncommand::cmSendDataBlock;
    io->setOption(0, 1);
    io->setOption(1, 0, "/api/v0/spi/cmd16/send");
    io->setOption(2, 0);
    io->setOption(2, 1, "format", "hextext");
    QByteArray cmd16data, answer;
    uint16_t lines = (!flags) ? 0 : linesToPrint;
    cmd16data += cmd;
    cmd16data += offset;
    cmd16data += offset >> 8;
    //uint16_t sz = out.size();
    //cmd16data += sz;
    //cmd16data += sz >> 8;
    cmd16data += QByteArray(2, 0x00);
    cmd16data += flags;
    cmd16data += flags >> 8;
    //uint16_t crcm = getCRC16(out);
    //cmd16data += crcm;
    //cmd16data += crcm >> 8;
    cmd16data += QByteArray(2, 0x00);
    cmd16data += lines;
    cmd16data += lines >> 8;
    cmd16data += QByteArray(1, 0x00);
    QString cmd16(cmd16data.toHex());
    io->setOption(2, 1, "cmd16data", cmd16.toUpper());
    io->setOption(2, 1, "addtxbuflentopos", "3");
    io->setOption(2, 1, "addtxbufcrc16topos", "7");
    QString txdata(out.toHex());
    io->setOption(2, 1, "txdata", txdata.toUpper());
    if (!io->writeRead(out, answer, 0, 3000)) res = -1;
    qDebug() << "flushBuffer() writeRead() answer = " << answer << ", res = " << res << ", size = " << out.size() << ", offset = " << offset << ", cmd16 = " << cmd16;

    if (!res && answer.toStdString()=="ok" && !lines)
    {
        QByteArray send;
        int nn = 0;
        do
        {
            //std::this_thread::sleep_for(std::chrono::milliseconds(150));
            res = sendCommand(prncommand::cmLastSendDataRes, send, answer, 12);
        } while (answer == "error: spi device busy" && ++nn < 50);
        if (!res && answer.toStdString()=="ok")
        {
            //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            res = readAnswer(cmd, out, answer);
            if (!res && answer.size() == 12)
            {
                res = 0;
            }
            qDebug() << "flushBuffer() answer = " << answer.toHex(' ') << ", res = " << res << ", size = " << out.size() << ", offset = " << offset;
        }
        else qDebug() << "flushBuffer() answer = " << answer;
    }
    else qDebug() << "flushBuffer() answer = " << answer;
    return res;
}

uint16_t Slpa100uProtocolHttp::getCRC16(const QByteArray &src)
{
    // CRC16_MODBUS
    static const uint16_t table[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };
    static const uint16_t  CRCINIT = 0xFFFF;
    uint16_t crc = CRCINIT;
    uint8_t byte;
    QDataStream ds(src);
    while (!ds.atEnd())
    {
        ds >> byte;
        byte ^= crc;
        crc >>= 8;
        crc ^= table[byte];
    }
    return crc;
}

void Slpa100uProtocolHttp::parseReply(const QByteArray &reply, QByteArray &answer)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply);
    QJsonObject jsonObject = jsonResponse.object();
    QJsonValue jsonValue = jsonObject["rxbuf"].toObject()["data"];
    QString data = jsonValue.toString().remove(' ');
    answer = QByteArray::fromHex(data.toLatin1());
 }
