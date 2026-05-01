#pragma once

#include <QString>
#include <vector>

namespace ui {

enum class ButtonKind { Digit, Operator, Function, Action, Equals };
enum class ButtonAction {
    Digit,
    Operator,
    Function,
    Paren,
    Dot,
    Clear,
    Backspace,
    Equals
};

struct ButtonSpec {
    QString label;
    int row;
    int col;
    int row_span;
    int col_span;
    ButtonKind kind;
    ButtonAction action;
};

const std::vector<ButtonSpec>& button_specs();

constexpr int grid_rows = 7;
constexpr int grid_cols = 4;

}
