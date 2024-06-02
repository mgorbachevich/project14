#include "productdbtable.h"
#include "tools.h"

ProductDBTable::ProductDBTable(const QString& name, QObject *parent): DBTable(name, parent)
{
    Tools::debugLog("@@@@@ ProductDBTable::ProductDBTable");

    addColumn("Код #",                  "code",              "TEXT PRIMARY KEY");
    addColumn("Штрих-код",              "barcode",           "TEXT");
    addColumn("Наименование",           "name",              "TEXT");
    addColumn("Краткое наименование",   "name2",             "TEXT");
    addColumn("Наименование*",          "upper_name",        "TEXT");
    addColumn("Номер №",                "code2",             "TEXT");
    addColumn("Тип",                    "type",              "INT");
    addColumn("Цена",                   "price",             "INT");
    addColumn("Ценовая база",           "price_base",        "INT");
    addColumn("Цена 2",                 "price2",            "INT");
    addColumn("Код схемы скидки",       "discount_code",     "INT");
    addColumn("Код группы",             "group_code",        "TEXT");
    addColumn("Масса штуки",            "unit_weight",       "INT");
    addColumn("Код формата этикетки",   "label_format",      "INT");
    addColumn("Код формата этикетки 2", "label_format2",     "INT");
    addColumn("Код формата этикетки 3", "label_format3",     "INT");
    addColumn("Формат ШК",              "barcode_format",    "TEXT");
    addColumn("Масса тары",             "tare",              "REAL");
    addColumn("Сертификат товара",      "certificate",       "TEXT");
    addColumn("Срок годности",          "shelflife",         "INT");
    addColumn("Дата реализации",        "sell_date",         "DATE");
    addColumn("Дата производства",      "produce_date",      "DATE");
    addColumn("Дата упаковки",          "packing_date",      "DATE");
    addColumn("Код файла картинки",     "picture_code",      "INT");
    addColumn("Код сообщения",          "message_code",      "INT");
    addColumn("Код файла сообщения",    "message_file_code", "INT");
    addColumn("Код файла ролика",       "movie_code",        "INT");
    addColumn("Код звукового файла",    "sound_code",        "INT");
    addColumn("В избранном",            "favorite",          "INT"); // Да/нет
}

const DBRecord ProductDBTable::checkRecord(const DBRecord& record)
{
    Tools::debugLog("@@@@@ ProductDBTable::checkRecord " + name);
    DBRecord result;
    if (record.count() < columnCount())
    {
        Tools::debugLog("@@@@@ ProductDBTable::checkRecord ERROR");
        return result;
    }
    int code = Tools::stringToInt(record.at(Columns::Code), 0);
    int groupCode = Tools::stringToInt(record.at(Columns::GroupCode), 0);
    result.append(record);
    result.replace(Columns::Code, QString("%1").arg(code));
    result.replace(Columns::GroupCode, QString("%1").arg(groupCode));
    result.replace(Columns::UpperName, record.at(Columns::Name).toString().toUpper());
    return result;
}

bool ProductDBTable::isForShowcase(const DBRecord& record)
{
    return (record.count() >= ProductDBTable::COLUMN_COUNT) && !isGroup(record);
}

bool ProductDBTable::isGroup(const DBRecord& record)
{
    return (record.count() >= ProductDBTable::COLUMN_COUNT) &&
            record[ProductDBTable::Type].toInt() == ProductType_Group;
}

bool ProductDBTable::isPiece(const DBRecord& record)
{
    return (record.count() >= ProductDBTable::COLUMN_COUNT) &&
            record[ProductDBTable::Type].toInt() == ProductType_Piece;
}

bool ProductDBTable::is100gBase(const DBRecord& record)
{
    return (record.count() >= ProductDBTable::COLUMN_COUNT) &&
            record[ProductDBTable::PriceBase].toInt() == ProductPriceBase_100g;
}
