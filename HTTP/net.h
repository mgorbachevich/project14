#ifndef NET_H
#define NET_H

#include <QObject>
#include <QThread>

class Net : public QObject
{
    Q_OBJECT
public:
    explicit Net(QObject *parent = nullptr);
    ~Net();

private:
    QThread backThread;

signals:
    void showMessageBox(const QString&, const QString&);
    void newData(const QString&);

public slots:
     void onNewData(const QString& json) { emit newData(json); }
};

#endif // NET_H
