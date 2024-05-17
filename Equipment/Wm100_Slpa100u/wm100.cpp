#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include <QRandomGenerator>
#include <QRegularExpression>
#include "qcoreevent.h"
#include "wm100.h"
#include "Wm100ProtocolCom.h"
#include "Wm100ProtocolHttp.h"
#include "Wm100ProtocolDemo.h"


bool operator==(Wm100Protocol::channel_status &r1, Wm100Protocol::channel_status &r2)
{
    return r1.flags==r2.flags && r1.state==r2.state && r1.weight==r2.weight && r1.tare==r2.tare;
}

bool operator!=(Wm100Protocol::channel_status &r1, Wm100Protocol::channel_status &r2)
{
    return !(r1==r2);
}

Wm100::Wm100(QObject *parent)
    : QObject{parent}
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()), Qt::QueuedConnection);
    disconnectDevice();
}

Wm100::~Wm100()
{
    disconnectDevice();
}

int Wm100::readConnectParam(const QString &filename, const QString &param, QString &uri)
{
    int res = 0;
    QFile file(filename);
    QByteArray data;
    if (!file.exists()) res = -21;
    if (!res && file.open(QIODevice::ReadOnly))
    {
        data = file.readAll();
        if (file.error() != QFileDevice::NoError) res = -24;
        else if (data.isEmpty()) res = -25;
        file.close();
    }
    else if (file.error() == QFileDevice::PermissionsError) res = -22;
    else res = -23;

    if (!res)
    {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
        QJsonObject   jsonObject = jsonDocument.object();
        QJsonValue    jsonValue = jsonObject[param];
        if (jsonValue != QJsonValue::Null) uri = jsonValue.toString();
        else res = -26;
    }
    return res;
}

Wm100Protocol::deviceinterface Wm100::getInterface()
{
    if (protocol != nullptr) return protocol->getInterface();
    else return Wm100Protocol::diNone;
}

Wm100Protocol::deviceinterface Wm100::checkUri(const QString &uri)
{
    if (Wm100ProtocolCom::checkUri(uri)) return Wm100Protocol::diCom;
    else if (Wm100ProtocolHttp::checkUri(uri)) return Wm100Protocol::diHttp;
    else if (Wm100ProtocolDemo::checkUri(uri)) return Wm100Protocol::diDemo;
    return Wm100Protocol::diNone;
}

int Wm100::connectDevice(const QString &uri)
{
    disconnectDevice();
    if (Wm100ProtocolCom::checkUri(uri)) protocol = new Wm100ProtocolCom(this);
    else if (Wm100ProtocolHttp::checkUri(uri)) protocol = new Wm100ProtocolHttp(this);
    else if (Wm100ProtocolDemo::checkUri(uri)) protocol = new Wm100ProtocolDemo(this);
    else return -11;

    int res = protocol->open(uri);
    if (!res) res = protocol->cGetDeviceMetrics();
    if (!res) res = protocol->cGetChannelParam();

    if (res) disconnectDevice();
    return res;
}

int Wm100::disconnectDevice()
{
    qDebug() << "disconnectDevice - thread() =" << this->thread();
    stopPolling();
    lastStatusError = 0;
    lastStatus = {0, 0.0, 0.0, 0};
    if (protocol != nullptr)
    {
        protocol->close();
        delete protocol;
        protocol = nullptr;
    }
    return 0;
}

int Wm100::getStatus(Wm100Protocol::channel_status *status)
{
    if (!isConnected()) return -20;
    int res = protocol->cGetStatus(status);
    // if (!res) qDebug() << "getStatus - res =" << res;
    return res;
}

int Wm100::getStatusEx(Wm100Protocol::channel_status_ex *status)
{
    if (!isConnected()) return -20;
    return protocol->cGetStatusEx(status);
}

int Wm100::setMode(uint8_t mode)
{
    if (!isConnected()) return -20;
    return protocol->cSetMode(mode);
}

int Wm100::getMode(uint8_t *mode)
{
    if (!isConnected()) return -20;
    return protocol->cGetMode(mode);
}

int Wm100::setZero()
{
    if (!isConnected()) return -20;
    return protocol->cSetZero();
}

int Wm100::setTare()
{
    if (!isConnected()) return -20;
    return protocol->cSetTare();
}

int Wm100::setTareValue(const double_t tare)
{
    if (!isConnected()) return -20;
    return protocol->cSetTareValue(tare);
}

int Wm100::calibWritePoint(const uint8_t num, const double_t weight)
{
    if (!isConnected()) return -20;
    return protocol->cCalibWritePoint(num, weight);
}

int Wm100::calibReadPoint(const uint8_t num, double_t *weight)
{
    if (!isConnected()) return -20;
    return protocol->cCalibReadPoint(num, weight);
}

int Wm100::calibStart()
{
    if (!isConnected()) return -20;
    return protocol->cCalibStart();
}

int Wm100::calibStop()
{
    if (!isConnected()) return -20;
    return protocol->cCalibStop();
}

int Wm100::calibStatus(Wm100Protocol::calib_status *status)
{
    if (!isConnected()) return -20;
    return protocol->cCalibStatus(status);
}

int Wm100::getADC(uint32_t *ADCValue)
{
    if (!isConnected()) return -20;
    return protocol->cGetADC(ADCValue);
}

int Wm100::controllerId(Wm100Protocol::controller_id *id)
{
    if (!isConnected()) return -20;
    return protocol->cControllerId(id);
}

int Wm100::killDaemon()
{
    Wm100ProtocolHttp* pr = new Wm100ProtocolHttp(this);
    int res =  pr->cKillDaemon("http://127.0.0.1:51232");
    delete pr;
    return res;
}

int Wm100::getDaemonVersion(QString &version, QString &build)
{
    Wm100ProtocolHttp* pr = new Wm100ProtocolHttp(this);
    int res =  pr->cDaemonVersion(version, build, "http://127.0.0.1:51232");
    delete pr;
    return res;
}

int Wm100::setDateTime(const QDateTime &datetime)
{
    Wm100ProtocolHttp* pr =  new Wm100ProtocolHttp(this);
    int res = pr->cSetDateTime(datetime, "http://127.0.0.1:51232");
    delete pr;
    return res;
}

int Wm100::calibAccStart()
{
    if (!isConnected()) return -20;
    return protocol->cCalibAccStart();
}

void Wm100::startPolling(int time)
{
    QTimer::singleShot(time, this, [this]() { this->onTimer(); } );
    timerInterval = time;
}

void Wm100::stopPolling()
{
    timerInterval = 0;
}

void Wm100::onTimer()
{
    if (!timerInterval) return;
    Wm100Protocol::channel_status st;
    int res = getStatus(&st);
    if (res != -13)
    {
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
    }
    emit pollingStatus(res);
    if (timerInterval) QTimer::singleShot(timerInterval, this, [this]() { this->onTimer(); } );
}

bool Wm100::isConnected()
{
    return protocol != nullptr && protocol->connected();
}

QString Wm100::
    errorDescription(const int err) const
{
    QString desc;
    switch (err)
    {
    case -35: desc = "Ошибка на линии i2c"; break;
    case -34:
    case -33:
    case -32:
    case -31: desc = "Устройство отсутствует на линии"; break;
    case -26: desc = "Параметр не найден"; break;
    case -25: desc = "Пустой файл"; break;
    case -24: desc = "Ошибка чтения файла"; break;
    case -23: desc = "Ошибка открытия файла"; break;
    case -22: desc = "Нет разрешений на чтение файла"; break;
    case -21: desc = "Файл не найден"; break;
    case -20: desc = "Соединение не установлено"; break;
    case -19: desc = "Команда не реализуется драйвером"; break;
    case -18: desc = "Неверный тип устройства"; break;
    case -17: desc = "Устройство занято обработкой предыдущей команды"; break;
    case -16: desc = "Получен ответ на предыдущую команду"; break;
    case -15: desc = "Команда не реализуется в данной версии"; break;
    case -13: desc = "Контрольная сумма не совпадает"; break;
    case -12: desc = "Устройство отсутствует на линии"; break;
    case -11: desc = "Интерфейс не поддерживается"; break;
    case -9:  desc = "Параметр вне диапазона"; break;
    case -8:  desc = "Неожиданный ответ"; break;
    case -5:  desc = "Ошибка чтения из порта"; break;
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
    qDebug() << "timerEvent - thread() =" << this->thread();
    if (timerid == event->timerId())
    {
        qDebug() << "timerEvent.killTimer timerid =" << timerid;
        killTimer(timerid);
        timerid = 0;
        Wm100Protocol::channel_status st;
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
        if (timerInterval) timerid = startTimer(timerInterval);
        qDebug() << "timerEvent.startTimer(" << timerInterval << ") timerid =" << timerid;
    }
}

int Wm100::getDeviceMetrics(Wm100Protocol::device_metrics *deviceMetrics)
{
    if (!isConnected()) return -20;
    protocol->getDeviceMetrics(deviceMetrics);
    return 0;
}

int Wm100::getChannelParam(Wm100Protocol::channel_specs *channelParam)
{
    if (!isConnected()) return -20;
    protocol->getChannelParam(channelParam);
    return 0;
}

int Wm100::resetDevice()
{
    return -19;
}
