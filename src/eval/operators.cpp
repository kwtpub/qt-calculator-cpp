#include "eval/operators.h"
#include "eval/eval_error.h"

namespace eval::operators {

double apply_binary(const std::string& op, double a, double b) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0.0) throw EvalError("Деление на ноль");
        return a / b;
    }
    throw EvalError("Неизвестный оператор: " + op);
}

double apply_unary(const std::string& op, double a) {
    if (op == "u-") return -a;
    throw EvalError("Неизвестный унарный оператор: " + op);
}

}
