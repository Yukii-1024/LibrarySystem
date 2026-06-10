#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QString::fromUtf8("高校图书馆智能管理系统"));
    app.setOrganizationName("DataStructureProject");

    MainWindow w;
    w.show();

    return app.exec();
}
