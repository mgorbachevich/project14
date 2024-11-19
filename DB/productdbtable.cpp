#include "productdbtable.h"
#include "tools.h"
#include "database.h"

ProductDBTable::ProductDBTable(const QString& name, QSqlDatabase& sqlDB, DataBase *parent):
    DBTable(name, sqlDB, parent)
{
    Tools::debugLog("@@@@@ ProductDBTable::ProductDBTable");

    addColumn("Код #",                  "code",              "UNSIGNED BIG INT PRIMARY KEY");
    addColumn("Штрих-код",              "barcode",           "TEXT");
    addColumn("Наименование",           "name",              "TEXT");
    addColumn("Краткое наименование",   "name2",             "TEXT");
    addColumn("Наименование*",          "upper_name",        "TEXT");
    addColumn("Номер №",                "code2",             "UNSIGNED BIG INT");
    addColumn("Тип",                    "type",              "INT");
    addColumn("Цена",                   "price",             "INT");
    addColumn("Ценовая база",           "price_base",        "INT");
    addColumn("Цена2",                  "price2",            "INT");
    addColumn("Код схемы скидки",       "discount_code",     "INT");
    addColumn("Код группы",             "group_code",        "UNSIGNED BIG INT");
    addColumn("Масса штуки",            "unit_weight",       "INT");
    addColumn("Код формата этикетки",   "label_format",      "INT");
    addColumn("Код формата этикетки 2", "label_format2",     "INT");
    addColumn("Код формата этикетки 3", "label_format3",     "INT");
    addColumn("Формат ШК",              "barcode_format",    "TEXT");
    addColumn("Масса тары",             "tare",              "REAL");
    addColumn("Сертификат",             "certificate",       "TEXT");
    addColumn("Срок годности",          "shelflife",         "INT");
    addColumn("Дата реализации",        "sell_date",         "DATE");
    addColumn("Произведено",            "produce_date",      "DATE");
    addColumn("Упаковано",              "packing_date",      "DATE");
    addColumn("Код файла картинки",     "picture_code",      "INT");
    addColumn("Код сообщения",          "message_code",      "INT");
    addColumn("Код файла сообщения",    "message_file_code", "INT");
    addColumn("Код файла ролика",       "movie_code",        "INT");
    addColumn("Код звукового файла",    "sound_code",        "INT");
    addColumn("В избранном",            "favorite",          "INT"); // Не используется

    indexDescriptors.append(DBIndexDescriptor(Code2, columnName(Code2) + "_index"));
    indexDescriptors.append(DBIndexDescriptor(Barcode, columnName(Barcode) + "_index"));
    indexDescriptors.append(DBIndexDescriptor(UpperName, columnName(UpperName) + "_index"));
    //indexDescriptors.append(DBIndexDescriptor(Code, columnName(Code) + "_index_9", QString(" WHERE %1 LIKE '%2'").arg(columnName(Code), "9%")));
}

const DBRecord ProductDBTable::checkRecord(const DBRecord& product)
{
    Tools::debugLog("@@@@@ ProductDBTable::checkRecord " + name);
    DBRecord result;
    if (product.count() < columnCount())
    {
        Tools::debugLog("@@@@@ ProductDBTable::checkRecord ERROR");
        return result;
    }
    int code = Tools::toInt(product.at(Columns::Code));
    int groupCode = Tools::toInt(product.at(Columns::GroupCode));
    result.append(product);
    result.replace(Columns::Code, QString("%1").arg(code));
    result.replace(Columns::GroupCode, QString("%1").arg(groupCode));
    result.replace(Columns::UpperName, product.at(Columns::Name).toString().toUpper());
    return result;
}

QVariantList ProductDBTable::makeRootGroup()
{
    DBRecord r = createRecord();
    r[ProductDBTable::Code] = "0";
    r[ProductDBTable::Type] = ProductType_Group;
    r[ProductDBTable::GroupCode] = ROOT_PARENT_CODE;
    r[ProductDBTable::Name] = ALL_GOODS;
    //r[ProductDBTable::PictureCode] = 0;
    return r;
}


