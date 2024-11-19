#include <QQmlContext>
#include "calendarmodel.h"
#include "appmanager.h"
#include "baselistmodel.h"

CalendarModel::CalendarModel(QQmlContext* context, AppManager* parent) : QObject{parent}
{
    Tools::debugLog("@@@@@ CalendarModel::CalendarModel");

    calendarDayModel    = new BaseListModel(parent);
    calendarMonthModel  = new BaseListModel(parent);
    calendarYearModel   = new BaseListModel(parent);
    calendarHourModel   = new BaseListModel(parent);
    calendarMinuteModel = new BaseListModel(parent);
    calendarSecondModel = new BaseListModel(parent);

    context->setContextProperty("calendarDayModel",    calendarDayModel);
    context->setContextProperty("calendarMonthModel",  calendarMonthModel);
    context->setContextProperty("calendarYearModel",   calendarYearModel);
    context->setContextProperty("calendarHourModel",   calendarHourModel);
    context->setContextProperty("calendarMinuteModel", calendarMinuteModel);
    context->setContextProperty("calendarSecondModel", calendarSecondModel);

    QStringList days, months, years, hours, minutes, seconds;
    for (int i = 1; i <= 31; i++) days    << Tools::toString(i);
    for (int i = 1; i <= 12; i++) months  << Tools::toString(i);
    for (int i = 1; i <= 25; i++) years   << Tools::toString(i + 2023);
    for (int i = 0; i <= 23; i++) hours   << Tools::toString(i);
    for (int i = 0; i <= 59; i++) minutes << Tools::toString(i);
    for (int i = 0; i <= 59; i++) seconds << Tools::toString(i);

    calendarDayModel   ->setStringList(days);
    calendarMonthModel ->setStringList(months);
    calendarYearModel  ->setStringList(years);
    calendarHourModel  ->setStringList(hours);
    calendarMinuteModel->setStringList(minutes);
    calendarSecondModel->setStringList(seconds);
}
