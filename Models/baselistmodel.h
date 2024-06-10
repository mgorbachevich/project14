#ifndef BASELISTMODEL_H
#define BASELISTMODEL_H

#include <QStringListModel>
#include "constants.h"

class AppManager;

class BaseListModel : public QStringListModel
{
    Q_OBJECT

public:
    enum Roles { ValueRole = Qt::UserRole + 1 };

    explicit BaseListModel(AppManager *parent): QStringListModel((QObject*)parent) { appManager = parent; }
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void update(const QStringList& values) { setStringList(values); }

protected:
    AppManager* appManager;
    DBRecord emptyRecord;
};

#endif // BASELISTMODEL_H
