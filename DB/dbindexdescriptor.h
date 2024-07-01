#ifndef DBINDEXDESCRIPTOR_H
#define DBINDEXDESCRIPTOR_H

#include <QString>

class DBIndexDescriptor
{
public:
    DBIndexDescriptor(const int indexColumn, const QString& indexName,  const QString& indexParam = "")
    {
        column = indexColumn;
        name = indexName;
        param = indexParam;
    }

    int column;
    QString name;
    QString param;
};

#endif // DBINDEXDESCRIPTOR_H
