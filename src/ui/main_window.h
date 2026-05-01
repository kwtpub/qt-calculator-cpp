#pragma once

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
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void handle_action(int action_index);

private:
    QLineEdit* expression_edit_;
    QLabel* result_label_;
    QLabel* history_label_;

    void build_ui();
    void build_buttons(class QGridLayout* grid);
    QPushButton* make_button(const ButtonSpec& spec);

    void append(const QString& text);
    void clear_display();
    void backspace();
    void compute();
    void show_result(const QString& text, bool is_error);
};

}
