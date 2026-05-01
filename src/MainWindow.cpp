#include "MainWindow.h"
#include "Evaluator.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QKeyEvent>
#include <QFont>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Калькулятор — Вариант 2 (ln, exp, sqrt, abs)");

    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);

    expressionEdit_ = new QLineEdit(this);
    expressionEdit_->setPlaceholderText("Введите выражение");
    QFont exprFont = expressionEdit_->font();
    exprFont.setPointSize(18);
    expressionEdit_->setFont(exprFont);
    expressionEdit_->setAlignment(Qt::AlignRight);
    mainLayout->addWidget(expressionEdit_);

    resultLabel_ = new QLabel("0", this);
    QFont resFont = resultLabel_->font();
    resFont.setPointSize(22);
    resFont.setBold(true);
    resultLabel_->setFont(resFont);
    resultLabel_->setAlignment(Qt::AlignRight);
    resultLabel_->setStyleSheet("color: #2e7d32; padding: 8px;");
    mainLayout->addWidget(resultLabel_);

    auto* grid = new QGridLayout();
    mainLayout->addLayout(grid);

    struct B { QString label; int row; int col; const char* slot; };
    QList<B> buttons = {
        {"C",        0, 0, SLOT(onClearClicked())},
        {"⌫",        0, 1, SLOT(onBackspaceClicked())},
        {"(",        0, 2, SLOT(onParenClicked())},
        {")",        0, 3, SLOT(onParenClicked())},
        {"/",        0, 4, SLOT(onOperatorClicked())},

        {"7",        1, 0, SLOT(onDigitClicked())},
        {"8",        1, 1, SLOT(onDigitClicked())},
        {"9",        1, 2, SLOT(onDigitClicked())},
        {"*",        1, 3, SLOT(onOperatorClicked())},
        {"ln",       1, 4, SLOT(onFunctionClicked())},

        {"4",        2, 0, SLOT(onDigitClicked())},
        {"5",        2, 1, SLOT(onDigitClicked())},
        {"6",        2, 2, SLOT(onDigitClicked())},
        {"-",        2, 3, SLOT(onOperatorClicked())},
        {"exp",      2, 4, SLOT(onFunctionClicked())},

        {"1",        3, 0, SLOT(onDigitClicked())},
        {"2",        3, 1, SLOT(onDigitClicked())},
        {"3",        3, 2, SLOT(onDigitClicked())},
        {"+",        3, 3, SLOT(onOperatorClicked())},
        {"sqrt",     3, 4, SLOT(onFunctionClicked())},

        {"0",        4, 0, SLOT(onDigitClicked())},
        {".",        4, 1, SLOT(onDotClicked())},
        {"=",        4, 2, SLOT(onEqualsClicked())},
        {"=",        4, 3, SLOT(onEqualsClicked())},
        {"abs",      4, 4, SLOT(onFunctionClicked())},
    };

    for (const B& b : buttons) {
        auto* btn = makeButton(b.label, b.label);
        grid->addWidget(btn, b.row, b.col);
        connect(btn, SIGNAL(clicked()), this, b.slot);
    }

    resize(420, 520);
}

QPushButton* MainWindow::makeButton(const QString& text, const QString& objectName) {
    auto* btn = new QPushButton(text, this);
    btn->setObjectName(objectName);
    QFont f = btn->font();
    f.setPointSize(14);
    btn->setFont(f);
    btn->setMinimumSize(60, 50);
    btn->setFocusPolicy(Qt::NoFocus);
    return btn;
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
    if (btn) appendText(btn->text());
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
    resultLabel_->setText("0");
    resultLabel_->setStyleSheet("color: #2e7d32; padding: 8px;");
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
        resultLabel_->setText("0");
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
        resultLabel_->setText(out);
        resultLabel_->setStyleSheet("color: #2e7d32; padding: 8px;");
    } catch (const std::exception& e) {
        resultLabel_->setText(QString("Ошибка: %1").arg(QString::fromStdString(e.what())));
        resultLabel_->setStyleSheet("color: #c62828; padding: 8px;");
    }
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
