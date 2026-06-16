#include <QApplication>
#include <QIcon>
#include "ui/MainWindow.h"
#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    // Hide the console window when double-clicked
    FreeConsole();
#endif
    QApplication app(argc, argv);

    // Tell Qt where to find SQL plugins (relative to exe)
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/sqldrivers");
    app.setApplicationName(QString::fromUtf8("高校图书馆智能管理系统"));
    app.setOrganizationName("DataStructureProject");

    // Set application icon (icon.png beside the executable)
    QString iconPath = QCoreApplication::applicationDirPath() + "/icon.png";
    app.setWindowIcon(QIcon(iconPath));

    MainWindow w;
    if (w.isLoginSuccess()) {
        w.show();
        return app.exec();
    }
    return 0;
}
