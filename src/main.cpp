#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Calculator");
    app.setOrganizationName("M3O-103BV-25");

    MainWindow w;
    w.show();

    return app.exec();
}
