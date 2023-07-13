#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "SpeedReceiver.h"
#include "ButtonsReceiver.h"
#include "RPMReceiver.h"
#include <qqml.h>
#include <QMetaType>
#include <string>
#include <iostream>
#include <thread>
#include <CommonAPI/CommonAPI.hpp>
#include "ClusterStubImpl.hpp"
#include "WeatherAPI.h"

// #include <QWebEngineSettings>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QWebEngineView>
#include <QWebChannel>

using namespace std;
Q_DECLARE_METATYPE(std::string)

int main(int argc, char *argv[])
{
    qRegisterMetaType<std::string>();

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    std::shared_ptr<ClusterStubImpl> myService = std::make_shared<ClusterStubImpl>();
    runtime->registerService("local", "cluster_service", myService);
    std::cout << "Successfully Registered Service!" << std::endl;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    SpeedReceiver speedStorage;
    ButtonsReceiver buttonStorage;
    RPMReceiver rpmStorage;
    WeatherAPI weatherAPIStorage;

    engine.rootContext()->setContextProperty("speedReceiver", &speedStorage);
    engine.rootContext()->setContextProperty("buttonsReceiver", &buttonStorage);
    engine.rootContext()->setContextProperty("rpmReceiver", &rpmStorage);
    engine.rootContext()->setContextProperty("weatherAPI", &weatherAPIStorage);

    QObject::connect(&(*myService), &ClusterStubImpl::signalSpeed, &speedStorage, &SpeedReceiver::receiveSpeed);
    QObject::connect(&(*myService), &ClusterStubImpl::signalButtons, &buttonStorage, &ButtonsReceiver::receiveButtons);
    QObject::connect(&(*myService), &ClusterStubImpl::signalRPM, &rpmStorage, &RPMReceiver::receiveRPM);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QWebEngineView view;
    QWebChannel channel;
    view.page()->setWebChannel(&channel);
    view.load(QUrl::fromLocalFile("/home/sea/sea-me-hackathon-2023/Cluster/src/map.html"));

    // Check fuel condition and load the map if fuel is below 20%
    int fuelLevel = 25; // Example fuel level, you can replace it with your actual fuel level
    if (fuelLevel < 20) {
        view.show();
    }

    // QWebEngineView view;
    // QWebChannel channel;
    // view.page()->setWebChannel(&channel);
    // view.load(QUrl::fromLocalFile("/home/sea/sea-me-hackathon-2023/Cluster/src/map.html"));
    // view.show();
    

    return app.exec();
}
