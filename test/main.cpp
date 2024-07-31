#include "QSingleApplication.h"
#include <QWidget>

int main(int argc, char *argv[])
{
    QSingleApplication app(argc, argv, "test");
    if (app.isAnotherInstanceRunning()) {
        app.sendMessage("Another instance tried to start.");
        return 0;
    }

    QObject::connect(&app, &QSingleApplication::messageReceived, [](const QString &message) {
        qDebug() << "Received message:" << message;
    });
    QWidget w;
    w.show();

    return app.exec();
}
