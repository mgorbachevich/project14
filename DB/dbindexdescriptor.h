#ifndef DBINDEXDESCRIPTOR_H
#define DBINDEXDESCRIPTOR_H

#include <QString>

class DBIndexDescriptor
{
public:
    DBIndexDescriptor(const int indexColumn, const QString& indexName,  const QString& param = "")
    {
        column = indexColumn;
        name = indexName;
        condition = param;
    }

    int column;
    QString name;
    QString condition;
};

#endif // DBINDEXDESCRIPTOR_H
