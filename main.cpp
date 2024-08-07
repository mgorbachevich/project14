#include <QApplication>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>
#include <QSplashScreen>
#include "baselistmodel.h"
#include "appmanager.h"
#include "screenmanager.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QQmlApplicationEngine engine;

    QSize screenSize;
#ifdef Q_OS_ANDROID
    QRect r = application.primaryScreen()->geometry();
    screenSize = QSize(r.width() > r.height() ? r.width() : r.height(),
                       r.width() <= r.height() ? r.width() : r.height());
#else
    screenSize = QSize(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
#endif

    // GUI fonts:
    //QFontDatabase::addApplicationFont(":/Fonts/LeagueGothic-CondensedRegular.otf");
    QFontDatabase::addApplicationFont(":/Fonts/LeagueGothic-Regular.otf");
    QFontDatabase::addApplicationFont(":/Fonts/Roboto-Bold.ttf");
    //QFontDatabase::addApplicationFont(":/Fonts/Roboto-Condensed.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/Roboto-Italic.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/Roboto-Regular.ttf");
    // Label fonts:
    QFontDatabase::addApplicationFont(":/Fonts/arial.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/arialbd.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/arialbi.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/ariali.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/arialn.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/arialnb.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/arialnbi.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/arialni.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/ariblk.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/impact.ttf");
    QFontDatabase::addApplicationFont(":/Fonts/sserifer.fon");
    QFontDatabase::addApplicationFont(":/Fonts/times.ttf");

    AppManager* appManager = new AppManager(engine.rootContext(), screenSize, &application);
    qmlRegisterUncreatableType<BaseListModel>("RegisteredTypes", 1, 0, "BaseListModel", "");
    engine.rootContext()->setContextProperty("app", appManager);
    const QUrl url("qrc:/main.qml");
    engine.load(url);

    return application.exec();
}

