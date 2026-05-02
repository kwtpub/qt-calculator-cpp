// =====================================================================
// parser.cpp — рекурсивный спуск.
//
// Один класс Parser хранит входную строку и текущую позицию.
// Три рекурсивные функции parseExpression / parseTerm / parseFactor
// разбирают и СРАЗУ вычисляют выражение.
//
// Никаких токенов, никакого RPN, никаких стеков — только три функции,
// которые вызывают друг друга по правилам грамматики.
// =====================================================================

#include "parser.h"

#include <cctype>
#include <cmath>
#include <string>

namespace calc {

class Parser {
    const std::string& s;   // ссылка на исходную строку (не копируем)
    size_t pos = 0;         // текущая позиция в строке

public:
    explicit Parser(const std::string& expr) : s(expr) {}

    // Точка входа: разобрать всё выражение и проверить, что строка
    // дочитана до конца (иначе остались лишние символы).
    double parse() {
        skip_spaces();
        if (pos >= s.size()) throw std::runtime_error("Пустое выражение");
        double r = parse_expression();
        skip_spaces();
        if (pos < s.size()) {
            throw std::runtime_error(
                std::string("Лишний символ: '") + s[pos] + "'");
        }
        return r;
    }

private:
    // Пропустить пробелы.
    void skip_spaces() {
        while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos]))) ++pos;
    }

    // Посмотреть текущий символ, не двигая позицию. Возвращает 0,
    // если строка кончилась.
    char peek() {
        skip_spaces();
        return pos < s.size() ? s[pos] : '\0';
    }

    // Сожрать (consume) текущий символ и вернуть его, сдвинув позицию.
    char eat() {
        skip_spaces();
        return s[pos++];
    }

    // Ожидаем конкретный символ. Если его нет — синтаксическая ошибка.
    void expect(char c) {
        skip_spaces();
        if (pos >= s.size() || s[pos] != c) {
            throw std::runtime_error(std::string("Ожидался символ '") + c + "'");
        }
        ++pos;
    }

    // ----- expression := term (('+' | '-') term)* -----
    // Сложение и вычитание — самые слабые операторы. Поэтому они
    // обрабатываются в самом верхнем уровне рекурсии.
    double parse_expression() {
        double left = parse_term();
        while (true) {
            char c = peek();
            if (c != '+' && c != '-') break;
            eat();
            // После бинарного оператора ждём ОПЕРАНД, не ещё один оператор:
            // так "2++3" поймает ошибку двойного оператора.
            char nxt = peek();
            if (nxt == '+' || nxt == '*' || nxt == '/') {
                throw std::runtime_error(
                    std::string("Двойной оператор: '") + c + nxt + "'");
            }
            double right = parse_term();
            left = (c == '+') ? left + right : left - right;
        }
        return left;
    }

    // ----- term := factor (('*' | '/') factor)* -----
    // Умножение и деление сильнее сложения, поэтому обрабатываются
    // на уровень глубже.
    double parse_term() {
        double left = parse_factor();
        while (true) {
            char c = peek();
            if (c != '*' && c != '/') break;
            eat();
            // После бинарного оператора ждём ОПЕРАНД, не ещё один оператор.
            char nxt = peek();
            if (nxt == '+' || nxt == '*' || nxt == '/') {
                throw std::runtime_error(
                    std::string("Двойной оператор: '") + c + nxt + "'");
            }
            double right = parse_factor();
            if (c == '*') {
                left = left * right;
            } else {
                if (right == 0.0) throw std::runtime_error("Деление на ноль");
                left = left / right;
            }
        }
        return left;
    }

    // ----- factor := число | (expr) | -factor | имя(expr) -----
    // Самый "глубокий" уровень: атомы выражения.
    double parse_factor() {
        char c = peek();

        // Унарный минус — допустим всегда (в начале выражения, после
        // оператора, после '(' и т.п.). Рекурсивно зовём parse_factor,
        // так "--5" = -(-5) = 5 само разворачивается.
        if (c == '-') { eat(); return -parse_factor(); }

        // Скобки: вычисляем содержимое как полноценное выражение.
        if (c == '(') {
            eat();
            double r = parse_expression();
            expect(')');
            return r;
        }

        // Имя функции: ln, exp, sqrt, abs.
        if (std::isalpha(static_cast<unsigned char>(c))) {
            return parse_function();
        }

        // Число (или ошибка, если ничего больше не подошло).
        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            return parse_number();
        }

        if (c == '\0') throw std::runtime_error("Неожиданный конец выражения");
        throw std::runtime_error(std::string("Неожиданный символ: '") + c + "'");
    }

    // Имя функции + аргумент в скобках. После '(' идёт обычное выражение.
    double parse_function() {
        std::string name;
        while (pos < s.size() && std::isalpha(static_cast<unsigned char>(s[pos]))) {
            name += s[pos++];
        }
        expect('(');
        double arg = parse_expression();
        expect(')');
        return apply_function(name, arg);
    }

    // Применение функции по имени. Все четыре функции варианта 2.
    static double apply_function(const std::string& name, double x) {
        if (name == "ln") {
            if (x <= 0.0) throw std::runtime_error("ln от неположительного числа");
            return std::log(x);
        }
        if (name == "exp") {
            double r = std::exp(x);
            if (!std::isfinite(r)) throw std::runtime_error("Переполнение");
            return r;
        }
        if (name == "sqrt") {
            if (x < 0.0) throw std::runtime_error("sqrt от отрицательного числа");
            return std::sqrt(x);
        }
        if (name == "abs") {
            return std::fabs(x);
        }
        throw std::runtime_error("Неизвестная функция: " + name);
    }

    // Чтение числа: целая часть, точка, дробная часть.
    // Парсим вручную, чтобы не зависеть от системной локали
    // (на русской/немецкой стандартный std::stod ломается на точке).
    double parse_number() {
        skip_spaces();
        const size_t start = pos;
        bool has_dot = false;

        while (pos < s.size() && (std::isdigit(static_cast<unsigned char>(s[pos])) || s[pos] == '.')) {
            if (s[pos] == '.') {
                if (has_dot) throw std::runtime_error("Две точки в числе");
                has_dot = true;
            }
            ++pos;
        }

        const std::string num = s.substr(start, pos - start);
        if (num == ".") throw std::runtime_error("Одиночная точка не число");

        // Ручной разбор: целая часть + дробная.
        double value = 0.0;
        size_t k = 0;
        while (k < num.size() && std::isdigit(static_cast<unsigned char>(num[k]))) {
            value = value * 10.0 + (num[k] - '0');
            ++k;
        }
        if (k < num.size() && num[k] == '.') {
            ++k;
            double scale = 0.1;
            while (k < num.size() && std::isdigit(static_cast<unsigned char>(num[k]))) {
                value += (num[k] - '0') * scale;
                scale *= 0.1;
                ++k;
            }
        }
        return value;
    }
};

double evaluate(const std::string& expression) {
    Parser p(expression);
    return p.parse();
}

}  // namespace calc
