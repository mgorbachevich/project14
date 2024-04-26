#include <QTimer>
#include "Wm100Protocol.h"

Wm100Protocol::Wm100Protocol(QObject *parent)
    : QObject{parent}
{

}

Wm100Protocol::~Wm100Protocol()
{
    close();
}

void Wm100Protocol::close()
{
    lastStatus = {0, 0.0, 0.0, 0};
    //if (!mtx.tryLock(3000))
    //{
        //QTimer::singleShot(10, this, [this]() { this->close(); } );
    //    return;
    //}
    if (io != nullptr)
    {
        io->close();
        delete io;
        io = nullptr;
    }
    //mtx.unlock();
}

bool Wm100Protocol::connected()
{
    return io && io->isOpen();
}

int Wm100Protocol::cGetStatus(channel_status *status)
{
    QByteArray out("0030"), in;
    int res = executeCommand(cmGetStatus, out, in);
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

int Wm100Protocol::cGetStatusEx(channel_status_ex *status)
{
    QByteArray out("0030"), in;
    int res = executeCommand(cmGetStatusEx, out, in);
    if (!res)
    {
        int32_t  weight, weight2;
        uint16_t tare;
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> status->state >> weight >> tare >> status->flags >> weight2 >> status->levelx >> status->levely;
        status->weight = weight * qPow(10, channelParams.measure);
        status->tare   = tare   * qPow(10, channelParams.measure);
        status->weightprecise = weight2 * qPow(10, -4);
    }
    return res;
}

int Wm100Protocol::cSetMode(uint8_t mode)
{
    QByteArray out("0030"), in;
    out += mode;
    return executeCommand(cmSetMode, out, in);
}

int Wm100Protocol::cGetMode(uint8_t *mode)
{
    QByteArray out, in;
    int res = executeCommand(cmGetMode, out, in);
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

int Wm100Protocol::cSetZero()
{
    QByteArray out("0030"), in;
    return executeCommand(cmSetZero, out, in);
}

int Wm100Protocol::cSetTare()
{
    QByteArray out("0030"), in;
    return executeCommand(cmSetTare, out, in);
}

int Wm100Protocol::cSetTareValue(const double_t tare)
{
    QByteArray out("0030"), in;
    quint16 n = qRound(tare / qPow(10, channelParams.measure));
    out += quint8(n);
    out += quint8(n>>8);
    return executeCommand(cmSetTareValue, out, in);
}

int Wm100Protocol::cCalibWritePoint(const uint8_t num, const double_t weight)
{
    QByteArray out("0030"), in;
    quint16 n = qRound(weight / qPow(10, channelParams.measure));
    out += num;
    out += quint8(n);
    out += quint8(n>>8);
    return executeCommand(cmCalibWritePoint, out, in);
}

int Wm100Protocol::cCalibReadPoint(const uint8_t num, double_t *weight)
{
    QByteArray out("0030"), in;
    out += num;
    int res = executeCommand(cmCalibReadPoint, out, in);
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

int Wm100Protocol::cCalibStart()
{
    QByteArray out("0030"), in;
    return executeCommand(cmCalibStart, out, in);
}

int Wm100Protocol::cCalibStop()
{
    QByteArray out("0030"), in;
    return executeCommand(cmCalibStop, out, in);
}

int Wm100Protocol::cCalibStatus(calib_status *status)
{
    QByteArray out("0030"), in;
    int res = executeCommand(cmCalibStatus, out, in);
    if (!res)
    {
        uint16_t w;
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> status->channel_number >> w >> status->reference_point_state;
        status->reference_point = w * qPow(10, channelParams.measure);
    }
    return res;
}

int Wm100Protocol::cCalibAccStart()
{
    QByteArray out("0030"), in;
    return executeCommand(cmCalibAccStart, out, in);
}

int Wm100Protocol::cControllerId(controller_id *id)
{
    QByteArray out, in;
    int res = executeCommand(cmControllerID, out, in);
    if (!res)
    {
        id->id = in.first(16);
        in.remove(0,16);
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> id->crc16;
        in = in.mid(2, in.size()-4);
        QString firmware(in);
        auto n = firmware.indexOf('\0');
        if (n >= 0) firmware.remove(n, firmware.size()-n);
        id->firmware = firmware.trimmed();
    }
    return res;
}

int Wm100Protocol::cGetDeviceMetrics()
{
    QByteArray out, in;
    int res = executeCommand(cmGetDeviceMetrics, out, in);
    if (!res)
    {
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds  >> deviceMetrics.type
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

int Wm100Protocol::cGetChannelParam()
{
    QByteArray out, in;
    out += (char)0;
    int res = executeCommand(cmGetChannelParam, out, in);
    if (!res)
    {
        uint16_t max,min,tare,ranges[3];
        uint8_t  discreteness[4];
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds  >> channelParams.flags
            >> channelParams.reserved0
            >> channelParams.measure
            >> max >> min >> tare
            >> ranges[0] >> ranges[1] >> ranges[2]
            >> discreteness[0] >> discreteness[1] >> discreteness[2] >> discreteness[3]
            >> channelParams.calibration_points_number
            >> channelParams.calibrations_number;
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

int Wm100Protocol::cResetDevice()
{
    return -19;
}

int Wm100Protocol::cGetADC(uint32_t *ADCValue)
{
    QByteArray out("0030"), in;
    int res = executeCommand(cmGetADC, out, in);
    if (!res)
    {
        QDataStream ds(in);
        ds.setByteOrder(QDataStream::LittleEndian);
        ds >> *ADCValue;
    }
    return res;
}

int Wm100Protocol::cSetDateTime(const QDateTime &datetime, const QString &uri)
{
    return 0;
}

void Wm100Protocol::getDeviceMetrics(device_metrics *metrics)
{
   *metrics = deviceMetrics;
}

void Wm100Protocol::getChannelParam(channel_specs *params)
{
    *params = channelParams;
}


