#ifndef EXTERNALMESSAGER_H
#define EXTERNALMESSAGER_H

#include <QObject>

class AppManager;

class ExternalMessager : public QObject
{
    Q_OBJECT

public:
    ExternalMessager(AppManager*);

protected:
    void showMessage(const QString&, const QString&);
    void showAttention(const QString&);

    AppManager* appManager;
};

#endif // EXTERNALMESSAGER_H
