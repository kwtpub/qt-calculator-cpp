#include "main_window.h"
#include "evaluator.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QFont>
#include <QFontDatabase>
#include <QStyle>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Calculator — Variant 2");
    setMinimumSize(380, 560);

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

    using K = ButtonKind;
    struct B { QString label; int row; int col; int row_span; int col_span; K kind; const char* slot; };
    QList<B> buttons = {
        {"C",     0, 0, 1, 1, K::Action,   SLOT(on_clear_clicked())},
        {"⌫",    0, 1, 1, 1, K::Action,   SLOT(on_backspace_clicked())},
        {"(",     0, 2, 1, 1, K::Action,   SLOT(on_paren_clicked())},
        {")",     0, 3, 1, 1, K::Action,   SLOT(on_paren_clicked())},

        {"ln",    1, 0, 1, 1, K::Function, SLOT(on_function_clicked())},
        {"exp",   1, 1, 1, 1, K::Function, SLOT(on_function_clicked())},
        {"sqrt",  1, 2, 1, 1, K::Function, SLOT(on_function_clicked())},
        {"abs",   1, 3, 1, 1, K::Function, SLOT(on_function_clicked())},

        {"7",     2, 0, 1, 1, K::Digit,    SLOT(on_digit_clicked())},
        {"8",     2, 1, 1, 1, K::Digit,    SLOT(on_digit_clicked())},
        {"9",     2, 2, 1, 1, K::Digit,    SLOT(on_digit_clicked())},
        {"÷",     2, 3, 1, 1, K::Operator, SLOT(on_operator_clicked())},

        {"4",     3, 0, 1, 1, K::Digit,    SLOT(on_digit_clicked())},
        {"5",     3, 1, 1, 1, K::Digit,    SLOT(on_digit_clicked())},
        {"6",     3, 2, 1, 1, K::Digit,    SLOT(on_digit_clicked())},
        {"×",     3, 3, 1, 1, K::Operator, SLOT(on_operator_clicked())},

        {"1",     4, 0, 1, 1, K::Digit,    SLOT(on_digit_clicked())},
        {"2",     4, 1, 1, 1, K::Digit,    SLOT(on_digit_clicked())},
        {"3",     4, 2, 1, 1, K::Digit,    SLOT(on_digit_clicked())},
        {"−",     4, 3, 1, 1, K::Operator, SLOT(on_operator_clicked())},

        {"0",     5, 0, 1, 2, K::Digit,    SLOT(on_digit_clicked())},
        {".",     5, 2, 1, 1, K::Digit,    SLOT(on_dot_clicked())},
        {"+",     5, 3, 1, 1, K::Operator, SLOT(on_operator_clicked())},

        {"=",     6, 0, 1, 4, K::Equals,   SLOT(on_equals_clicked())},
    };

    for (const B& b : buttons) {
        auto* btn = make_button(b.label, b.kind);
        grid->addWidget(btn, b.row, b.col, b.row_span, b.col_span);
        connect(btn, SIGNAL(clicked()), this, b.slot);
    }

    for (int c = 0; c < 4; ++c) grid->setColumnStretch(c, 1);
    for (int r = 0; r < 7; ++r) grid->setRowStretch(r, 1);

    apply_theme();
    resize(420, 640);
}

QPushButton* MainWindow::make_button(const QString& text, ButtonKind kind) {
    auto* btn = new QPushButton(text, this);
    btn->setMinimumHeight(56);
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setCursor(Qt::PointingHandCursor);
    switch (kind) {
        case ButtonKind::Digit:    btn->setProperty("kind", "digit"); break;
        case ButtonKind::Operator: btn->setProperty("kind", "operator"); break;
        case ButtonKind::Function: btn->setProperty("kind", "function"); break;
        case ButtonKind::Action:   btn->setProperty("kind", "action"); break;
        case ButtonKind::Equals:   btn->setProperty("kind", "equals"); break;
    }
    return btn;
}

void MainWindow::apply_theme() {
    const QString qss = R"(
        QWidget#central {
            background-color: #1e1e2e;
        }

        QWidget#displayPanel {
            background-color: #181825;
            border-radius: 16px;
        }

        QLabel#historyLabel {
            color: #6c7086;
            font-size: 14px;
            background: transparent;
        }

        QLineEdit#expressionEdit {
            color: #cdd6f4;
            background: transparent;
            font-size: 28px;
            font-weight: 500;
            border: none;
            padding: 0;
            selection-background-color: #45475a;
        }

        QLabel#resultLabel {
            color: #a6e3a1;
            font-size: 36px;
            font-weight: 700;
            background: transparent;
        }

        QLabel#resultLabel[error="true"] {
            color: #f38ba8;
            font-size: 18px;
            font-weight: 500;
        }

        QPushButton {
            border: none;
            border-radius: 14px;
            font-size: 20px;
            font-weight: 600;
            padding: 0;
        }

        QPushButton[kind="digit"] {
            background-color: #313244;
            color: #cdd6f4;
        }
        QPushButton[kind="digit"]:hover {
            background-color: #45475a;
        }
        QPushButton[kind="digit"]:pressed {
            background-color: #585b70;
        }

        QPushButton[kind="operator"] {
            background-color: #f9e2af;
            color: #1e1e2e;
            font-size: 24px;
        }
        QPushButton[kind="operator"]:hover {
            background-color: #fae3b0;
        }
        QPushButton[kind="operator"]:pressed {
            background-color: #f2cdcd;
        }

        QPushButton[kind="function"] {
            background-color: #89b4fa;
            color: #1e1e2e;
            font-size: 16px;
        }
        QPushButton[kind="function"]:hover {
            background-color: #b4befe;
        }
        QPushButton[kind="function"]:pressed {
            background-color: #74c7ec;
        }

        QPushButton[kind="action"] {
            background-color: #585b70;
            color: #f5c2e7;
            font-size: 18px;
        }
        QPushButton[kind="action"]:hover {
            background-color: #6c7086;
        }
        QPushButton[kind="action"]:pressed {
            background-color: #7f849c;
        }

        QPushButton[kind="equals"] {
            background-color: #a6e3a1;
            color: #1e1e2e;
            font-size: 22px;
        }
        QPushButton[kind="equals"]:hover {
            background-color: #94e2d5;
        }
        QPushButton[kind="equals"]:pressed {
            background-color: #74c7ec;
        }
    )";
    setStyleSheet(qss);
}

void MainWindow::append_text(const QString& text) {
    expression_edit_->setText(expression_edit_->text() + text);
}

void MainWindow::on_digit_clicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (btn) append_text(btn->text());
}

void MainWindow::on_operator_clicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QString t = btn->text();
    QString internal = t;
    if (t == "÷") internal = "/";
    else if (t == "×") internal = "*";
    else if (t == "−") internal = "-";
    append_text(internal);
}

void MainWindow::on_function_clicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (btn) append_text(btn->text() + "(");
}

void MainWindow::on_paren_clicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (btn) append_text(btn->text());
}

void MainWindow::on_dot_clicked() {
    append_text(".");
}

void MainWindow::on_clear_clicked() {
    expression_edit_->clear();
    result_label_->setText("");
    history_label_->setText("");
    result_label_->setProperty("error", false);
    result_label_->style()->unpolish(result_label_);
    result_label_->style()->polish(result_label_);
}

void MainWindow::on_backspace_clicked() {
    QString t = expression_edit_->text();
    if (!t.isEmpty()) expression_edit_->setText(t.left(t.length() - 1));
}

void MainWindow::on_equals_clicked() {
    compute();
}

void MainWindow::compute() {
    QString expr = expression_edit_->text().trimmed();
    if (expr.isEmpty()) {
        result_label_->setText("");
        return;
    }
    try {
        Evaluator ev;
        double r = ev.evaluate(expr.toStdString());
        QString out;
        if (r == static_cast<long long>(r) && std::abs(r) < 1e15) {
            out = QString::number(static_cast<long long>(r));
        } else {
            out = QString::number(r, 'g', 12);
        }
        history_label_->setText(expr + " =");
        result_label_->setText(out);
        result_label_->setProperty("error", false);
    } catch (const std::exception& e) {
        history_label_->setText(expr);
        result_label_->setText(QString::fromStdString(e.what()));
        result_label_->setProperty("error", true);
    }
    result_label_->style()->unpolish(result_label_);
    result_label_->style()->polish(result_label_);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    QString text = event->text();
    if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Equal) {
        compute();
        return;
    }
    if (key == Qt::Key_Backspace) {
        on_backspace_clicked();
        return;
    }
    if (key == Qt::Key_Escape) {
        on_clear_clicked();
        return;
    }
    if (!text.isEmpty()) {
        QChar c = text.at(0);
        if (c.isDigit() || c == '.' || c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
            append_text(text);
            return;
        }
        if (c.isLetter()) {
            append_text(text);
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}
