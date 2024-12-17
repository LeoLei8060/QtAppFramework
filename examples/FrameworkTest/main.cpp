#include "mainwindow.h"
#include "core/utils/SingleApplication.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    Core::SingleApplication app(argc, argv, "FrameworkTest");
    
    if (app.isRunning()) {
        QMessageBox::warning(nullptr, "警告", "应用程序已经在运行！");
        app.sendMessage("show");
        return 0;
    }

    MainWindow w;
    w.show();

    QObject::connect(&app, &Core::SingleApplication::messageReceived,
        &w, &MainWindow::onMessageReceived);

    return app.exec();
}
