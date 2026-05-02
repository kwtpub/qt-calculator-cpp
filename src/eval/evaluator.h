// =====================================================================
// evaluator.h — вычислитель RPN.
//
// Вход — массив токенов в обратной польской записи (результат работы
// shunting_yard). Алгоритм: один проход по массиву со стеком чисел.
// =====================================================================

#pragma once

#include "parser/token.h"

#include <string>
#include <vector>

namespace eval {

// Вычисление готового RPN. Бросает EvalError при:
//  - делении на ноль,
//  - аргументе функции вне области определения,
//  - переполнении (inf/nan),
//  - структурно некорректном RPN (нехватка операндов и т.п.).
double evaluate(const std::vector<parser::Token>& rpn);

// Удобный фасад: сразу строка → парсер → вычисление.
// Используется из UI (MainWindow::compute).
double evaluate(const std::string& expression);

}  // namespace eval
