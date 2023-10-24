#include <QPainter>
#include "labelobject.h"

LabelObject::LabelObject(QObject *parent)
    : QObject{parent}
{

}

bool LabelObject::load(const QByteArray &ba, const QFileInfo &fileInfo, const QList<QString> &stringList)
{
    QDataStream ds(ba);
    uint8_t b8, b8_1, b8_2;
    uint32_t i32;
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> b8_1 >> b8_2 >> left >> top >> width >> height >>
        nStrings >> alignment >> isVisible >> isTransparent >>
        isSelected >> isFrame >> frameBorder >> frameForm >>
        i32 >> i32 >> i32;
    group = static_cast<objgroup>(b8_1);
    type  = static_cast<objtype>(b8_2);
    if (i32 < stringList.size()) data = stringList.at(i32);
    ds >> i32;
    if (withSymbolDecode())
        isSymbol = (i32 & 0x00010000)?0:1;
    ds >> i32;
    // font 17 байт
    ds >> i32; // name
    if (i32 < stringList.size()) font.setFamily(stringList.at(i32));
    ds >> b8;  // charset
    ds >> i32; // color
    fontColor.setRgb(static_cast<QRgb>(i32)); // color
    ds >> i32; // size
#ifdef Q_OS_ANDROID
    i32 *= 1.3;
#endif
    font.setPointSize(i32);
    ds >> b8; // bold
    font.setBold(b8);
    ds >> b8; // italic
    font.setItalic(b8);
    ds >> b8; // underline
    font.setUnderline(b8);
    ds >> b8; // strikeout
    font.setStrikeOut(b8);
    return true;
}

bool LabelObject::draw(QImage &dest)
{
    if (!isVisible) return true;

    static int n = 0;
    //data = QString("Text %1").arg(n++);
    //isFrame = 1;
    //frameForm = 0;

    QPainter painter;
    painter.begin(&dest);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setBackgroundMode((isTransparent) ? Qt::BGMode::TransparentMode : Qt::BGMode::OpaqueMode);
    QRect rect(left*8, top*8, width*8, height*8);
    if (!isTransparent) painter.fillRect(rect, Qt::white);
    int flags  = 0;
    int vAlign = alignment / 3;
    int gAlign = alignment % 3;
    switch (gAlign)
    {
        case 0: flags |= Qt::AlignLeft; break;
        case 1: flags |= Qt::AlignRight; break;
        case 2: flags |= Qt::AlignVCenter; break;
    }
    switch (vAlign)
    {
        case 0: flags |= Qt::AlignTop; break;
        case 1: flags |= Qt::AlignHCenter; break;
        case 2: flags |= Qt::AlignBottom; break;
    }
        if (isMultiline()) flags |= Qt::TextWordWrap;
    else flags |= Qt::TextSingleLine;

    painter.setFont(font);

    QBrush brush = painter.brush();
    brush.setStyle(Qt::NoBrush);
    painter.setBrush(brush);
    QRect boundingRect;
    painter.drawText(rect, flags, data, &boundingRect);

    if (isFrame)
    {
        if (frameBorder==1) rect = rect.adjusted(0, 0, -1, -1);
        else rect = rect.intersected(boundingRect);
        if (frameForm==0) painter.drawRect(rect);
        if (frameForm==1) painter.drawRoundedRect(rect, 2, 2);
    }
    painter.end();
    return true;
}

bool LabelObject::withSymbolDecode()
{
    static const QList<objtype> withSymbolList = {t_weight, t_price, t_cost, t_tare, t_operator, t_message};
    return withSymbolList.contains(type);
}

bool LabelObject::isMultiline()
{
    static const QList<objtype> isMultiLine = {t_name, t_shop, t_message, t_string};
    return isMultiLine.contains(type);
}

int LabelObject::getDataSourceIndex()
{
    int res = 0;
    if (getType() == t_operator || getType() == t_graphic  || getType() == t_message)
        res = isSymbol;
    return res;
}

// ----------------LabelObjectPicture----------------

bool LabelObjectPicture::load(const QByteArray &ba, const QFileInfo &fileInfo, const QList<QString> &stringList)
{
    LabelObject::load(ba, fileInfo, stringList);
    QDataStream ds(ba);
    uint32_t i32;
    ds.setByteOrder(QDataStream::LittleEndian);
    ds.skipRawData(30);
    ds >> i32;
    stretch  = i32 & 0x00000001;
    isSymbol = (i32 & 0x00000002)?1:0;
    if (!isSymbol && !data.isEmpty()) data = fileInfo.path() + "/" + fileInfo.baseName() + ".dat/" + data;

    ds >> i32;
    ds.skipRawData(17);
    return true;
}

bool LabelObjectPicture::draw(QImage &dest)
{
    if (!isVisible) return true;
    if (!image.load(data)) return true;

    QPainter painter;
    painter.begin(&dest);
    /*left -= 10;
    width *= 2;
    height *= 2;
    stretch = false;
    isFrame = true;*/
    if (stretch)
    {
        painter.drawImage(QRect(left*8, top*8, width*8, height*8), image);
        if (isFrame) painter.drawRect(left*8, top*8, width*8, height*8);
    }
    else {
        painter.drawImage(QPoint(left*8, top*8), image, QRect(0, 0, image.width(), image.height()));
        if (isFrame) painter.drawRect(QRect(left*8, top*8, image.width(), image.height()));
    }
    painter.end();
    return true;
}

// ----------------LabelObjectBarcode----------------

bool LabelObjectBarcode::load(const QByteArray &ba, const QFileInfo &fileInfo, const QList<QString> &stringList)
{
    LabelObject::load(ba, fileInfo, stringList);
    QDataStream ds(ba);
    uint32_t i32;
    ds.setByteOrder(QDataStream::LittleEndian);
    ds.skipRawData(30);
    ds >> i32;
    bcType = static_cast<bctype>(alignment);
    isText = i32 & 0x01;
    dotsPerLine = (i32 >> 8) & 0xff;
    autoErrorDetectionLevel = i32 & 0x00000040;
    errorDetectionLevel = frameBorder;
    ds >> i32;

    /*if NotFirstBarcode then begin
        p.Caption:=BCValue;
        p.Alignment:=BCType;
        p.FrameBorder:=BCMin;
        p.FrameForm:=BCMax;
        p.Tag:=p.Tag or integer(BCControl);
      end
      else begin
        BCValue:=p.Caption;
        BCType:=p.Alignment;
        BCMin:=p.FrameBorder;
        BCMax:=p.FrameForm;
        BCControl:=lo(p.Tag) and $40;
        NotFirstBarcode:=true;
      end;*/

    ds.skipRawData(17); // font
    return true;
}

bool LabelObjectBarcode::draw(QImage &dest)
{
    if (!isVisible) return true;
    if (bcType > BC_QR) return false;

    bc.setText(data);
    //bcType = BC_EAN13;
    left -= 0;
    width += 0;
    height *= 1;

    QImage img(width*8+((bcType==BC_EAN13)?8:0), height*8, QImage::Format_Mono);
    img.fill(Qt::black);
    QPainter painter;
    painter.begin(&img);
    switch (bcType)
    {
    case BC_EAN13:
        bc.setSymbol(BARCODE_EANX);
        bc.setGuardDescent((isText) ? 5.0 : 0.0);
        bc.setNoQuietZones(true);
        break;
    case BC_CODE39:
        bc.setSymbol(BARCODE_CODE39);
        bc.setOption2((autoErrorDetectionLevel)?2:0);
        break;
    case BC_CODE128:
        bc.setSymbol(BARCODE_CODE128);
        break;
    case BC_2of5:
        bc.setSymbol(BARCODE_C25INTER);
        bc.setOption2((autoErrorDetectionLevel)?2:0);
        break;
    case BC_PDF417:
        bc.setSymbol(BARCODE_PDF417);
        if (autoErrorDetectionLevel) bc.setOption1(-1);
        else bc.setOption1(errorDetectionLevel);
        break;
    case BC_QR:
        if (autoErrorDetectionLevel) bc.setOption1(3);
        else bc.setOption1(errorDetectionLevel);
        bc.setSymbol(BARCODE_QRCODE);
        break;
    }
    bc.setShowText(isText);
    bc.render(painter, QRectF(0, 0, width*8+((bcType==BC_EAN13)?8:0), height*8));
    painter.end();
    if (true)

    painter.begin(&dest);
    painter.drawImage( left*8, top*8, img.copy(0,0,12,height*8));
    painter.drawImage( left*8+12, top*8, img.copy(18,0,width*8+8-18,height*8));
    //painter.drawImage( left*8, top*8, img);
    painter.end();
    return true;
}

// ----------------LabelObjectString----------------

bool LabelObjectString::load(const QByteArray &ba, const QFileInfo &fileInfo, const QList<QString> &stringList)
{
    LabelObject::load(ba, fileInfo, stringList);
    QDataStream ds(ba);
    uint32_t i32, i32_2;
    ds.setByteOrder(QDataStream::LittleEndian);
    ds.skipRawData(30);
    ds >> i32 >> i32_2;
    nStrings = i32;
    data = "";
    for (int i = 0; i < nStrings; ++i)
    {
        if (i32_2 < stringList.size()) data += stringList.at(i32_2+i);
        if (i < nStrings-1) data += "\r\n";
    }

    ds.skipRawData(17);
    return true;
}
