// =====================================================================
// main_window.cpp — реализация главного окна.
// Тут вся "склейка" UI: создание виджетов, расстановка по сетке,
// привязка кликов и клавиатуры к парсеру/вычислителю.
// =====================================================================

#include "ui/main_window.h"
#include "ui/theme.h"        // для stylesheet() — QSS-палитры
#include "ui/format.h"       // operator_to_internal, format_result
#include "parser.h"          // calc::evaluate — рекурсивный спуск

// Qt-зависимости, нужные ТОЛЬКО внутри реализации.
#include <QGridLayout>       // сетка для кнопок
#include <QVBoxLayout>       // вертикальный лейаут (дисплей + кнопки)
#include <QWidget>           // central widget
#include <QKeyEvent>         // в keyPressEvent
#include <QStyle>            // для unpolish/polish при смене QSS-свойства
#include <QSignalMapper>     // мост "кнопка → индекс действия"

namespace ui {

// ---------------------------------------------------------------------
// Конструктор: устанавливаем заголовок, минимальный размер,
// строим UI, применяем тему, задаём стартовый размер.
// ---------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Calculator — Variant 2");   // заголовок окна
    setMinimumSize(380, 560);                    // не даём ужать слишком сильно
    build_ui();                                  // создаём виджеты
    setStyleSheet(theme::stylesheet());          // применяем QSS-тему
    resize(420, 640);                            // стартовый размер при показе
}

// ---------------------------------------------------------------------
// Создание иерархии виджетов:
//   central
//   ├── displayPanel (своя цветная подложка со скруглением)
//   │     ├── historyLabel  (серая строка "2+3 =")
//   │     ├── expressionEdit (строка ввода)
//   │     └── resultLabel    (большой результат)
//   └── grid (сетка кнопок 7×4)
// ---------------------------------------------------------------------
void MainWindow::build_ui() {
    // Центральный виджет — обязателен для QMainWindow.
    auto* central = new QWidget(this);
    central->setObjectName("central");   // имя нужно для QSS-селектора #central
    setCentralWidget(central);

    // Вертикальный лейаут на весь центральный виджет.
    auto* main_layout = new QVBoxLayout(central);
    main_layout->setContentsMargins(20, 20, 20, 20);   // отступы от краёв окна
    main_layout->setSpacing(12);                       // между дисплеем и кнопками

    // Панель дисплея — отдельный QWidget с собственным фоном и скруглением.
    // Без подпанели нельзя было бы покрасить только верхнюю часть окна.
    auto* display_panel = new QWidget(central);
    display_panel->setObjectName("displayPanel");
    auto* display_layout = new QVBoxLayout(display_panel);
    display_layout->setContentsMargins(20, 20, 20, 20);
    display_layout->setSpacing(6);

    // ---- Строка истории сверху ("2+3 =") ----
    history_label_ = new QLabel("", display_panel);
    history_label_->setObjectName("historyLabel");
    history_label_->setAlignment(Qt::AlignRight);   // вправо, как в обычных калькуляторах
    history_label_->setMinimumHeight(20);
    display_layout->addWidget(history_label_);

    // ---- Поле ввода выражения ----
    // QLineEdit, чтобы пользователь мог редактировать строку и руками.
    expression_edit_ = new QLineEdit(display_panel);
    expression_edit_->setObjectName("expressionEdit");
    expression_edit_->setPlaceholderText("0");      // подсказка в пустом поле
    expression_edit_->setAlignment(Qt::AlignRight);
    expression_edit_->setFrame(false);              // убираем стандартную рамку
    display_layout->addWidget(expression_edit_);

    // ---- Большой результат (зелёный/красный) ----
    result_label_ = new QLabel("", display_panel);
    result_label_->setObjectName("resultLabel");
    result_label_->setAlignment(Qt::AlignRight);
    result_label_->setMinimumHeight(48);
    display_layout->addWidget(result_label_);

    // Дисплей готов — добавляем его в основной лейаут.
    main_layout->addWidget(display_panel);

    // ---- Сетка кнопок ----
    auto* grid = new QGridLayout();
    grid->setSpacing(10);                // зазоры между кнопками
    main_layout->addLayout(grid);

    build_buttons(grid);                  // заполняем сетку

    // Растяжки 1:1 — все колонки/строки делят пространство поровну,
    // кнопки получаются одинакового размера при изменении окна.
    for (int c = 0; c < grid_cols; ++c) grid->setColumnStretch(c, 1);
    for (int r = 0; r < grid_rows; ++r) grid->setRowStretch(r, 1);
}

// ---------------------------------------------------------------------
// Создаём кнопки по описанию из button_specs() и подключаем все
// к одному слоту через QSignalMapper. Mapper передаёт индекс кнопки
// в массиве, чтобы handle_action знал, какое действие выполнять.
// ---------------------------------------------------------------------
void MainWindow::build_buttons(QGridLayout* grid) {
    auto* mapper = new QSignalMapper(this);   // владелец — окно, удалится автоматически
    const auto& specs = button_specs();

    for (size_t i = 0; i < specs.size(); ++i) {
        // Создаём кнопку (с правильным цветом из QSS).
        auto* btn = make_button(specs[i]);
        // Размещаем в сетке: addWidget(виджет, row, col, rowspan, colspan).
        grid->addWidget(btn, specs[i].row, specs[i].col, specs[i].row_span, specs[i].col_span);
        // Подключаем сигнал clicked() → mapper::map(). qOverload снимает
        // двусмысленность между map() и map(QObject*) у QSignalMapper.
        connect(btn, &QPushButton::clicked, mapper, qOverload<>(&QSignalMapper::map));
        // Связываем эту кнопку с числом i — потом mapper передаст его в слот.
        mapper->setMapping(btn, static_cast<int>(i));
    }
    // Когда любая кнопка кликнута, mapper кидает mappedInt(i) → handle_action(i).
    connect(mapper, &QSignalMapper::mappedInt, this, &MainWindow::handle_action);
}

// ---------------------------------------------------------------------
// Создаёт QPushButton по спецификации.
// Главное здесь — проставить динамическое свойство "kind", по которому
// QSS подбирает цвета (digit/operator/function/action/equals).
// ---------------------------------------------------------------------
QPushButton* MainWindow::make_button(const ButtonSpec& spec) {
    auto* btn = new QPushButton(spec.label, this);
    btn->setMinimumHeight(56);                    // высота, чтобы было удобно тыкать
    btn->setFocusPolicy(Qt::NoFocus);             // фокус не должен бегать по кнопкам
    btn->setCursor(Qt::PointingHandCursor);       // курсор-палец на наведении

    // Конвертируем enum в строку для QSS-селектора.
    const char* kind_str = "digit";
    switch (spec.kind) {
        case ButtonKind::Digit:    kind_str = "digit";    break;
        case ButtonKind::Operator: kind_str = "operator"; break;
        case ButtonKind::Function: kind_str = "function"; break;
        case ButtonKind::Action:   kind_str = "action";   break;
        case ButtonKind::Equals:   kind_str = "equals";   break;
    }
    // Динамическое свойство — QSS его видит через селектор [kind="digit"].
    btn->setProperty("kind", kind_str);
    return btn;
}

// ---------------------------------------------------------------------
// Диспатчер кликов. По индексу достаём спецификацию кнопки и
// вызываем нужное действие.
// ---------------------------------------------------------------------
void MainWindow::handle_action(int action_index) {
    const auto& spec = button_specs()[static_cast<size_t>(action_index)];
    switch (spec.action) {
        case ButtonAction::Digit:
            append(spec.label);                                   // "5" → +"5"
            break;
        case ButtonAction::Operator:
            // Оператор может быть "красивым" (÷, ×, −) — переводим в ASCII.
            append(format::operator_to_internal(spec.label));
            break;
        case ButtonAction::Function:
            // Удобство: после "ln" сразу подставляем "(", чтобы пользователю
            // не надо было жать ещё одну кнопку.
            append(spec.label + "(");
            break;
        case ButtonAction::Paren:
            append(spec.label);                                   // "(" или ")"
            break;
        case ButtonAction::Dot:
            append(".");
            break;
        case ButtonAction::Clear:
            clear_display();
            break;
        case ButtonAction::Backspace:
            backspace();
            break;
        case ButtonAction::Equals:
            compute();                                            // главный сценарий
            break;
    }
}

// Добавляет текст в конец выражения.
void MainWindow::append(const QString& text) {
    expression_edit_->setText(expression_edit_->text() + text);
}

// Полный сброс: пустые строки везде, результат прячем.
void MainWindow::clear_display() {
    expression_edit_->clear();
    history_label_->setText("");
    show_result("", false);
}

// Удаляем последний символ из строки выражения.
void MainWindow::backspace() {
    QString t = expression_edit_->text();
    if (!t.isEmpty()) expression_edit_->setText(t.left(t.length() - 1));
}

// ---------------------------------------------------------------------
// Главный сценарий: парсим выражение и считаем.
// Все исключения (ParseError, EvalError) ловим одним catch — у них
// общий базовый класс std::exception.
// ---------------------------------------------------------------------
void MainWindow::compute() {
    QString expr = expression_edit_->text().trimmed();   // убираем пробелы по краям
    if (expr.isEmpty()) {
        // Пустая строка — нечего считать. Просто прячем результат.
        show_result("", false);
        return;
    }
    try {
        // toStdString() переводит QString в std::string (UTF-8).
        // calc::evaluate реализован рекурсивным спуском (см. src/parser.cpp).
        double r = calc::evaluate(expr.toStdString());

        // Успех: в истории показываем "2+3 =", в результате — число.
        history_label_->setText(expr + " =");
        show_result(format::format_result(r), false);
    } catch (const std::exception& e) {
        // Любая ошибка (синтаксис или математика) — сюда.
        // В истории — само выражение БЕЗ "=", чтобы пользователь видел,
        // что не посчиталось. В результате — текст ошибки красным.
        history_label_->setText(expr);
        show_result(QString::fromStdString(e.what()), true);
    }
}

// ---------------------------------------------------------------------
// Показать результат и переключить визуальное состояние label'а.
// unpolish/polish нужны, чтобы Qt пересчитал стили после смены
// динамического свойства "error" — иначе цвет не обновится.
// ---------------------------------------------------------------------
void MainWindow::show_result(const QString& text, bool is_error) {
    result_label_->setText(text);
    result_label_->setProperty("error", is_error);   // для QSS-селектора [error="true"]
    result_label_->style()->unpolish(result_label_); // снимаем старые стили
    result_label_->style()->polish(result_label_);   // применяем заново — теперь цвет правильный
}

// ---------------------------------------------------------------------
// Перехват клавиатуры. Qt вызывает этот метод, когда окно в фокусе.
// ---------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    QString text = event->text();

    // Enter, Return или "=" — вычислить.
    if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Equal) { compute();        return; }
    // Backspace — стереть.
    if (key == Qt::Key_Backspace)                                               { backspace();      return; }
    // Escape — полный сброс.
    if (key == Qt::Key_Escape)                                                  { clear_display();  return; }

    // Любой обычный символ: цифра, оператор, скобка, точка, буква
    // (буквы — для функций ln/exp/sqrt/abs).
    if (!text.isEmpty()) {
        QChar c = text.at(0);
        if (c.isDigit() || c == '.' || c == '+' || c == '-' || c == '*' || c == '/'
            || c == '(' || c == ')' || c.isLetter()) {
            append(text);
            return;
        }
    }
    // Всё остальное (стрелки, Tab, F-клавиши и т.п.) — отдаём родителю,
    // чтобы стандартное поведение сохранилось.
    QMainWindow::keyPressEvent(event);
}

}  // namespace ui
