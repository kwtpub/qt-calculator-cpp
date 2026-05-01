#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onDigitClicked();
    void onOperatorClicked();
    void onFunctionClicked();
    void onParenClicked();
    void onDotClicked();
    void onClearClicked();
    void onBackspaceClicked();
    void onEqualsClicked();

private:
    enum class ButtonKind { Digit, Operator, Function, Action, Equals };

    QLineEdit* expressionEdit_;
    QLabel* resultLabel_;
    QLabel* historyLabel_;

    QPushButton* makeButton(const QString& text, ButtonKind kind);
    void appendText(const QString& text);
    void compute();
    void applyTheme();
};
