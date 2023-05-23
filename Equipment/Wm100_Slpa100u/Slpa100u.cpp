#include <QRegularExpression>
#include <QTimerEvent>
#include "Slpa100u.h"
#include "Slpa100uProtocolCom.h"
#include "Slpa100uProtocolUsb.h"
#include "Slpa100uProtocolDemo.h"


Slpa100u::Slpa100u(QObject *parent)
    : QObject{parent}
{
    disconnectDevice();
}

int Slpa100u::connectDevice(const QString &uri)
{
    disconnectDevice();
    if (Slpa100uProtocolCom::checkUri(uri)) protocol = new Slpa100uProtocolCom(this);
    else if (Slpa100uProtocolUsb::checkUri(uri)) protocol = new Slpa100uProtocolUsb(this);
    else if (Slpa100uProtocolDemo::checkUri(uri)) protocol = new Slpa100uProtocolDemo(this);
    else return -11;

    int res = 0;
    res = protocol->open(uri);
    Slpa100uProtocol::prnanswer status;
    if (!res) res = getStatus(&status);

    if (res) disconnectDevice();
    return res;
}

int Slpa100u::disconnectDevice()
{
    stopPolling();
    lastStatusError = 0;
    lastStatus = 0;
    if (protocol != nullptr) delete protocol;
    protocol = nullptr;
    return 0;
}

void Slpa100u::startPolling(int time)
{
    timerInterval = time;
    timerid = startTimer(time);
}

void Slpa100u::stopPolling()
{
    killTimer(timerid);
    timerid = 0;
    timerInterval = 0;
}

bool Slpa100u::isConnected()
{
    return protocol != nullptr;
}

QString Slpa100u::errorDescription(const int err) const
{
    QString desc;
    switch (err)
    {
    case -20: desc = "Соединение не установлено"; break;
    case -18: desc = "Неверный тип устройства"; break;
    case -17: desc = "Устройство занято обработкой предыдущей команды"; break;
    case -16: desc = "Получен ответ на предыдущую команду"; break;
    case -15: desc = "Команда не реализуется в данной версии"; break;
    case -11: desc = "Интерфейс не поддерживается"; break;
    case -9:  desc = "Параметр вне диапазона"; break;
    case -8:  desc = "Неожиданный ответ"; break;
    case -5:  desc = "Ошибка чтения из порта"; break;
    case -4:  desc = "Ошибка записи в порт"; break;
    case -3:  desc = "Порт недоступен"; break;
    case -2:  desc = "Порт занят другим приложением"; break;
    case -1:  desc = "Нет связи"; break;
    case 0:   desc = "Ошибок нет"; break;
    case 1:   desc = "Нет бумаги"; break;
    case 2:   desc = "Не найдена этикетка"; break;
    case 3:   desc = "Поднята печатающая головка"; break;
    case 4:   desc = "Не снята напечатанная этикетка"; break;
    case 5:   desc = "Перегрев печатающей головки"; break;
    case 9:   desc = "Печать прервана"; break;
    case 10:  desc = "Пустой буфер печати"; break;
    case 11:  desc = "Неизвестный тип печати"; break;
    case 12:  desc = "Печать прервана"; break;
    case 13:  desc = "Неверная длина для тестовой печати"; break;
    case 20:  desc = "Ошибка памяти"; break;
    default:  desc = "Неизвестная ошибка";
    }
    return desc;
}

int Slpa100u::print(QImage &pic)
{
    if (!isConnected()) return -20;
    QImage p(pic);
    p.convertTo(QImage::Format_Mono);

    int res = protocol->cClearBuffer();
    if (!res) res = protocol->cBufferSize(p.height());
    int n = qMin(56, (p.width()+7) / 8 );
    for (qint32 y = 0; y < p.height(); ++y) {
        if (res) break;
        char* tempLine = reinterpret_cast<char*>(p.scanLine(y));
        QByteArray buf(tempLine,n);
        buf[n-1] &= 0xff << (p.width() % 8); // обнуляем хвосты последнего байта
        res = protocol->cBufferData(buf);
    }
    Slpa100uProtocol::prnanswer status;
    if (!res) res = protocol->cPrint(&status);
    return res;
}

int Slpa100u::printTest(int lines)
{
    if (!isConnected()) return -20;
    Slpa100uProtocol::prnanswer status;
    return protocol->cPrintTest(lines, &status);
}

int Slpa100u::feed()
{
    if (!isConnected()) return -20;
    Slpa100uProtocol::prnanswer status;
    return protocol->cFeed(&status);
}

int Slpa100u::getStatus(Slpa100uProtocol::prnanswer *status)
{
    if (!isConnected()) return -20;
    return protocol->cGetStatus(status);
}

int Slpa100u::reset()
{
    if (!isConnected()) return -20;
    return protocol->cReset();
}

int Slpa100u::setBrightness(int value)
{
    if (!isConnected()) return -20;
    return protocol->cSetBrightness(value);
}

int Slpa100u::setOffset(int value)
{
    if (!isConnected()) return -20;
    if (value<0 || value>15) return -9;
    if (protocol->getPrinterVersion() < 0x0207) return protocol->cSetOffset(value);
    return protocol->cSetOffset2(value);
}

int Slpa100u::setPaper(Slpa100uProtocol::papertype value)
{
    if (!isConnected()) return -20;
    return protocol->cSetPaperType(value);
}

int Slpa100u::setSensor(bool value)
{
    if (!isConnected()) return -20;
    return protocol->cSensorControl(value);
}

int Slpa100u::getOffset(int *value)
{
    if (!isConnected()) return -20;
    if (protocol->getPrinterVersion() < 0x0207) return -15;
    uint8_t u8;
    int res = protocol->cGetOffset2(&u8);
    if (!res) *value = u8;
    return res;
}

void Slpa100u::timerEvent(QTimerEvent *event)
{
    if (timerid == event->timerId())
    {
        killTimer(timerid);
        Slpa100uProtocol::prnanswer status;
        int res = getStatus(&status);
        if (res != lastStatusError)
        {
            lastStatusError = res;
            emit printerErrorChanged(res);
        }
        if (!res && lastStatus != status.printerStatus)
        {
            lastStatus = status.printerStatus;
            emit printerStatusChanged(lastStatus);
        }
        emit pollingStatus(res);
        timerid = startTimer(timerInterval);
    }
}
