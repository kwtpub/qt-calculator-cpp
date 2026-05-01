// Точка входа в программу. Создаём Qt-приложение и показываем главное окно.

#include "ui/main_window.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Calculator");
    app.setOrganizationName("M3O-103BV-25");

    ui::MainWindow w;
    w.show();

    // Запуск цикла событий Qt. Возвращает управление, когда окно закрыто.
    return app.exec();
}
