#include "searchfiltermodel.h"

void SearchFilterModel::update()
{
    qDebug() << "@@@@@ SearchFilterModel::update";
    QStringList ss;
    ss << "Код" << "Штрихкод";
    setStringList(ss);
}
