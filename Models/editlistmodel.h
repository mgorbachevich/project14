#ifndef EDITLISTMODEL_H
#define EDITLISTMODEL_H

#include <QPair>
#include <QAbstractListModel>

#define StringPair QPair<QString, QString>

class EditListModel: public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles { NameRole = Qt::UserRole + 1, ValueRole = Qt::UserRole + 2 };

    explicit EditListModel(QObject *parent = nullptr): QAbstractListModel(parent) {}
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &) const override { return items.count(); }
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void addItem(const QString& name, const QString& value) { StringPair v(name, value); items << v; }

private:
    QList<StringPair> items;
    QHash<int, QByteArray> roles;
};

#endif // EDITLISTMODEL_H
