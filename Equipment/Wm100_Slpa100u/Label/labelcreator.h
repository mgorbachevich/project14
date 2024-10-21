#ifndef LABELCREATOR_H
#define LABELCREATOR_H

#include <QObject>
#include <QImage>
#include <QFileInfo>
#include "labelobject.h"

struct PrintData
{
    QString weight;
    QString price;
    QString cost;
    QString tare;
    QString barcode;
    QString itemcode;
    QString name;
    QString shelflife;
    QString selldate;
    QString currequiv;
    QString certificate;
    QString message;
    QString shop;
    QString operatorcode;
    QString operatorname;
    QString date;
    QString time;
    QString labelnumber;
    QString scalesnumber;
    QString picturefile;
    QString textfile;
    QString producedate;
    QString name2;
    QString price2;
    QString cost2;
    QString code2;
    QString packagedate;
    QString validitydate;
};

class LabelCreator : public QObject
{
public:
    explicit LabelCreator(QObject *parent = nullptr);

public:
    struct prjsettings {
        bool isLabel54;
        uint16_t width;
        uint16_t height;
     };
    const uint8_t VERSION_OLD = 2;
    const uint8_t VERSION = 3;
public:
    bool getLoaded() { return loaded; }
    int loadLabel(const QString &fileName);
    QImage createImage(const PrintData &data);

private:
    QList<LabelObject*> o;
    prjsettings prj;
    bool loaded = false;
    void clear();
    int openProject(const QString &fileName);
    QList<QString> openStrings(const QString &fileName);
    int openParams(const QString &fileName);
    int decodeParams(const QByteArray &data, const QFileInfo fileInfo, const QList<QString> &strings);

signals:

};

#endif // LABELCREATOR_H
