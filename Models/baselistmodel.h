#ifndef BASELISTMODEL_H
#define BASELISTMODEL_H

#include <QStringListModel>

class BaseListModel : public QStringListModel
{
    Q_OBJECT

public:
    enum Roles { ValueRole = Qt::UserRole + 1 };

    explicit BaseListModel(QObject *parent): QStringListModel(parent) {}
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
};

#endif // BASELISTMODEL_H
