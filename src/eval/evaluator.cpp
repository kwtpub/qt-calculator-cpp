// =====================================================================
// evaluator.cpp — реализация вычислителя RPN.
//
// Алгоритм классический: проходим по токенам слева направо со стеком
// чисел. Число — кладём в стек. Оператор/функция — снимаем со стека
// нужное число операндов, считаем, кладём результат обратно.
// В конце на стеке должно остаться ровно одно число — это ответ.
// =====================================================================

#include "eval/evaluator.h"
#include "eval/eval_error.h"
#include "eval/functions.h"
#include "eval/operators.h"
#include "parser/parser.h"   // для второго перегруженного evaluate(string)

// std::isfinite — проверка, что результат не inf и не nan.
#include <cmath>

// std::stack — стек чисел для выполнения RPN.
#include <stack>

namespace eval {

// Сокращения для удобства: чтобы дальше не писать parser::Token каждый раз.
using parser::Token;
using parser::TokenType;

// ---------------------------------------------------------------------
// Защита от inf/nan на выходе любой операции.
// Если результат — не конечное число (например, exp(1000) даёт inf,
// либо операция сделала nan), бросаем EvalError("Переполнение").
// ---------------------------------------------------------------------
static double check_finite(double r) {
    if (!std::isfinite(r)) throw EvalError("Переполнение");
    return r;
}

// ---------------------------------------------------------------------
// Снимает со стека нужное число операндов и применяет оператор.
//   - Унарный (u-): 1 операнд.
//   - Бинарный (+ - * /): 2 операнда. Порядок снятия ВАЖЕН:
//     ВЕРХНИЙ элемент стека — это правый операнд (b),
//     ПОД ним — левый (a). Если перепутать — "5 - 3" даст -2.
// ---------------------------------------------------------------------
static void apply_operator(std::stack<double>& st, const Token& t) {
    if (t.text == "u-") {
        // Унарный минус: один операнд.
        if (st.empty()) throw EvalError("Нет операнда для унарного минуса");
        double a = st.top(); st.pop();
        st.push(check_finite(operators::apply_unary(t.text, a)));
        return;
    }
    // Бинарный оператор: нужны два операнда.
    if (st.size() < 2) throw EvalError("Недостаточно операндов для " + t.text);
    double b = st.top(); st.pop();   // ВЕРХНИЙ — правый
    double a = st.top(); st.pop();   // следующий — левый
    st.push(check_finite(operators::apply_binary(t.text, a, b)));
}

// ---------------------------------------------------------------------
// Снимает один аргумент со стека и применяет функцию (ln/exp/sqrt/abs).
// ---------------------------------------------------------------------
static void apply_function(std::stack<double>& st, const Token& t) {
    if (st.empty()) throw EvalError("Нет аргумента для функции " + t.text);
    double a = st.top(); st.pop();
    st.push(check_finite(functions::apply(t.text, a)));
}

// =====================================================================
// Главная функция вычисления RPN.
// =====================================================================
double evaluate(const std::vector<Token>& rpn) {
    std::stack<double> st;   // стек чисел

    // Стандартный обход RPN: число — на стек, оператор/функция —
    // снять операнды, посчитать, положить результат.
    for (const Token& t : rpn) {
        switch (t.type) {
            case TokenType::Number:    st.push(t.value);          break;
            case TokenType::Operator:  apply_operator(st, t);     break;
            case TokenType::Function:  apply_function(st, t);     break;

            // Скобки в RPN попасть не могут — shunting_yard их выкидывает.
            // Если попали — это баг программы, а не пользовательского ввода.
            default: throw EvalError("Неожиданный токен в RPN");
        }
    }

    // По завершении на стеке должно остаться РОВНО ОДНО число — ответ.
    // Если 0 — выражение было пустое (но это поймал бы tokenizer).
    // Если >1 — RPN был структурно неполный, например "2 3" без оператора.
    if (st.size() != 1) throw EvalError("Некорректное выражение");
    return st.top();
}

// Перегрузка-фасад: строка → парсер → вычислитель.
// Используется из UI и из тестов.
double evaluate(const std::string& expression) {
    return evaluate(parser::parse(expression));
}

}  // namespace eval
