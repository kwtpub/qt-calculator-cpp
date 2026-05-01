#include "eval/functions.h"
#include "eval/eval_error.h"

#include <cmath>

namespace eval::functions {

double apply_ln(double x) {
    if (x <= 0.0) throw EvalError("ln от неположительного числа");
    return std::log(x);
}

double apply_exp(double x) {
    return std::exp(x);
}

double apply_sqrt(double x) {
    if (x < 0.0) throw EvalError("sqrt от отрицательного числа");
    return std::sqrt(x);
}

double apply_abs(double x) {
    return std::fabs(x);
}

double apply(const std::string& name, double x) {
    if (name == "ln")   return apply_ln(x);
    if (name == "exp")  return apply_exp(x);
    if (name == "sqrt") return apply_sqrt(x);
    if (name == "abs")  return apply_abs(x);
    throw EvalError("Неизвестная функция: " + name);
}

}
