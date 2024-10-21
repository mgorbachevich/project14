#include <QFile>
#include <QPainter>
#include <QTextCodec>
#include "labelcreator.h"

LabelCreator::LabelCreator(QObject *parent)
    : QObject{parent}
{

}

int LabelCreator::loadLabel(const QString &fileName)
{
    int res = 0;
    clear();
    res = openProject(fileName);
    qDebug() << "loadLabel() openProject = " << res << ", fileName" << fileName;
    if (res == 0) res = openParams(fileName);
    qDebug() << "loadLabel() openParams = " << res << ", fileName" << fileName;
    if (res != 0) clear();
    loaded = res == 0;
    return res;
}

QImage LabelCreator::createImage(const PrintData &data)
{
    QImage pic(prj.width*8, prj.height*8, QImage::Format_Mono);
    if (!getLoaded()) return pic;
    pic.fill(Qt::black);
    for (auto obj: o)
    {
        switch (obj->getType())
        {
        case LabelObject::t_weight:      obj->setData(data.weight); break;
        case LabelObject::t_price:       obj->setData(data.price); break;
        case LabelObject::t_cost:        obj->setData(data.cost); break;
        case LabelObject::t_barcode:     obj->setData(data.barcode); break;
        case LabelObject::t_itemcode:    obj->setData(data.itemcode); break;
        case LabelObject::t_name:        obj->setData(data.name); break;
        case LabelObject::t_shelflife:   obj->setData(data.shelflife); break;
        case LabelObject::t_selldate:    obj->setData(data.selldate); break;
        case LabelObject::t_tare:        obj->setData(data.tare); break;
        case LabelObject::t_date:        obj->setData(data.date); break;
        case LabelObject::t_time:        obj->setData(data.time); break;
        case LabelObject::t_label:       obj->setData(data.labelnumber); break;
        case LabelObject::t_scales:      obj->setData(data.scalesnumber); break;
        case LabelObject::t_currequiv:   obj->setData(data.currequiv); break;
        case LabelObject::t_shop:        obj->setData(data.shop); break;
        case LabelObject::t_producedate:obj->setData(data.producedate); break;
        case LabelObject::t_name2:       obj->setData(data.name2); break;
        case LabelObject::t_price2:      obj->setData(data.price2); break;
        case LabelObject::t_cost2:       obj->setData(data.cost2); break;
        case LabelObject::t_code2:       obj->setData(data.code2); break;
        case LabelObject::t_packagedate: obj->setData(data.packagedate); break;
        case LabelObject::t_validitydate:obj->setData(data.validitydate); break;
        case LabelObject::t_message:
            if (obj->getDataSourceIndex() == 0) obj->setData(data.textfile);
            if (obj->getDataSourceIndex() == 1) obj->setData(data.message);
            break;
        case LabelObject::t_operator:
            if (obj->getDataSourceIndex() == 0) obj->setData(data.operatorname);
            if (obj->getDataSourceIndex() == 1) obj->setData(data.operatorcode);
            break;
        case LabelObject::t_graphic:
            if (obj->getDataSourceIndex() == 1) obj->setData(data.picturefile);
            break;
        case LabelObject::t_certification: obj->setData(data.certificate); break;
        }

        obj->draw(pic);
    }
    pic.invertPixels();
    return pic;
}

void LabelCreator::clear()
{
    prj = { false, 0, 0 };
    loaded = false;
    qDeleteAll(o);
    o.clear();
}

int LabelCreator::openProject(const QString &fileName)
{
    if (fileName.right(4) != ".lpr") return 102;

    QFile file(fileName);
    QByteArray data;
    if (file.open(QIODevice::ReadOnly))
    {
        data = file.readAll();
        file.close();
        if (data.size() != 16) return 101;
        if (data.at(0) != VERSION_OLD && data.at(0) != VERSION) return 103;
        prj.isLabel54 = (data.at(11) & 0x04) == 0;

    }
    else return 110;
    return 0;
}

QList<QString> LabelCreator::openStrings(const QString &fileName)
{
    QList<QString> res;
    QString name(fileName);
    name.remove(name.size()-4,4);
    name += ".dat/strings";

    QFile file(name);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
        QTextDecoder *decoder = codec->makeDecoder();
        while (!file.atEnd())
        {
            QString str = decoder->toUnicode(file.readLine());
            str.remove(QChar('\r'));
            str.remove(QChar('\n'));
            res.append(str);
        }
        file.close();
    }
    return res;
}

int LabelCreator::openParams(const QString &fileName)
{
    QList<QString> strings = openStrings(fileName);
    QFileInfo fileInfo(fileName);
    QString name = fileInfo.path() + "/" + fileInfo.baseName() + ".dat/" + fileInfo.baseName() + ".par";

    qDebug() << "openParams() fileName" << name;
    QFile file(name);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray data;
        data = file.readAll();
        file.close();
        qDebug() << "Файл параметров прочитан fileName = " << fileName << ", size = " << data.size();
        return decodeParams(data, fileInfo, strings);
    }
    else return 105;
    return 0;
}

int LabelCreator::decodeParams(const QByteArray &data, const QFileInfo fileInfo, const QList<QString> &strings)
{
    if (data.size() == 0) return 106;
    if (data.size() < 9)  return 107;
    if (data.at(0) != VERSION_OLD && data.at(0) != VERSION) return 104;

    QDataStream ds(data);
    ds.setByteOrder(QDataStream::LittleEndian);
    uint8_t b8;
    uint32_t count;
    ds >> b8 >> prj.width >> prj.height >> count;
    prj.height -= 3;
    if (prj.isLabel54) prj.width = qMin(prj.width-3, 54);
    else prj.width = qMin(prj.width-2, 56);
    if (9+count*55 != data.size()) return 108;

    LabelObject *p;
    for (uint32_t i = 0; i < count; ++i)
    {
        uint8_t b8_1, b8_2;
        ds >> b8_1 >> b8_2;
        LabelObject::objgroup gr = static_cast<LabelObject::objgroup>(b8_1);
        LabelObject::objtype  tp = static_cast<LabelObject::objtype>(b8_2);
        if (tp == LabelObject::t_graphic) p = new LabelObjectPicture();
        else if (tp == LabelObject::t_shape) p = new LabelObjectShape();
        else if (gr == LabelObject::g_string) p = new LabelObjectString();
        else if (tp == LabelObject::t_barcode) p = new LabelObjectBarcode();
        else p = new LabelObject();

        if (p->load(data.mid(9+i*55, 55), fileInfo, strings)) o.append(p);
        else
        {
            delete p;
            return 109;
        }
        ds.skipRawData(53);
    }
    return 0;
}
