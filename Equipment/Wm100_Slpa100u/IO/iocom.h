#ifndef IOCOM_H
#define IOCOM_H

#include <QSerialPort>  // it needs "QT += serialport" in *.pro file
#include "iobase.h"

class IoCom : public IoBase
{
public:
    explicit IoCom(QObject *parent = nullptr);
    virtual ~IoCom();

    virtual bool open(const QString &uri);
    virtual void close();
    virtual void clear();
    virtual bool write(const QByteArray &data);
    virtual bool read(QByteArray &data, int64_t toRead, int timeoutRead = 0);
    virtual bool isOpen();

    static bool checkUri(const QString &uri);

private:
    int timeout = 100;
    QSerialPort* serialPort = nullptr;
    QString getPort(const QString &uri);
    int getBaudRate(const QString &uri);
    int getTimeout(const QString &uri);
    static QString getRegexStr();
    QString queryArgumentValue(const QString &uri, const QString &key);
    void setError(int e);

};

#endif // IOCOM_H
