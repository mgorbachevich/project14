#include <QGuiApplication>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "baselistmodel.h"
#include "weightmanager.h"
#include "appmanager.h"
#include "printmanager.h"

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

    AppManager* appManager = new AppManager(&application, engine.rootContext());
    WeightManager* weightManager = new WeightManager(&application);
    PrintManager* printManager = new PrintManager(&application);

    qmlRegisterUncreatableType<BaseListModel>("RegisteredTypes", 1, 0, "BaseListModel", "");
    engine.rootContext()->setContextProperty("app", appManager);

    QObject::connect(appManager, &AppManager::resetProduct, appManager, &AppManager::onResetProduct);
    QObject::connect(appManager, &AppManager::print, printManager, &PrintManager::onPrint);
    QObject::connect(appManager, &AppManager::setWeightParam, weightManager, &WeightManager::onSetWeightParam);

    QObject::connect(weightManager, &WeightManager::weightChanged, appManager, &AppManager::onWeightChanged);
    QObject::connect(weightManager, &WeightManager::weightParamChanged, appManager, &AppManager::onWeightParamChanged);
    QObject::connect(weightManager, &WeightManager::showMessageBox, appManager, &AppManager::onShowMessageBox);
    QObject::connect(weightManager, &WeightManager::log, appManager, &AppManager::onLog);

    QObject::connect(printManager, &PrintManager::showMessageBox, appManager, &AppManager::onShowMessageBox);
    QObject::connect(printManager, &PrintManager::printed, appManager, &AppManager::onPrinted);
    QObject::connect(printManager, &PrintManager::log, appManager, &AppManager::onLog);

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

