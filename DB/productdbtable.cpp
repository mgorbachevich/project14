#include "productdbtable.h"

ProductDBTable::ProductDBTable(const QString& name, QObject *parent): DBTable(name, parent)
{
    qDebug() << "@@@@@ ProductDBTable::ProductDBTable";

    addColumn("Код",                    "code",             "TEXT PRIMARY KEY");
    addColumn("Штрих-код",              "barcode",          "TEXT");
    addColumn("Наименование",           "name",             "TEXT");
    addColumn("Краткое наименование",   "name2",            "TEXT");
    addColumn("Короткий код",           "code2",            "TEXT");
    addColumn("Тип",                    "type",             "INT");
    addColumn("Цена",                   "price",            "REAL");
    addColumn("Ценовая база",           "price_base",       "INT");
    addColumn("Цена 2",                 "price2",           "REAL");
    addColumn("Код схемы скидки",       "discount_code",    "INT");
    addColumn("Код группы",             "group_code",       "TEXT");
    addColumn("Масса штуки",            "unit_weight",      "INT");
    addColumn("Код формата этикетки",   "label_format",     "INT");
    addColumn("Код формата этикетки 2", "label_format2",    "INT");
    addColumn("Код формата этикетки 3", "label_format3",    "INT");
    addColumn("Формат ШК",              "barcode_format",   "TEXT");
    addColumn("Масса тары",             "tare",             "REAL");
    addColumn("Сертификат товара",      "certificate",      "TEXT");
    addColumn("Срок годности",          "shelflife",        "INT");
    addColumn("Дата реализации",        "sell_date",        "DATE");
    addColumn("Дата производства",      "produce_date",     "DATE");
    addColumn("Дата упаковки",          "packing_date",     "DATE");
    addColumn("Код файла картинки",     "picture_code",     "INT");
    addColumn("Код сообщения",          "message_code",     "INT");
    addColumn("Код файла сообщения",    "messageFile_code", "INT");
    addColumn("Код файла ролика",       "movie_code",       "INT");
    addColumn("Код звукового файла",    "sound_code",       "INT");
}

bool ProductDBTable::isSuitableForShowcase(const DBRecord& record)
{
    return (record.count() >= ProductDBTable::Columns::COLUMN_COUNT) && !isGroup(record);
}

bool ProductDBTable::isGroup(const DBRecord& record)
{
    return (record.count() >= ProductDBTable::Columns::COLUMN_COUNT) &&
            record[ProductDBTable::Columns::Type].toInt() == ProductDBTable::ProductType::ProductType_Group;
}
