#include <QRegularExpression>
#include <QtWidgets>
#include "Slpa100uProtocolDemo.h"

Slpa100uProtocolDemo::Slpa100uProtocolDemo(QObject *parent)
    : Slpa100uProtocol{parent}
{
}

bool Slpa100uProtocolDemo::checkUri(const QString &uri)
{
    static QString RegexStr("^demo:\\/\\/([\\w\\-\\/\\.]+)(\\?(\\w+=\\w+)(\\&(\\w+=\\w+))*)?$");
    return uri.contains(QRegularExpression(RegexStr,QRegularExpression::CaseInsensitiveOption));
}

int Slpa100uProtocolDemo::executeCommand(prncommand cmd, const QByteArray &out, QByteArray &in)
{
    static uint16_t height = 0;
    static QImage pic(1, 1, QImage::Format_Mono);

    int res = 0;
    uint8_t n = 0;
    switch (cmd) {
    case cmProtectionMode:
    case cmFeed: res = 0; break;
    case cmBufferSize:
        if (out.size()>=2) height = (uint8_t)out[0]+(uint8_t)out[1]*256;
        else height = 0;
        pic = pic.scaled(56*8, height);
        pic.fill(0);
        res = 0; break;
    case cmBufferData: {
        if (pic.height() > 0) {
            char* p = reinterpret_cast<char*>(pic.scanLine(pic.height()-height));
            for (int i = 0; i<qMin(pic.width()/8,out.size()); i++, p++) *p = out[i];
            height--;
            if (!height) pic.invertPixels();
        }
        res = 0;
    } break;
    case cmClearBuffer: pic = pic.scaled(1, 1); res = 0; break;
    case cmPrint:
        if (pic.width()>1) showImage(pic);
        in = statusToByteArray(status);
        res = 0; break;
    case cmPrintTest:
        if (out.size()>=2) printTest((uint8_t)out[0]+(uint8_t)out[1]*256);
        res = 0; break;
    case cmGetPE:
    case cmGetLAB:
    case cmGetVersion:
    case cmGetStatus: in = statusToByteArray(status); res = 0; break;
    case cmSetBrightness: if (out.size()) status.brightness = out[0]; res = 0; break;
    case cmSetOffset:
    case cmSetOffset2: if (out.size()) offset = out[0]; res = 0; break;
    case cmSetPaperType:
        if (out.size()) n = out[0];
        if (n==0) status.printerStatus &= 0xffdf; else status.printerStatus |= 0x0020;
        res = 0; break;
    case cmSensorControl:
        if (out.size()) n = out[0];
        if (n==0) status.printerStatus &= 0xffbf; else status.printerStatus |= 0x0040;
        res = 0; break;
    case cmReset: res = 0; break;
    case cmGetOffset2: in = statusToByteArray(status); in[8] = offset; res = 0; break;
    case cmMemoryTest:
    case cmErrorCnt:
    case cmResetCnt:
    case cmProtectionCnt: in.append(11,0); res = 0; break;
    default: res = -15;
    }
    return res;
}

int Slpa100uProtocolDemo::open(const QString &uri)
{
    if (!checkUri(uri)) return -11;
    return 0;
}

void Slpa100uProtocolDemo::showImage(const QImage &img)
{
    QDialog dlg(nullptr);
    QVBoxLayout verticalLayout(&dlg);
    QScrollArea sa(&dlg);
    QLabel lbl;
    lbl.setPixmap(QPixmap::fromImage(img));
    lbl.setFrameShape(QFrame::Box);
    sa.setWidget(&lbl);
    sa.resize(lbl.width(), lbl.height());
    int height = dlg.height();
    int width = lbl.width()+50;
    dlg.resize(width, height);
    verticalLayout.addWidget(&sa);
    dlg.exec();
}

void Slpa100uProtocolDemo::printTest(uint16_t height)
{
    QImage img(56*8, height, QImage::Format_Mono);
    img.fill(0);
    for (int j = 0; j < img.height(); j++) {
        char* p = reinterpret_cast<char*>(img.scanLine(j));
        for (int i = 0; i < img.bytesPerLine(); i++, p++) {
            if ((i & 1) == ((j/8) & 1)) *p = 0xff;
        }
    }
    showImage(img);
}
