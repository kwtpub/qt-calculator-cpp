#pragma once

// Описание раскладки кнопок калькулятора как ДАННЫХ.
// Чтобы добавить/убрать/переместить кнопку, не нужно трогать main_window.cpp —
// достаточно отредактировать массив в button_layout.cpp.

#include <QString>
#include <vector>

namespace ui {

// Стилевая категория — определяет цвет в QSS (см. theme.cpp).
enum class ButtonKind { Digit, Operator, Function, Action, Equals };

// Логическая роль — что делать при клике. Главное окно диспатчит по этому полю.
enum class ButtonAction {
    Digit,       // подставить цифру
    Operator,    // подставить +/-/*//
    Function,    // подставить "имя("
    Paren,       // подставить ( или )
    Dot,         // подставить .
    Clear,       // очистить дисплей
    Backspace,   // стереть последний символ
    Equals       // вычислить
};

// Описание одной кнопки: что показывать, где разместить и как реагировать.
struct ButtonSpec {
    QString label;
    int row;
    int col;
    int row_span;
    int col_span;
    ButtonKind kind;
    ButtonAction action;
};

// Полный список кнопок в порядке появления в сетке.
const std::vector<ButtonSpec>& button_specs();

// Размер сетки нужен для setRowStretch/setColumnStretch.
constexpr int grid_rows = 7;
constexpr int grid_cols = 4;

}
