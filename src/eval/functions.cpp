// =====================================================================
// functions.cpp — реализации функций варианта 2.
// Простые обёртки над <cmath> с проверкой области определения.
// =====================================================================

#include "eval/functions.h"
#include "eval/eval_error.h"

// <cmath> даёт std::log, std::exp, std::sqrt, std::fabs.
#include <cmath>

namespace eval::functions {

// ln(x) — натуральный логарифм. std::log в <cmath> — это именно ln,
// не log10. Для x = 0 он бы вернул -inf, для x < 0 — nan. Чтобы
// пользователь получил понятное сообщение, проверяем сами.
double apply_ln(double x) {
    if (x <= 0.0) throw EvalError("ln от неположительного числа");
    return std::log(x);
}

// exp(x) = e^x. Определена для любого x. Если x очень большой
// (например, 1000), результат уйдёт в +inf — это не ошибка функции,
// а переполнение double; ловится в evaluator'е через check_finite.
double apply_exp(double x) {
    return std::exp(x);
}

// sqrt(x) — квадратный корень. Определён для x >= 0; для отрицательных
// std::sqrt вернул бы nan, но мы предпочитаем явное сообщение.
double apply_sqrt(double x) {
    if (x < 0.0) throw EvalError("sqrt от отрицательного числа");
    return std::sqrt(x);
}

// abs(x) — модуль. fabs работает с double; std::abs тоже бы сработал,
// но fabs всегда возвращает double, без перегрузок — чуть надёжнее.
double apply_abs(double x) {
    return std::fabs(x);
}

// Диспетчер по имени. Один обычный if-каскад — функций всего 4.
double apply(const std::string& name, double x) {
    if (name == "ln")   return apply_ln(x);
    if (name == "exp")  return apply_exp(x);
    if (name == "sqrt") return apply_sqrt(x);
    if (name == "abs")  return apply_abs(x);
    throw EvalError("Неизвестная функция: " + name);
}

}  // namespace eval::functions
