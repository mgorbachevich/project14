#include <QRegularExpression>
#include <QRandomGenerator>
#include "Wm100ProtocolDemo.h"
#include "qcoreevent.h"

Wm100ProtocolDemo::Wm100ProtocolDemo(QObject *parent)
    : Wm100Protocol{parent}
{
    deviceInterface = diDemo;
}

int Wm100ProtocolDemo::open(const QString &uri)
{
    initDemo();
    demo.timerid = startTimer(150);
    demo.active = demo.timerid!=0;
    return (demo.active)?0:-1;
}

void Wm100ProtocolDemo::close()
{
    if (demo.timerid) killTimer(demo.timerid);
    initDemo();
}

bool Wm100ProtocolDemo::connected()
{
    return demo.timerid != 0;
}

void Wm100ProtocolDemo::initDemo()
{
    demo =  {
        false, dmNone, 0, 0, 0, 0, 0.0, 0,
        { 0x06, 0.0, 0.0, 0 },
        { 0x0E, 0, -3, 15.0, 0.04, 7.5, {6.0, 15.0, 0.0}, {2, 5, 0, 0} , 2, 0},
        { 1, 3, 0x0100, 0, 1, "SLIM DEMO v1.0 SHTRIH-M"}
    };
}

int Wm100ProtocolDemo::cGetStatus(channel_status *status)
{
    *status = demo.status;
    return 0;
}

int Wm100ProtocolDemo::cGetStatusEx(channel_status_ex *status)
{
    status->weight = demo.status.weight;
    status->tare = demo.status.tare;
    status->state = demo.status.state;
    status->flags = demo.status.flags;
    status->weightprecise = demo.status.weight+0.1;
    status->levelx = 127;
    status->levely = 127;
    return 0;
}

int Wm100ProtocolDemo::cSetMode(uint8_t mode)
{
    demo.mode = mode;
    return 0;
}

int Wm100ProtocolDemo::cGetMode(uint8_t *mode)
{
    *mode = demo.mode;
    return 0;
}

int Wm100ProtocolDemo::cSetZero()
{
    demo.state = dmNone;
    demo.status.tare = demo.status.weight = 0.0;
    return 0;
}

int Wm100ProtocolDemo::cSetTare()
{
    if (demo.status.weight <= demo.spec.tare) {
        demo.status.tare = demo.status.weight;
        demo.status.weight = 0.0;
        demo.state = dmNone;
        return 0;
    }
    else return 151;
}

int Wm100ProtocolDemo::cSetTareValue(const double_t tare)
{
    if (tare <= demo.spec.tare) {
        demo.status.tare = tare;
        demo.status.weight -= tare;
        demo.status.state |= 0x0008;
        return 0;
    }
    else return 151;
}

int Wm100ProtocolDemo::cGetADC(uint32_t *ADCValue)
{
    *ADCValue = QRandomGenerator::global()->bounded(22535438, 22536438);
    return 0;
}
/*
int Wm100Protocol::cControllerId(controller_id *id)
{
    id->crc16 = 0xc0de;
    id->id = QByteArray(16, 7);
    id->firmware = "Feb 12 2016 23:59:01 v0.1 SME22022.121_1";
    return 0;
}*/

bool Wm100ProtocolDemo::checkUri(const QString &uri)
{
    static QString RegexStr("^demo:\\/\\/([\\w\\-\\/\\.]+)(\\?(\\w+=\\w+)(\\&(\\w+=\\w+))*)?$");
    return uri.contains(QRegularExpression(RegexStr, QRegularExpression::CaseInsensitiveOption));
}

void Wm100ProtocolDemo::timerEvent(QTimerEvent *event)
{
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
            demo.stepZero = 50;
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
                demo.stepFix = 50;
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


