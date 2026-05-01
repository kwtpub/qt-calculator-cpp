#include "eval/evaluator.h"
#include "eval/eval_error.h"
#include "eval/functions.h"
#include "eval/operators.h"
#include "parser/parser.h"

#include <cmath>
#include <stack>

namespace eval {

using parser::Token;
using parser::TokenType;

static double check_finite(double r) {
    if (!std::isfinite(r)) throw EvalError("Переполнение");
    return r;
}

static void apply_operator(std::stack<double>& st, const Token& t) {
    if (t.text == "u-") {
        if (st.empty()) throw EvalError("Нет операнда для унарного минуса");
        double a = st.top(); st.pop();
        st.push(check_finite(operators::apply_unary(t.text, a)));
        return;
    }
    if (st.size() < 2) throw EvalError("Недостаточно операндов для " + t.text);
    double b = st.top(); st.pop();
    double a = st.top(); st.pop();
    st.push(check_finite(operators::apply_binary(t.text, a, b)));
}

static void apply_function(std::stack<double>& st, const Token& t) {
    if (st.empty()) throw EvalError("Нет аргумента для функции " + t.text);
    double a = st.top(); st.pop();
    st.push(check_finite(functions::apply(t.text, a)));
}

double evaluate(const std::vector<Token>& rpn) {
    std::stack<double> st;

    for (const Token& t : rpn) {
        switch (t.type) {
            case TokenType::Number:    st.push(t.value);          break;
            case TokenType::Operator:  apply_operator(st, t);     break;
            case TokenType::Function:  apply_function(st, t);     break;
            default: throw EvalError("Неожиданный токен в RPN");
        }
    }

    if (st.size() != 1) throw EvalError("Некорректное выражение");
    return st.top();
}

double evaluate(const std::string& expression) {
    return evaluate(parser::parse(expression));
}

}
