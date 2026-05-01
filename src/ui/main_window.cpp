#include "ui/main_window.h"
#include "ui/theme.h"
#include "ui/format.h"
#include "eval/evaluator.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QStyle>
#include <QSignalMapper>

namespace ui {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Calculator — Variant 2");
    setMinimumSize(380, 560);
    build_ui();
    setStyleSheet(theme::stylesheet());
    resize(420, 640);
}

void MainWindow::build_ui() {
    auto* central = new QWidget(this);
    central->setObjectName("central");
    setCentralWidget(central);

    auto* main_layout = new QVBoxLayout(central);
    main_layout->setContentsMargins(20, 20, 20, 20);
    main_layout->setSpacing(12);

    auto* display_panel = new QWidget(central);
    display_panel->setObjectName("displayPanel");
    auto* display_layout = new QVBoxLayout(display_panel);
    display_layout->setContentsMargins(20, 20, 20, 20);
    display_layout->setSpacing(6);

    history_label_ = new QLabel("", display_panel);
    history_label_->setObjectName("historyLabel");
    history_label_->setAlignment(Qt::AlignRight);
    history_label_->setMinimumHeight(20);
    display_layout->addWidget(history_label_);

    expression_edit_ = new QLineEdit(display_panel);
    expression_edit_->setObjectName("expressionEdit");
    expression_edit_->setPlaceholderText("0");
    expression_edit_->setAlignment(Qt::AlignRight);
    expression_edit_->setFrame(false);
    display_layout->addWidget(expression_edit_);

    result_label_ = new QLabel("", display_panel);
    result_label_->setObjectName("resultLabel");
    result_label_->setAlignment(Qt::AlignRight);
    result_label_->setMinimumHeight(48);
    display_layout->addWidget(result_label_);

    main_layout->addWidget(display_panel);

    auto* grid = new QGridLayout();
    grid->setSpacing(10);
    main_layout->addLayout(grid);

    build_buttons(grid);

    for (int c = 0; c < grid_cols; ++c) grid->setColumnStretch(c, 1);
    for (int r = 0; r < grid_rows; ++r) grid->setRowStretch(r, 1);
}

void MainWindow::build_buttons(QGridLayout* grid) {
    auto* mapper = new QSignalMapper(this);
    const auto& specs = button_specs();
    for (size_t i = 0; i < specs.size(); ++i) {
        auto* btn = make_button(specs[i]);
        grid->addWidget(btn, specs[i].row, specs[i].col, specs[i].row_span, specs[i].col_span);
        connect(btn, &QPushButton::clicked, mapper, qOverload<>(&QSignalMapper::map));
        mapper->setMapping(btn, static_cast<int>(i));
    }
    connect(mapper, &QSignalMapper::mappedInt, this, &MainWindow::handle_action);
}

QPushButton* MainWindow::make_button(const ButtonSpec& spec) {
    auto* btn = new QPushButton(spec.label, this);
    btn->setMinimumHeight(56);
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setCursor(Qt::PointingHandCursor);
    const char* kind_str = "digit";
    switch (spec.kind) {
        case ButtonKind::Digit:    kind_str = "digit";    break;
        case ButtonKind::Operator: kind_str = "operator"; break;
        case ButtonKind::Function: kind_str = "function"; break;
        case ButtonKind::Action:   kind_str = "action";   break;
        case ButtonKind::Equals:   kind_str = "equals";   break;
    }
    btn->setProperty("kind", kind_str);
    return btn;
}

void MainWindow::handle_action(int action_index) {
    const auto& spec = button_specs()[static_cast<size_t>(action_index)];
    switch (spec.action) {
        case ButtonAction::Digit:     append(spec.label);                              break;
        case ButtonAction::Operator:  append(format::operator_to_internal(spec.label)); break;
        case ButtonAction::Function:  append(spec.label + "(");                        break;
        case ButtonAction::Paren:     append(spec.label);                              break;
        case ButtonAction::Dot:       append(".");                                     break;
        case ButtonAction::Clear:     clear_display();                                 break;
        case ButtonAction::Backspace: backspace();                                     break;
        case ButtonAction::Equals:    compute();                                       break;
    }
}

void MainWindow::append(const QString& text) {
    expression_edit_->setText(expression_edit_->text() + text);
}

void MainWindow::clear_display() {
    expression_edit_->clear();
    history_label_->setText("");
    show_result("", false);
}

void MainWindow::backspace() {
    QString t = expression_edit_->text();
    if (!t.isEmpty()) expression_edit_->setText(t.left(t.length() - 1));
}

void MainWindow::compute() {
    QString expr = expression_edit_->text().trimmed();
    if (expr.isEmpty()) {
        show_result("", false);
        return;
    }
    try {
        double r = eval::evaluate(expr.toStdString());
        history_label_->setText(expr + " =");
        show_result(format::format_result(r), false);
    } catch (const std::exception& e) {
        history_label_->setText(expr);
        show_result(QString::fromStdString(e.what()), true);
    }
}

void MainWindow::show_result(const QString& text, bool is_error) {
    result_label_->setText(text);
    result_label_->setProperty("error", is_error);
    result_label_->style()->unpolish(result_label_);
    result_label_->style()->polish(result_label_);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    QString text = event->text();
    if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Equal) { compute();        return; }
    if (key == Qt::Key_Backspace)                                               { backspace();      return; }
    if (key == Qt::Key_Escape)                                                  { clear_display();  return; }
    if (!text.isEmpty()) {
        QChar c = text.at(0);
        if (c.isDigit() || c == '.' || c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')' || c.isLetter()) {
            append(text);
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}

}
