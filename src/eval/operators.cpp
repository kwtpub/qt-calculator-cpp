// =====================================================================
// operators.cpp — реализация арифметических операторов.
// =====================================================================

#include "eval/operators.h"
#include "eval/eval_error.h"   // для бросания EvalError

namespace eval::operators {

// Бинарные операторы. Простой диспетчер по тексту оператора.
double apply_binary(const std::string& op, double a, double b) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        // Деление на ноль — единственная "математическая" ошибка
        // среди арифметических операторов. Бросаем EvalError.
        if (b == 0.0) throw EvalError("Деление на ноль");
        return a / b;
    }
    // Сюда мы попадаем, только если в коде где-то выше прошёл
    // неизвестный текст оператора — это ошибка ПРОГРАММЫ, а не ввода.
    throw EvalError("Неизвестный оператор: " + op);
}

// Унарные операторы. Сейчас один — "u-".
double apply_unary(const std::string& op, double a) {
    if (op == "u-") return -a;
    throw EvalError("Неизвестный унарный оператор: " + op);
}

}  // namespace eval::operators
