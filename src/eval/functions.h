#pragma once

// Математические функции варианта 2: ln, exp, sqrt, abs.
// Каждая обёрнута отдельно, чтобы её было легко найти и протестировать.
// Бросают EvalError, если аргумент вне области определения.

#include <string>

namespace eval::functions {

double apply_ln(double x);     // x > 0; иначе EvalError
double apply_exp(double x);    // любое x
double apply_sqrt(double x);   // x >= 0; иначе EvalError
double apply_abs(double x);    // любое x

// Диспатчер по имени функции — используется evaluator'ом, который
// видит только строку из токена. Бросает EvalError для неизвестного имени.
double apply(const std::string& name, double x);

}
