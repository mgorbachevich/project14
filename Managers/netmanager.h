#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>
#include <QThread>

class NetManager : public QObject
{
    Q_OBJECT
public:
    explicit NetManager(QObject *parent = nullptr);
    ~NetManager();

private:
    QThread netThread;

signals:
    void showMessageBox(const QString&, const QString&);
    void newData(const QString&);

public slots:
     void onNewData(const QString&);
};

#endif // NETMANAGER_H
