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
    QLineEdit* expressionEdit_;
    QLabel* resultLabel_;

    QPushButton* makeButton(const QString& text, const QString& objectName);
    void appendText(const QString& text);
    void compute();
};
