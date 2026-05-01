#pragma once

// Главное окно калькулятора.
// Класс наследуется от QMainWindow — этого требует Qt (Q_OBJECT + сигналы/слоты).
// Логика тонкая: связывает виджеты с данными из button_layout, делегирует
// вычисление в eval::evaluate, форматирование — в ui::format.

#include "ui/button_layout.h"

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSignalMapper>

namespace ui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    // Перехват клавиатуры: Enter = "=", Esc = "C", Backspace = ⌫,
    // цифры/операторы/буквы — добавляются в строку.
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    // Единый обработчик кликов кнопок. action_index — индекс в button_specs().
    // Слот привязан через QSignalMapper, поэтому отдельные слоты не нужны.
    void handle_action(int action_index);

private:
    QLineEdit* expression_edit_;
    QLabel* result_label_;
    QLabel* history_label_;

    void build_ui();
    void build_buttons(class QGridLayout* grid);
    QPushButton* make_button(const ButtonSpec& spec);

    // Базовые операции над дисплеем, дёргаются из handle_action и keyPressEvent.
    void append(const QString& text);
    void clear_display();
    void backspace();
    void compute();
    void show_result(const QString& text, bool is_error);
};

}
