#include <QRegularExpression>
#include "Wm100ProtocolCom.h"
#include "IO/iocom.h"

Wm100ProtocolCom::Wm100ProtocolCom(QObject *parent)
    : Wm100Protocol{parent}
{
    deviceInterface = diCom;
}

int Wm100ProtocolCom::open(const QString &uri)
{
    close();
    io = new IoCom(this);
    if (!io->open(uri)) return io->error;
    return 0;
}

bool Wm100ProtocolCom::checkUri(const QString &uri)
{
    static QString RegexStr("^serial:\\/\\/([\\w\\-\\/\\.]+)(\\?(\\w+=\\w+)(\\&(\\w+=\\w+))*)?$");
    return uri.contains(QRegularExpression(RegexStr,QRegularExpression::CaseInsensitiveOption));
}

char Wm100ProtocolCom::crc(const QByteArray &buf)
{
    char res = 0;
    foreach (const char c, buf) res ^= c;
    return res;
}

int Wm100ProtocolCom::executeCommand(wmcommand cmd, const QByteArray &out, QByteArray &in)
{
    if (!io) return -20;

    int res = 0;

    io->clear();
    QByteArray buf;

    buf += out.size()+1;
    buf += cmd;
    buf += out;
    buf += crc(buf);
    buf.push_front(STX);

    if (!io->write(buf)) res = -4;

    buf.clear();
    // ACK
    if (!res)
        if (!io->read(buf,1) || buf[0] != ACK) res = -5;
    // STX
    if (!res)
        if (!io->read(buf,1) || buf[1] != STX) res = -5;
    // length
    if (!res)
        if (!io->read(buf,1) || buf[2] < 2) res = -5;
    // other
    if (!res)
        if (!io->read(buf,buf[2]) || buf[3] != (char)cmd) res = -5;
    // crc
    if (!res)
    {
        buf.remove(0,2);
        char c = crc(buf);
        if (!io->read(buf,1) || buf[buf.size()-1] != c) res = -5;
        else
        {
            res = static_cast<quint8>(buf[2]);
            in = buf.mid(3,buf.size()-4);
        }
    }
    io->clear();
    return res;
}

int Wm100ProtocolCom::cSetDateTime(const QDateTime &datetime, const QString &uri)
{
    int res = -15;
    return res;
}
