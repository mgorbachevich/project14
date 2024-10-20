#ifndef LABELOBJECT_H
#define LABELOBJECT_H

#include <QObject>
#include <QPixmap>
#include <QFont>
#include <QFileInfo>
#include "qzint.h"

class LabelObject : public QObject
{
    Q_OBJECT
public:
    explicit LabelObject(QObject *parent = nullptr);

    enum objgroup {
        g_std,
        g_graphic,
        g_string
    };
    enum objtype {
        t_weight = 0,
        t_price,
        t_cost,
        t_barcode,
        t_itemcode,
        t_name,
        t_shelflife,
        t_validity,
        t_tare,
        t_date,
        t_time,
        t_label,
        t_scales,
        t_currequiv,
        t_certification,
        t_message,
        t_shop,
        t_operator,
        t_manufactured,
        t_name2,
        t_price2,
        t_cost2,
        t_graphic = 100,
        t_shape,
        t_string = 200
    };
    enum objalignment {
        topleft = 0,
        topright,
        topcenter,
        centerleft,
        centerright,
        centercenter,
        bottomleft,
        bottomright,
        bottomcenter,
    };
    enum bctype {
        BC_EAN13 = 0,
        BC_CODE39,
        BC_CODE128,
        BC_2of5,
        BC_PDF417,
        BC_QR,
    };

protected:
    objgroup group;
    objtype  type;
    int16_t  left;
    int16_t  top;
    int16_t  width;
    int16_t  height;
    int8_t   nStrings;
    int8_t   alignment;
    int8_t   isVisible;
    int8_t   isTransparent;
    int8_t   isSelected;
    int8_t   isFrame;
    int8_t   frameBorder;
    int8_t   frameForm;
    QString  data;
    QFont    font;
    QColor   fontColor;
    int8_t   isSymbol;

private:
    bool withSymbolDecode();
    bool isMultiline();

public:
    objtype  getType() { return type; }
    int getDataSourceIndex();
    void setData(const QString &s) { data = s; }
    virtual bool load(const QByteArray &ba, const QFileInfo &fileInfo, const QList<QString> &stringList);
    virtual bool draw(QImage &dest);

signals:

};

class LabelObjectPicture : public LabelObject
{
public:
    explicit LabelObjectPicture(QObject *parent = nullptr) : LabelObject(parent) {};
    virtual bool load(const QByteArray &ba, const QFileInfo &fileInfo, const QList<QString> &stringList);
    virtual bool draw(QImage &dest);

private:
    QImage image;
    bool   stretch;
};

class LabelObjectShape : public LabelObject
{
public:
    explicit LabelObjectShape(QObject *parent = nullptr) : LabelObject(parent) {};
    virtual bool draw(QImage &dest);
};

class LabelObjectBarcode : public LabelObject
{
public:
    explicit LabelObjectBarcode(QObject *parent = nullptr) : LabelObject(parent) {};
    virtual bool load(const QByteArray &ba, const QFileInfo &fileInfo, const QList<QString> &stringList);
    virtual bool draw(QImage &dest);

private:
    Zint::QZint bc;
    bool        isText;
    bool        autoErrorDetectionLevel;
    int32_t     errorDetectionLevel;
    int32_t     dotsPerLine;
    bctype      bcType;
    //int32_t     tag;
};

class LabelObjectString : public LabelObject
{
public:
    explicit LabelObjectString(QObject *parent = nullptr) : LabelObject(parent) {};
    virtual bool load(const QByteArray &ba, const QFileInfo &fileInfo, const QList<QString> &stringList);
};

#endif // LABELOBJECT_H
