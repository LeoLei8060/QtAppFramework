#include "QSingleApplication.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QSingleApplication app(argc, argv, "test");

    if (app.isAnotherInstanceRunning()) {
        app.sendMessage("Another instance tried to start.");
        return 0;
    }

    app.initializeStyleSheet(":/res/black.css");
    app.initializeFonts("./font");

    MainWindow w;
    w.show();
    QObject::connect(&app, &QSingleApplication::messageReceived, [](const QString &message) {
        qDebug() << "Received message:" << message;
    });

    return app.exec();
}
