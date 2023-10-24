#ifndef IOBASE_H
#define IOBASE_H

#include <QObject>

class IoBase : public QObject
{
    Q_OBJECT
public:
    explicit IoBase(QObject *parent = nullptr);

    virtual bool open(const QString &uri) = 0;
    virtual void close() = 0;
    virtual void clear() = 0;
    virtual bool write(const QByteArray &data) = 0;
    virtual bool read(QByteArray &data, int64_t toRead, int timeoutRead = 0) = 0;
    virtual bool writeRead(const QByteArray &out, QByteArray &in, int64_t toRead, int timeoutRead = 0) = 0;
    virtual bool isOpen() = 0;
    virtual void setOption(const int index, const int option, const QString &s1 = "", const QString &s2 = "") {};
    qint32 error = 0;
};

#endif // IOBASE_H
