#include "Evaluator.h"

#include <cmath>
#include <stack>

double Evaluator::evaluate(const std::vector<Token>& rpn) const {
    std::stack<double> st;

    for (const Token& t : rpn) {
        if (t.type == TokenType::Number) {
            st.push(t.value);
        } else if (t.type == TokenType::Operator) {
            if (t.text == "u-") {
                if (st.empty()) throw EvalError("Нет операнда для унарного минуса");
                double a = st.top(); st.pop();
                st.push(-a);
            } else {
                if (st.size() < 2) throw EvalError("Недостаточно операндов для " + t.text);
                double b = st.top(); st.pop();
                double a = st.top(); st.pop();
                double r = 0.0;
                if (t.text == "+") r = a + b;
                else if (t.text == "-") r = a - b;
                else if (t.text == "*") r = a * b;
                else if (t.text == "/") {
                    if (b == 0.0) throw EvalError("Деление на ноль");
                    r = a / b;
                } else throw EvalError("Неизвестный оператор: " + t.text);
                if (!std::isfinite(r)) throw EvalError("Переполнение");
                st.push(r);
            }
        } else if (t.type == TokenType::Function) {
            if (st.empty()) throw EvalError("Нет аргумента для функции " + t.text);
            double a = st.top(); st.pop();
            double r = 0.0;
            if (t.text == "ln") {
                if (a <= 0.0) throw EvalError("ln от неположительного числа");
                r = std::log(a);
            } else if (t.text == "exp") {
                r = std::exp(a);
            } else if (t.text == "sqrt") {
                if (a < 0.0) throw EvalError("sqrt от отрицательного числа");
                r = std::sqrt(a);
            } else if (t.text == "abs") {
                r = std::fabs(a);
            } else throw EvalError("Неизвестная функция: " + t.text);
            if (!std::isfinite(r)) throw EvalError("Переполнение");
            st.push(r);
        } else {
            throw EvalError("Неожиданный токен в RPN");
        }
    }

    if (st.size() != 1) throw EvalError("Некорректное выражение");
    return st.top();
}

double Evaluator::evaluate(const std::string& expression) const {
    Parser p;
    return evaluate(p.parse(expression));
}
