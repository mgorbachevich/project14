#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QEventLoop>
//#include <thread>
#include "Wm100ProtocolHttp2.h"
#include "IO/iohttp.h"

Wm100ProtocolHttp2::Wm100ProtocolHttp2(QObject *parent)
    : Wm100Protocol{parent}
{
    deviceInterface = diHttp;
}

int Wm100ProtocolHttp2::open(const QString &uri)
{
    close();
    io = new IoHttp(this);
    if (!io->open(uri)) return io->error;
    return 0;
}

int Wm100ProtocolHttp2::cSetDateTime(const QDateTime &datetime, const QString &uri)
{
    int res = open(uri);
    if (!res)
    {
        io->setOption(0, 1);
        io->setOption(1, 0, "/api/v0/i2c/setsystemtime");
        io->setOption(2, 0);
        io->setOption(2, 1, "year", QString("%1").arg(datetime.date().year()));
        io->setOption(2, 1, "month", QString("%1").arg(datetime.date().month()));
        io->setOption(2, 1, "day", QString("%1").arg(datetime.date().day()));
        io->setOption(2, 1, "hours", QString("%1").arg(datetime.time().hour()));
        io->setOption(2, 1, "minutes", QString("%1").arg(datetime.time().minute()));
        io->setOption(2, 1, "seconds", QString("%1").arg(datetime.time().second()));
        QByteArray out, in;
        if (!io->writeRead(out, in, 0, 3000)) res = -1;
        close();
    }
    return res;
}

int Wm100ProtocolHttp2::cDaemonVersion(QString &version, QString &build, const QString &uri)
{
    int res = open(uri);
    if (!res)
    {
        io->setOption(0, 0);
        io->setOption(1, 0, "/api/v0/info");
        io->setOption(2, 0);
        QByteArray out, in;
        if (!io->writeRead(out, in, 0, 3000)) res = -1;
        //qDebug() << in;

        QJsonObject jsonObject = QJsonDocument::fromJson(in).object();
        jsonObject = jsonObject["info"].toObject();
        version = jsonObject["version"].toString();
        build = jsonObject["build"].toString();
        close();
    }
    return res;
}

int Wm100ProtocolHttp2::cKillDaemon(const QString &uri)
{
    int res = open(uri);
    if (!res)
    {
        io->setOption(0, 1);
        io->setOption(1, 0, "/stopthedaemon");
        io->setOption(2, 0);
        QByteArray out, in;
        if (!io->writeRead(out, in, 0, 3000)) res = -1;
        //qDebug() << in;
        close();
    }
    return res;
}

int Wm100ProtocolHttp2::cDisplayData(const display_data &dd, const QString &uri)
{
    int res = open(uri);
    if (!res)
    {
        io->setOption(0, 1);
        io->setOption(1, 0, "/api/v0/extlcd/fields");
        io->setOption(2, 0);
        io->setOption(2, 1, "massa", dd.weight);
        io->setOption(2, 1, "price", dd.price);
        io->setOption(2, 1, "total", dd.cost);
        io->setOption(2, 1, "tare", dd.tare);
        io->setOption(2, 1, "text", QString(QUrl::toPercentEncoding(dd.text)));
        QString flags;
        if (dd.flZero) flags += "zero+";
        if (dd.flTare) flags += "tare+";
        if (dd.flCalm) flags += "calm+";
        if (dd.flAuto) flags += "auto+";
        if (dd.flMemory) flags += "memory+";
        if (dd.flUprow) flags += "uprow+";
        if (dd.flPieces) flags += "pieces+";
        if (dd.flTools) flags += "tools+";
        if (dd.flDataExchange) flags += "dataexchange+";
        if (dd.flPencil) flags += "pencil+";
        if (dd.flLock) flags += "lock+";
        if (dd.flPerKg) flags += "perkilo+";
        if (dd.flPer100g) flags += "perhundredg+";
        if (flags.endsWith('+')) flags.remove(flags.length()-1,1);
        io->setOption(2, 1, "flags", flags);
        QByteArray out, in;
        if (!io->writeRead(out, in, 0, 3000)) res = -1;
        //qDebug() << in;
        if (!res && in != "ok") res = -51;
        close();
    }
    return res;
}

int Wm100ProtocolHttp2::cGetStatus(channel_status *status)
{
    if (status == nullptr) return -9;
    memset(status, 0, sizeof(channel_status));
    channel_status_ex st;
    int res = cGetStatusEx(&st);
    if (!res)
    {
        status->state  = st.state;
        status->weight = st.weight;
        status->tare   = st.tare;
        status->flags  = st.flags;
    }
    return res;
}

int Wm100ProtocolHttp2::cGetStatusEx(channel_status_ex *status)
{
    if (status == nullptr) return -9;
    memset(status, 0, sizeof(channel_status_ex));
    QByteArray in;
    int res = executeCommandGet("wmstate", in);
    if (!res)
    {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(in);
        QJsonObject jsonObject = jsonResponse.object();
        //if (jsonObject["weight"]     != QJsonValue::Undefined)
        status->weight        = jsonObject["weight"].toInt(0) * qPow(10, channelParams.measure);
        status->tare          = jsonObject["tare"].toInt(0) * qPow(10, channelParams.measure);
        status->weightprecise = jsonObject["weight_aux"].toInt(0) * qPow(10, channelParams.measure-1);
        status->state         = jsonObject["wm_state"].toInt(0);
        status->flags  = 0;
        status->levelx = 0;
        status->levely = 0;
    }
    return res;
}

int Wm100ProtocolHttp2::cSetMode(uint8_t mode)
{
    return executeCommandPost("set_mode", QString("%1").arg(mode));
}

int Wm100ProtocolHttp2::cGetMode(uint8_t *mode)
{
    if (mode == nullptr) return -9;
    QByteArray in;
    int res = executeCommandGet("wmstate", in);
    if (!res) *mode = parseParamInt(in, "mode");
    return res;
}

int Wm100ProtocolHttp2::cSetZero()
{
    return executeCommandPost("zero");
}

int Wm100ProtocolHttp2::cSetTare()
{
    return executeCommandPost("tare");
}

int Wm100ProtocolHttp2::cSetTareValue(const double_t tare)
{
    return executeCommandPost("tare", QString("%1").arg(qRound(tare / qPow(10, channelParams.measure))));
}

int Wm100ProtocolHttp2::cCalibWritePoint(const uint8_t num, const double_t weight)
{
    return -19;
}

int Wm100ProtocolHttp2::cCalibReadPoint(const uint8_t num, double_t *weight)
{
    return -19;
}

int Wm100ProtocolHttp2::cCalibStart()
{
    return executeCommandPost("calibr_start");
}

int Wm100ProtocolHttp2::cCalibStop()
{
    return executeCommandPost("calibr_stop");
}

int Wm100ProtocolHttp2::cCalibStatus(calib_status *status)
{
    if (status == nullptr) return -9;
    QByteArray in;
    int res = executeCommandGet("wmstate", in);
    if (!res)
    {
        QJsonDocument json = QJsonDocument::fromJson(in);
        QJsonObject jsonObject = json.object();
        if (jsonObject["calibration_state"] != QJsonValue::Undefined)
        {
            jsonObject = jsonObject["calibration_state"].toObject();
            status->channel_number        = jsonObject["channel_number"].toInt(0);
            status->reference_point       = jsonObject["r_point_weight"].toInt(0) * qPow(10, channelParams.measure);
            status->reference_point_state = jsonObject["r_point_state"].toInt(0);
            //qDebug() << in;
        }
    }
    return res;
}

int Wm100ProtocolHttp2::cCalibAccStart()
{
    return -19;
}

int Wm100ProtocolHttp2::cControllerId(controller_id *id)
{
    if (id == nullptr) return -9;
    QByteArray in;
    int res = executeCommandGet("wmstate", in);
    if (!res)
    {
        QJsonDocument jsonResponse = QJsonDocument::fromJson(in);
        QJsonObject jsonObject = jsonResponse.object();
        id->id = jsonObject["chip_id"].toString().toLatin1();
        id->crc16 = jsonObject["fw_crc"].toString().toUInt(nullptr, 16);
        id->firmware = jsonObject["build_n_board_info"].toString();
    }
    return res;
}

int Wm100ProtocolHttp2::cGetDeviceMetrics()
{
    QByteArray in;
    int res = executeCommandGet("wmstate", in);
    if (!res)
    {
        QJsonDocument json = QJsonDocument::fromJson(in);
        QJsonObject jsonObject = json.object();
        if (jsonObject["device_info"] != QJsonValue::Undefined)
        {
            jsonObject = jsonObject["device_info"].toObject();
            deviceMetrics.type    = jsonObject["dev_type"].toInt(0);
            deviceMetrics.subtype = jsonObject["dev_subtype"].toInt(0);
            deviceMetrics.protocol_version = jsonObject["prot_ver"].toInt(0) * 256 + jsonObject["prot_subver"].toInt(0);
            deviceMetrics.model   = jsonObject["model"].toInt(0);
            deviceMetrics.lang    = jsonObject["lang"].toInt(0);
            deviceMetrics.name    = jsonObject["name"].toString();
        }
    }
    return res;
}

int Wm100ProtocolHttp2::cGetChannelParam()
{
    QByteArray in;
    memset(&channelParams, 0, sizeof(channel_specs));
    int res = executeCommandGet("wmstate", in);
    if (!res)
    {
        QJsonDocument json = QJsonDocument::fromJson(in);
        QJsonObject jsonObject = json.object();
        if (jsonObject["weight_channel_info"] != QJsonValue::Undefined)
        {
            jsonObject = jsonObject["weight_channel_info"].toObject();
            channelParams.flags     = jsonObject["flags"].toInt(0);
            channelParams.dec_point = jsonObject["decpointpos"].toInt(0);
            channelParams.measure   = jsonObject["power"].toInt(-3);
            channelParams.max       = jsonObject["max"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.min       = jsonObject["min"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.tare      = jsonObject["taremax"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.ranges[0] = jsonObject["diap_1"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.ranges[1] = jsonObject["diap_2"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.ranges[2] = jsonObject["diap_3"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.discreteness[0] = jsonObject["disc_1"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.discreteness[1] = jsonObject["disc_2"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.discreteness[2] = jsonObject["disc_3"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.discreteness[3] = jsonObject["disc_4"].toInt(0) * qPow(10, channelParams.measure);
            channelParams.calibration_points_number = jsonObject["calibr_points_amount"].toInt(0);
            channelParams.calibrations_number = jsonObject["reserved"].toInt(0);
        }
    }
    return res;
}

int Wm100ProtocolHttp2::cResetDevice()
{
    return executeCommandPost("reset");
}

int Wm100ProtocolHttp2::cGetADC(uint32_t *ADCValue)
{
    if (ADCValue == nullptr) return -9;
    QByteArray in;
    int res = executeCommandGet("wmstate", in);
    if (!res) *ADCValue = parseParamInt(in, "adc");
    return res;
}

bool Wm100ProtocolHttp2::checkUri(const QString &uri)
{
    static QString RegexStr("^http:\\/\\/([\\w\\-\\/\\.]+)(\\:(\\d+))$");
    return uri.contains(QRegularExpression(RegexStr,QRegularExpression::CaseInsensitiveOption));
}

int Wm100ProtocolHttp2::executeCommand(wmcommand cmd, const QByteArray &out, QByteArray &in)
{
    return -19;
}

int Wm100ProtocolHttp2::executeCommandGet(const QString cmd, QByteArray &in)
{
    if (!io) return -20;
    int res = -1;

    QByteArray out;
    io->setOption(0, 0); // GET
    io->setOption(1, 0, "/api/v0/i2c/" + cmd);
    io->setOption(2, 0);
    bool b = io->writeRead(out, in, 0, 3000);
    if (b) res = parseError(in);
    return res;
}

int Wm100ProtocolHttp2::executeCommandPost(const QString cmd, const QString val)
{
    if (!io) return -20;
    int res = -1;

    QByteArray out, in;
    io->setOption(0, 1); // POST
    io->setOption(1, 0, "/api/v0/i2c/wmcommand");
    io->setOption(2, 0);
    io->setOption(2, 1, "cmd", cmd);
    if (!val.isEmpty()) io->setOption(2, 1, "cmd_val", val);
    bool b = io->writeRead(out, in, 0, 3000);
    if (b) res = parseCommandError(in);
    return res;
}

int Wm100ProtocolHttp2::parseError(const QByteArray &reply)
{
    int res = 0;
    int polling_err = parseParamInt(reply, "polling_err");
    if (polling_err) res = polling_err - 40;
    return res;
}

int Wm100ProtocolHttp2::parseCommandError(const QByteArray &reply)
{
    int res = 0;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply);
    QJsonObject jsonObject = jsonResponse.object();
    int i2c_err = jsonObject["i2c_err"].toInt(0);
    int cmd_err = jsonObject["cmd_err"].toInt(0);
    if (i2c_err) res = i2c_err - 30;
    else res = cmd_err;
    return res;
}

int Wm100ProtocolHttp2::parseParamInt(const QByteArray &reply, const QString param)
{
    int res = 0;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(reply);
    QJsonObject jsonObject = jsonResponse.object();
    res = jsonObject[param].toInt(0);
    return res;
}
