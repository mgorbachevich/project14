#ifndef DBTABLECOLUMN_H
#define DBTABLECOLUMN_H

#include <QString>

class DBTableColumn
{
public:
    explicit DBTableColumn(const QString& title, const QString& name, const QString& type) :
        title(title), name(name), type(type) {}

    QString title;
    QString name;
    QString type;
};

#endif // DBTABLECOLUMN_H
