#ifndef EXTERNALMESSAGER_H
#define EXTERNALMESSAGER_H

#include <QObject>

class AppManager;

class ExternalMessager : public QObject
{
    Q_OBJECT

public:
    ExternalMessager(AppManager*);
    void showAttention(const QString&);

protected:
    void showMessage(const QString&, const QString&);

    AppManager* appManager;
};

#endif // EXTERNALMESSAGER_H
