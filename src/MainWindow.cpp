#include "MainWindow.h"
#include "Evaluator.h"

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

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(12);

    auto* displayPanel = new QWidget(central);
    displayPanel->setObjectName("displayPanel");
    auto* displayLayout = new QVBoxLayout(displayPanel);
    displayLayout->setContentsMargins(20, 20, 20, 20);
    displayLayout->setSpacing(6);

    historyLabel_ = new QLabel("", displayPanel);
    historyLabel_->setObjectName("historyLabel");
    historyLabel_->setAlignment(Qt::AlignRight);
    historyLabel_->setMinimumHeight(20);
    displayLayout->addWidget(historyLabel_);

    expressionEdit_ = new QLineEdit(displayPanel);
    expressionEdit_->setObjectName("expressionEdit");
    expressionEdit_->setPlaceholderText("0");
    expressionEdit_->setAlignment(Qt::AlignRight);
    expressionEdit_->setFrame(false);
    displayLayout->addWidget(expressionEdit_);

    resultLabel_ = new QLabel("", displayPanel);
    resultLabel_->setObjectName("resultLabel");
    resultLabel_->setAlignment(Qt::AlignRight);
    resultLabel_->setMinimumHeight(48);
    displayLayout->addWidget(resultLabel_);

    mainLayout->addWidget(displayPanel);

    auto* grid = new QGridLayout();
    grid->setSpacing(10);
    mainLayout->addLayout(grid);

    using K = ButtonKind;
    struct B { QString label; int row; int col; int rowSpan; int colSpan; K kind; const char* slot; };
    QList<B> buttons = {
        {"C",     0, 0, 1, 1, K::Action,   SLOT(onClearClicked())},
        {"⌫",    0, 1, 1, 1, K::Action,   SLOT(onBackspaceClicked())},
        {"(",     0, 2, 1, 1, K::Action,   SLOT(onParenClicked())},
        {")",     0, 3, 1, 1, K::Action,   SLOT(onParenClicked())},

        {"ln",    1, 0, 1, 1, K::Function, SLOT(onFunctionClicked())},
        {"exp",   1, 1, 1, 1, K::Function, SLOT(onFunctionClicked())},
        {"sqrt",  1, 2, 1, 1, K::Function, SLOT(onFunctionClicked())},
        {"abs",   1, 3, 1, 1, K::Function, SLOT(onFunctionClicked())},

        {"7",     2, 0, 1, 1, K::Digit,    SLOT(onDigitClicked())},
        {"8",     2, 1, 1, 1, K::Digit,    SLOT(onDigitClicked())},
        {"9",     2, 2, 1, 1, K::Digit,    SLOT(onDigitClicked())},
        {"÷",     2, 3, 1, 1, K::Operator, SLOT(onOperatorClicked())},

        {"4",     3, 0, 1, 1, K::Digit,    SLOT(onDigitClicked())},
        {"5",     3, 1, 1, 1, K::Digit,    SLOT(onDigitClicked())},
        {"6",     3, 2, 1, 1, K::Digit,    SLOT(onDigitClicked())},
        {"×",     3, 3, 1, 1, K::Operator, SLOT(onOperatorClicked())},

        {"1",     4, 0, 1, 1, K::Digit,    SLOT(onDigitClicked())},
        {"2",     4, 1, 1, 1, K::Digit,    SLOT(onDigitClicked())},
        {"3",     4, 2, 1, 1, K::Digit,    SLOT(onDigitClicked())},
        {"−",     4, 3, 1, 1, K::Operator, SLOT(onOperatorClicked())},

        {"0",     5, 0, 1, 2, K::Digit,    SLOT(onDigitClicked())},
        {".",     5, 2, 1, 1, K::Digit,    SLOT(onDotClicked())},
        {"+",     5, 3, 1, 1, K::Operator, SLOT(onOperatorClicked())},

        {"=",     6, 0, 1, 4, K::Equals,   SLOT(onEqualsClicked())},
    };

    for (const B& b : buttons) {
        auto* btn = makeButton(b.label, b.kind);
        grid->addWidget(btn, b.row, b.col, b.rowSpan, b.colSpan);
        connect(btn, SIGNAL(clicked()), this, b.slot);
    }

    for (int c = 0; c < 4; ++c) grid->setColumnStretch(c, 1);
    for (int r = 0; r < 7; ++r) grid->setRowStretch(r, 1);

    applyTheme();
    resize(420, 640);
}

QPushButton* MainWindow::makeButton(const QString& text, ButtonKind kind) {
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

void MainWindow::applyTheme() {
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

void MainWindow::appendText(const QString& text) {
    expressionEdit_->setText(expressionEdit_->text() + text);
}

void MainWindow::onDigitClicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (btn) appendText(btn->text());
}

void MainWindow::onOperatorClicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    QString t = btn->text();
    QString internal = t;
    if (t == "÷") internal = "/";
    else if (t == "×") internal = "*";
    else if (t == "−") internal = "-";
    appendText(internal);
}

void MainWindow::onFunctionClicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (btn) appendText(btn->text() + "(");
}

void MainWindow::onParenClicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (btn) appendText(btn->text());
}

void MainWindow::onDotClicked() {
    appendText(".");
}

void MainWindow::onClearClicked() {
    expressionEdit_->clear();
    resultLabel_->setText("");
    historyLabel_->setText("");
    resultLabel_->setProperty("error", false);
    resultLabel_->style()->unpolish(resultLabel_);
    resultLabel_->style()->polish(resultLabel_);
}

void MainWindow::onBackspaceClicked() {
    QString t = expressionEdit_->text();
    if (!t.isEmpty()) expressionEdit_->setText(t.left(t.length() - 1));
}

void MainWindow::onEqualsClicked() {
    compute();
}

void MainWindow::compute() {
    QString expr = expressionEdit_->text().trimmed();
    if (expr.isEmpty()) {
        resultLabel_->setText("");
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
        historyLabel_->setText(expr + " =");
        resultLabel_->setText(out);
        resultLabel_->setProperty("error", false);
    } catch (const std::exception& e) {
        historyLabel_->setText(expr);
        resultLabel_->setText(QString::fromStdString(e.what()));
        resultLabel_->setProperty("error", true);
    }
    resultLabel_->style()->unpolish(resultLabel_);
    resultLabel_->style()->polish(resultLabel_);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    QString text = event->text();
    if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Equal) {
        compute();
        return;
    }
    if (key == Qt::Key_Backspace) {
        onBackspaceClicked();
        return;
    }
    if (key == Qt::Key_Escape) {
        onClearClicked();
        return;
    }
    if (!text.isEmpty()) {
        QChar c = text.at(0);
        if (c.isDigit() || c == '.' || c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
            appendText(text);
            return;
        }
        if (c.isLetter()) {
            appendText(text);
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}
