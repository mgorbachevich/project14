#include <QThread>
#include <QRandomGenerator>
#include <QRegularExpression>
#include "qcoreevent.h"
#include "wm100.h"
#include "IO/iocom.h"

QByteArray &operator<<(QByteArray &l, quint8 r)
{
    l.append(r);
    return l;
}

QByteArray &operator<<(QByteArray &l, quint16 r)
{
    return l<<quint8(r)<<quint8(r>>8);
}

QByteArray &operator<<(QByteArray &l, qint32 r)
{
    return l<<quint16(r)<<quint16(r>>16);
}

bool operator==(channel_status &r1, channel_status &r2)
{
    return r1.flags==r2.flags && r1.state==r2.state && r1.weight==r2.weight && r1.tare==r2.tare;
}

bool operator!=(channel_status &r1, channel_status &r2)
{
    return !(r1==r2);
}

Wm100::Wm100(QObject *parent)
    : QObject{parent}
{
    disconnectDevice();
}

int Wm100::connectDevice(const QString &uri)
{
    stopDemo();
    if (io != nullptr) disconnectDevice();
    if (IoCom::checkUri(uri)) io = new IoCom(this);
    else if (checkForDemo(uri)) startDemo();
    else return -11;
    int res = 0;
    QByteArray buf;
    if  (io) {
        if (!io->open(uri)) res = io->error;
        io->clear();
        buf += ENQ;
        if (!res) if (!io->write(buf) || !io->read(buf,1) || buf[1]!=NAK) res = -1;
    }
    if (!res) res = getDeviceMetrics();
    if (!res) res = getChannelParam();

    if (res) disconnectDevice();
    return res;
}

int Wm100::disconnectDevice()
{
    stopDemo();
    stopPolling();
    lastStatusError = 0;
    lastStatus = {0, 0.0, 0.0, 0};
    if (io != nullptr) delete io;
    io = nullptr;
    return 0;
}

int Wm100::getStatus(channel_status *status)
{
    if (demo.active)
    { *status = demo.status;
        return 0; }
    QByteArray out("0030"), in;
    int res = command(cmGetStatus, out, in);
    if (!res)
    {
        int32_t  weight;
        uint16_t tare;
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> status->state >> weight >> tare >> status->flags;
        status->weight = weight * qPow(10, channelParams.measure);
        status->tare   = tare   * qPow(10, channelParams.measure);
    }
    return res;
}

int Wm100::setMode(uint8_t mode)
{
    if (demo.active) return mode?180:0;
    QByteArray out("0030"), in;
    out += mode;
    return command(cmSetMode, out, in);
}

int Wm100::getMode(uint8_t *mode)
{
    if (demo.active) { *mode = 0; return 0; }
    QByteArray out, in;
    int res = command(cmGetMode, out, in);
    if (!res)
    {
        uint8_t m;
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> m;
        *mode = m;
    }
    return res;
}

int Wm100::setZero()
{
    if (demo.active) {
        demo.state = dmNone;
        demo.status.tare = demo.status.weight = 0.0;
        return 0;
    }
    QByteArray out("0030"), in;
    return command(cmSetZero, out, in);
}

int Wm100::setTare()
{
    if (demo.active) {
        if (demo.status.weight <= demo.spec.tare) {
            demo.status.tare = demo.status.weight;
            demo.status.weight = 0.0;
            demo.state = dmNone;
            return 0;
        }
        else return 151;
    }
    QByteArray out("0030"), in;
    return command(cmSetTare, out, in);
}

int Wm100::setTareValue(const double_t tare)
{
    if (demo.active) {
        if (tare <= demo.spec.tare) {
            demo.status.tare = tare;
            demo.status.weight -= tare;
            demo.status.state |= 0x0008;
            return 0;
        }
        else return 151;
    }
    QByteArray out("0030"), in;
    quint16 n = qRound(tare / qPow(10, channelParams.measure));
    out << n;
    return command(cmSetTareValue, out, in);
}

int Wm100::calibWritePoint(const uint8_t num, const double_t weight)
{
    if (demo.active) return 123;
    QByteArray out("0030"), in;
    quint16 n = qRound(weight / qPow(10, channelParams.measure));
    out << num << n;
    return command(cmCalibWritePoint, out, in);
}

int Wm100::calibReadPoint(const uint8_t num, double_t *weight)
{
    if (demo.active) return 123;
    QByteArray out("0030"), in;
    out << num;
    int res = command(cmCalibReadPoint, out, in);
    if (!res)
    {
        uint16_t w;
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> w;
        *weight = w * qPow(10, channelParams.measure);
    }
    return res;
}

int Wm100::calibStart()
{
    if (demo.active) return 123;
    QByteArray out("0030"), in;
    return command(cmCalibStart, out, in);
}

int Wm100::calibStop()
{
    if (demo.active) return 123;
    QByteArray out("0030"), in;
    return command(cmCalibStop, out, in);
}

int Wm100::calibStatus(calib_status *status)
{
    if (demo.active) return 123;
    QByteArray out("0030"), in;
    int res = command(cmCalibStatus, out, in);
    if (!res)
    {
        uint16_t w;
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> status->channel_number >> w >> status->reference_point_state;
        status->reference_point = w;
    }
    return res;
}

int Wm100::getADC(uint32_t *ADCValue)
{
    if (demo.active) return 123;
    QByteArray out("0030"), in;
    int res = command(cmGetADC, out, in);
    if (!res)
    {
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> *ADCValue;
    }
    return res;
}

int Wm100::calibAccStart()
{
    if (demo.active) return 123;
    QByteArray out("0030"), in;
    return command(cmCalibAccStart, out, in);
}

void Wm100::startPolling(int time)
{
    timerid = startTimer(time);
    timerInterval = time;
}

void Wm100::stopPolling()
{
    killTimer(timerid);
    timerid = 0;
    timerInterval = 0;
}

bool Wm100::isConnected()
{
    return (io && io->isOpen()) || demo.active;
}

QString Wm100::errorDescription(const int err) const
{
    QString desc;
    switch (err)
    {
    case -20: desc = "Соединение не установлено"; break;
    case -18: desc = "Неверный тип устройства"; break;
    case -15: desc = "Команда не реализуется в данной версии"; break;
    case -11: desc = "Интерфейс не поддерживается"; break;
    case -9:  desc = "Параметр вне диапазона"; break;
    case -8:  desc = "Неожиданный ответ"; break;
    case -5:  desc = "Ошибка чтения из порт"; break;
    case -4:  desc = "Ошибка записи в порт"; break;
    case -3:  desc = "Порт недоступен"; break;
    case -2:  desc = "Порт занят другим приложением"; break;
    case -1:  desc = "Нет связи"; break;
    case 0:   desc = "Ошибок нет"; break;
    case 17:  desc = "Ошибка в значении тары"; break;
    case 120: desc = "Неизвестная команда"; break;
    case 121: desc = "Неверная длина данных команды"; break;
    case 122: desc = "Неверный пароль"; break;
    case 123: desc = "Команда не реализуется в данном режиме"; break;
    case 124: desc = "Неверное значение параметра"; break;
    case 150: desc = "Ошибка при попытке установки нуля"; break;
    case 151: desc = "Ошибка при установке тары"; break;
    case 152: desc = "Вес не фиксирован"; break;
    case 166: desc = "Сбой энергонезависимой памяти"; break;
    case 167: desc = "Команда не реализуется интерфейсом"; break;
    case 170: desc = "Исчерпан лимит попыток обращения с неверным паролем"; break;
    case 180: desc = "Режим градуировки блокирован градуировочным переключателем"; break;
    case 181: desc = "Клавиатура заблокирована"; break;
    case 182: desc = "Нельзя поменять тип текущего канала"; break;
    case 183: desc = "Нельзя выключить текущий канал"; break;
    case 184: desc = "С данным каналом ничего нельзя делать"; break;
    case 185: desc = "Неверный номер канала"; break;
    case 186: desc = "Нет ответа от АЦП"; break;
    default:  desc = "Неизвестная ошибка";
    }
    return desc;
}

void Wm100::timerEvent(QTimerEvent *event)
{
    if (timerid == event->timerId())
    {
        killTimer(timerid);
        channel_status st;
        int res = getStatus(&st);
        if (res != lastStatusError)
        {
            lastStatusError = res;
            emit errorStatusChanged(res);
        }
        if (!res && lastStatus != st)
        {
            lastStatus = st;
            emit weightStatusChanged(st);
        }
        emit pollingStatus(res);
        timerid = startTimer(timerInterval);
    }
    if (demo.timerid == event->timerId())
    {
        static const double_t weightCoef[7] = {1.00, 0.98, 0.96, 0.92, 0.85, 0.70, 0.20};
        switch (demo.state)
        {
        case dmNone:
            demo.status.weight = -demo.status.tare;
            demo.status.state |= 0x0002;
            demo.status.state &= 0xffee;
            if (qRound(demo.status.tare * 1000)) demo.status.state |= 0x0008;
            else demo.status.state &= 0xfff7;
            demo.weightFix = 0.001 * QRandomGenerator::global()->bounded(40, 15000);
            demo.stepZero = 30;
            demo.state = dmZero;
            break;
        case dmZero:
            demo.stepZero--;
            if (!demo.stepZero) {
                demo.status.state &= 0xfffd;
                demo.stepWaitFix = 7;
                demo.state = dmWaitFix;
            }
            break;
        case dmWaitFix:
            demo.stepWaitFix--;
            demo.status.weight = demo.weightFix * weightCoef[demo.stepWaitFix] - demo.status.tare;
            if (!demo.stepWaitFix) {
                demo.status.state |= 0x0011;
                demo.stepFix = 30;
                demo.state = dmFix;
            }
            break;
        case dmFix:
            demo.stepFix--;
            if (!demo.stepFix) {
                demo.status.state &= 0xffee;
                demo.stepWaitZero = 3;
                demo.state = dmWaitZero;
            }
            break;
        case dmWaitZero:
            demo.stepWaitZero--;
            demo.status.weight = demo.weightFix * demo.stepWaitZero / 3 - demo.status.tare;
            if (!demo.stepWaitFix) {
                demo.state = dmNone;
            }
            break;
        default: demo.state = dmNone;
        }
    }
}

int Wm100::getDeviceMetrics()
{
    if (demo.active) return 0;
    QByteArray out, in;
    int res = command(cmGetDeviceMetrics, out, in);
    if (!res)
    {
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> deviceMetrics.type
           >> deviceMetrics.subtype
           >> deviceMetrics.protocol_version
           >> deviceMetrics.model
           >> deviceMetrics.lang;
        in.remove(0,6);
        QString name(in);
        name.remove('\0');
        deviceMetrics.name = name.trimmed();
    }
    return res;
}

int Wm100::getChannelParam()
{
    if (demo.active) return 0;
    QByteArray out, in;
    out += (char)0;
    int res = command(cmGetChannelParam, out, in);
    if (!res)
    {
        uint16_t max,min,tare,ranges[3];
        uint8_t  discreteness[4];
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> channelParams.flags
           >> channelParams.reserved0
           >> channelParams.measure
           >> max >> min >> tare
           >> ranges[0] >> ranges[1] >> ranges[2]
           >> discreteness[0] >> discreteness[1] >> discreteness[2] >> discreteness[3]
           >> channelParams.calibration_points_number
           >> channelParams.reserved1;
        channelParams.max = max * qPow(10, channelParams.measure);
        channelParams.min = min * qPow(10, channelParams.measure);
        channelParams.tare = tare * qPow(10, channelParams.measure);
        channelParams.ranges[0] = ranges[0] * qPow(10, channelParams.measure);
        channelParams.ranges[1] = ranges[1] * qPow(10, channelParams.measure);
        channelParams.ranges[2] = ranges[2] * qPow(10, channelParams.measure);
        channelParams.discreteness[0] = discreteness[0] * qPow(10, channelParams.measure);
        channelParams.discreteness[1] = discreteness[1] * qPow(10, channelParams.measure);
        channelParams.discreteness[2] = discreteness[2] * qPow(10, channelParams.measure);
        channelParams.discreteness[3] = discreteness[3] * qPow(10, channelParams.measure);
    }
    return res;
}

int Wm100::resetDevice()
{
    return -19;
}

char Wm100::crc(const QByteArray &buf)
{
    char res = 0;
    foreach (const char c, buf) res ^= c;
    return res;
}

bool Wm100::checkForDemo(const QString &uri)
{
    static QString RegexStr("^demo:\\/\\/([\\w\\-\\/\\.]+)(\\?(\\w+=\\w+)(\\&(\\w+=\\w+))*)?$");
    return uri.contains(QRegularExpression(RegexStr,QRegularExpression::CaseInsensitiveOption));
}

void Wm100::initDemo()
{
    demo =  {
        false, dmNone, 0, 0, 0, 0, 0.0, 0,
        { 0x06, 0.0, 0.0, 0 },
        { 0x0E, 0, -3, 15.0, 0.04, 7.5, {6.0, 15.0, 0.0}, {2, 5, 0, 0} , 2, 0},
        { 1, 3, 0x0100, 0, 1, "SLIM DEMO v1.0 SHTRIH-M"}
    };
}

void Wm100::startDemo()
{
    demo.timerid = startTimer(150);
    demo.active = demo.timerid!=0;
}

void Wm100::stopDemo()
{
    if (demo.timerid) killTimer(demo.timerid);
    initDemo();
}

int Wm100::command(t_command cmd, const QByteArray &out, QByteArray &in)
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
