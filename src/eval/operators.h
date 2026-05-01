#pragma once

// Применение бинарных и унарных операторов.
// Изолировано от evaluator'а, чтобы добавить новый оператор можно было,
// не трогая основной цикл вычисления.

#include <string>

namespace eval::operators {

// Применяет +, -, *, /. Деление на 0 → EvalError.
double apply_binary(const std::string& op, double a, double b);

// Применяет u- (унарный минус). Других унарных операторов пока нет.
double apply_unary(const std::string& op, double a);

}
