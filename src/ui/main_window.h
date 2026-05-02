// =====================================================================
// main_window.h — главное окно калькулятора.
//
// Класс наследуется от QMainWindow — этого требует Qt для оконных
// классов с сигналами/слотами (Q_OBJECT, moc-генерация).
//
// Логика тонкая: окно только связывает UI с данными из button_layout,
// делегирует вычисление в eval::evaluate, форматирование — в ui::format.
// Сама математика и парсинг здесь не живут — это правильное разделение
// ответственности.
// =====================================================================

#pragma once

#include "ui/button_layout.h"

// Базовые Qt-типы, нужны прямо в объявлении класса (поля + методы).
#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>

// Forward-declaration: QGridLayout используется только как параметр функции,
// поэтому полный заголовок не нужен — экономим время компиляции.
class QGridLayout;

namespace ui {

class MainWindow : public QMainWindow {
    // Q_OBJECT — макрос Qt, обязателен для классов с сигналами/слотами.
    // moc (Meta-Object Compiler) на этапе сборки сгенерирует по нему
    // вспомогательный .cpp с реализацией метаобъектной системы Qt.
    Q_OBJECT

public:
    // Конструктор. parent=nullptr — окно верхнего уровня, без родителя.
    // explicit запрещает неявные конверсии вида MainWindow w = nullptr;
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    // Перехват клавиатуры. Qt вызывает этот метод, когда окно в фокусе
    // и пользователь нажал клавишу. Карта:
    //   Enter / =  → вычислить (как кнопка =)
    //   Esc        → сброс (как кнопка C)
    //   Backspace  → стереть последний символ (как ⌫)
    //   цифры/операторы/буквы → добавить в строку выражения.
    // override — гарантия от компилятора, что мы переопределяем виртуальный метод.
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    // Единый слот для кликов всех кнопок.
    // action_index — индекс в массиве button_specs(), переданный
    // через QSignalMapper при подключении сигнала clicked().
    // Выгода: один слот вместо 24 отдельных.
    void handle_action(int action_index);

private:
    // Виджеты дисплея. Указатели владеются Qt-системой родительских виджетов,
    // удаляются автоматически при разрушении главного окна — поэтому
    // smart pointer'ы здесь не нужны.
    QLineEdit* expression_edit_;   // строка ввода (можно править руками)
    QLabel*    result_label_;      // большой результат (зелёный/красный)
    QLabel*    history_label_;     // серая строка истории сверху ("2+3 =")

    // Хелперы построения интерфейса. Вынесены в private-методы, чтобы
    // конструктор не разрастался и каждая часть UI имела своё имя.
    void build_ui();
    void build_buttons(QGridLayout* grid);
    QPushButton* make_button(const ButtonSpec& spec);

    // Действия над дисплеем. Дёргаются из handle_action и keyPressEvent.
    void append(const QString& text);     // добавить text в конец выражения
    void clear_display();                  // очистить всё (кнопка C)
    void backspace();                      // стереть последний символ
    void compute();                        // вычислить (кнопка = / Enter)
    void show_result(const QString& text, bool is_error);  // вывести результат
};

}  // namespace ui
