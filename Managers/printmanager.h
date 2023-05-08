#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <QObject>

class PrintManager : public QObject
{
    Q_OBJECT

public:
    explicit PrintManager(QObject*);

signals:
    void showMessageBox(const QString&, const QString&, const bool);
    void printed();
    void log(const int, const QString&);

public slots:
    void onPrint();
};

#endif // PRINTMANAGER_H
