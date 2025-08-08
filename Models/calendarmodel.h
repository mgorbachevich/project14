#ifndef CALENDARMODEL_H
#define CALENDARMODEL_H

#include <QObject>

class BaseListModel;
class QQmlContext;

class CalendarModel : public QObject
{
    Q_OBJECT

public:
    explicit CalendarModel(QQmlContext*, QObject*);

private:
    BaseListModel* calendarDayModel = nullptr;
    BaseListModel* calendarMonthModel = nullptr;
    BaseListModel* calendarYearModel = nullptr;
    BaseListModel* calendarHourModel = nullptr;
    BaseListModel* calendarMinuteModel = nullptr;
    BaseListModel* calendarSecondModel = nullptr;
};

#endif // CALENDARMODEL_H
