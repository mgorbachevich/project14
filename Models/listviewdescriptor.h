#ifndef LISTVIEWDESCRIPTOR_H
#define LISTVIEWDESCRIPTOR_H

#include <QString>

class ListViewDescriptor
{
public:
    explicit ListViewDescriptor() {}
    void reset(const QString& v) { firstVisibleRow = firstLoadRow = loadRowCout = 0; param = v; }

    int firstVisibleRow = 0;
    int firstLoadRow = 0;
    int visibleRowCount = 20;
    int loadRowCout = 0;
    QString param;
};

#endif // LISTVIEWDESCRIPTOR_H
