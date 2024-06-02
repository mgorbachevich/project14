#ifndef BASELISTMODEL2_H
#define BASELISTMODEL2_H

#include <QAbstractListModel>
#include "constants.h"

class AppManager;

class BaseListModel2: public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles { FirstRole = Qt::UserRole + 1, SecondRole = Qt::UserRole + 2 };

    explicit BaseListModel2(AppManager *parent): QAbstractListModel((QObject*)parent) { appManager = parent; }
    int rowCount(const QModelIndex &) const override { return items.count(); }
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

protected:
    void addItem(const QString& name, const QString& value) { StringPair v(name, value); items << v; }

    QList<StringPair> items;
    QHash<int, QByteArray> roles;
    AppManager* appManager;
};

#endif // BASELISTMODEL2_H
