#include "ui/theme.h"

namespace ui::theme {

QString stylesheet() {
    return R"(
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
}

}
