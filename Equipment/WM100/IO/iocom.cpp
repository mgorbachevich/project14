#include <QRegularExpression>
#include "iocom.h"

IoCom::IoCom(QObject *parent): IoBase(parent)
{
    serialPort = new QSerialPort(this);
}

IoCom::~IoCom()
{
    delete serialPort;
}

QString IoCom::queryArgumentValue(const QString &uri, const QString &key)
{
    QRegularExpression reg("([\\&|\\?])" + key + "(=(\\w+))");
    QRegularExpressionMatch match = reg.match(uri);
    return match.captured(3);
}

QString IoCom::getPort(const QString &uri)
{
    QRegularExpression reg(getRegexStr());
    QRegularExpressionMatch match = reg.match(uri);
    return match.captured(1);
}

int IoCom::getBaudRate(const QString &uri)
{
    QString res = queryArgumentValue(uri, "baudrate");
    bool ok;
    int baud = res.toInt(&ok);
    if (!ok || !baud) baud = 115200;
    return baud;
}

int IoCom::getTimeout(const QString &uri)
{
    QString res = queryArgumentValue(uri, "timeout");
    bool ok;
    int t = res.toInt(&ok);
    if (!ok || !t) t = 100;
    return t;
}

bool IoCom::open(const QString &uri)
{
    if (!checkUri(uri)) return false;
    close();
    QString port = getPort(uri);
    if (port.isEmpty()) return false;

    serialPort->setPortName(port);
    serialPort->setBaudRate(getBaudRate(uri));
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setDataBits(QSerialPort::Data8);
    timeout = getTimeout(uri);

    if (!serialPort->open(QSerialPort::ReadWrite))
    {
        setError(serialPort->error());
        serialPort->clearError();
        return false;
    }
    return true;
}

void IoCom::close()
{
    if (serialPort->isOpen())
    {
        serialPort->clear();
        serialPort->close();
        while (serialPort->isOpen()) { ; }
    };
}

void IoCom::setError(int e)
{
    switch (e) {
    case QSerialPort::NoError: error = 0; break;
    case QSerialPort::DeviceNotFoundError: error = -3; break;
    case QSerialPort::PermissionError: error = -2; break;
    case QSerialPort::WriteError: error = -4; break;
    case QSerialPort::ReadError: error = -5; break;
    case QSerialPort::NotOpenError: error = -20; break;
    case QSerialPort::UnsupportedOperationError:
    case QSerialPort::ResourceError:
    case QSerialPort::UnknownError:
    case QSerialPort::TimeoutError:
    case QSerialPort::OpenError: error = -1; break;
    default: error = -1;
    }
}

QString IoCom::getRegexStr()
{
    static QString RegexStr("^serial:\\/\\/([\\w\\-\\/\\.]+)(\\?(\\w+=\\w+)(\\&(\\w+=\\w+))*)?$");
    return RegexStr;
}

void IoCom::clear()
{
    if (serialPort->isOpen()) serialPort->clear();
}

bool IoCom::write(const QByteArray &data)
{
    int64_t result = 0;

    if (serialPort->isOpen()) {
        result = serialPort->write(data);
        while (serialPort->bytesToWrite()) serialPort->waitForBytesWritten(timeout);
    }
    setError(serialPort->error());
    return result==data.size();
}

bool IoCom::read(QByteArray &data, int64_t toRead, int timeoutRead)
{
    QByteArray d;
    if (timeoutRead==0) timeoutRead = timeout;
    if (serialPort->isOpen())
    {
        if (serialPort->bytesAvailable() || serialPort->waitForReadyRead(timeoutRead)) {
            if (serialPort->bytesAvailable())
                d += serialPort->read(std::min(serialPort->bytesAvailable(), toRead-d.size()));
            int n = toRead*2;
            while (d.size()<toRead && n-- && serialPort->waitForReadyRead(10))
            {
                if (serialPort->bytesAvailable())
                    d += serialPort->read(std::min(serialPort->bytesAvailable(), toRead-d.size()));
            }
        }
        else {
            serialPort->waitForReadyRead(timeoutRead);
            if (serialPort->bytesAvailable()) {
                d += serialPort->read((serialPort->bytesAvailable()));
            }
        }
    }
    data += d;
    return d.size()==toRead;
}

bool IoCom::isOpen()
{
    return serialPort->isOpen();
}

bool IoCom::checkUri(const QString &uri)
{
    return uri.contains(QRegularExpression(getRegexStr(),QRegularExpression::CaseInsensitiveOption));
}
