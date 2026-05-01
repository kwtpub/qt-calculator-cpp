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
    void on_digit_clicked();
    void on_operator_clicked();
    void on_function_clicked();
    void on_paren_clicked();
    void on_dot_clicked();
    void on_clear_clicked();
    void on_backspace_clicked();
    void on_equals_clicked();

private:
    enum class ButtonKind { Digit, Operator, Function, Action, Equals };

    QLineEdit* expression_edit_;
    QLabel* result_label_;
    QLabel* history_label_;

    QPushButton* make_button(const QString& text, ButtonKind kind);
    void append_text(const QString& text);
    void compute();
    void apply_theme();
};
