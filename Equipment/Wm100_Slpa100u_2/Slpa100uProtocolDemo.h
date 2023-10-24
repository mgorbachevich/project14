#ifndef SLPA100UPROTOCOLDEMO_H
#define SLPA100UPROTOCOLDEMO_H

#include "Slpa100uProtocol.h"
#include <QObject>
#include <QImage>


class Slpa100uProtocolDemo : public Slpa100uProtocol
{
    Q_OBJECT
public:
    explicit Slpa100uProtocolDemo(QObject *parent = nullptr);

public:
    static bool checkUri(const QString &uri);

private:
    prnanswer status = {1093, 3, 5, 42, 36, 25, 25, 0};
    uint8_t offset = 0;
    virtual int executeCommand(prncommand cmd, const QByteArray &out, QByteArray &in);
    virtual int open(const QString &uri);
    virtual void close() {};
    void showImage(const QImage &img);
    void printTest(uint16_t height);
};

#endif // SLPA100UPROTOCOLDEMO_H
