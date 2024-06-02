#ifndef BASELISTMODEL3_H
#define BASELISTMODEL3_H

#include <QAbstractListModel>

class AppManager;

class BaseListModel3: public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles
    {
        FirstRole = Qt::UserRole + 1,
        SecondRole = Qt::UserRole + 2,
        ThirdRole = Qt::UserRole + 3,
        FourthRole = Qt::UserRole + 4,
        FifthRole = Qt::UserRole + 5,
    };

    explicit BaseListModel3(AppManager *parent): QAbstractListModel((QObject*)parent) { appManager = parent; }
    int rowCount(const QModelIndex &) const override { return items.count(); }
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

protected:
    void addItem(const QStringList& v) { items << v; }

    QList<QStringList> items;
    QHash<int, QByteArray> roles;
    AppManager* appManager;
};

#endif // BASELISTMODEL3_H
