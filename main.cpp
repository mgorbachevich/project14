#include <QGuiApplication>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>
#include "baselistmodel.h"
#include "appmanager.h"

//#define RECOMENDED

int main(int argc, char *argv[])
{
    QGuiApplication application(argc, argv);
    QQmlApplicationEngine engine;

    QFontDatabase::addApplicationFont(":/Resources/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/Resources/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/Resources/Roboto-Italic.ttf");
    QFontDatabase::addApplicationFont(":/Resources/LeagueGothic-Regular.otf");
    //qDebug() <<  "@@@@@ main font families: " << QFontDatabase::families();

    QSize screenSize = QSize(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
#ifdef Q_OS_ANDROID
    QRect r = application.primaryScreen()->geometry();
    screenSize = QSize(r.width() > r.height() ? r.width() : r.height(),
                       r.width() <= r.height() ? r.width() : r.height());
#endif
    AppManager* appManager = new AppManager(engine.rootContext(), &application, screenSize);
    qmlRegisterUncreatableType<BaseListModel>("RegisteredTypes", 1, 0, "BaseListModel", "");
    engine.rootContext()->setContextProperty("app", appManager);

#ifdef RECOMENDED
    const QUrl url(u"qrc:/Project14/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &application, [url](QObject *obj, const QUrl &objUrl)
    {
        if (!obj && url == objUrl) QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
#else
    const QUrl url("qrc:/main.qml");
#endif

    engine.load(url);
    return application.exec();
}

