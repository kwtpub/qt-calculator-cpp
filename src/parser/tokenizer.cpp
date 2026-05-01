#include "parser/tokenizer.h"
#include "parser/parse_error.h"

#include <cctype>
#include <unordered_set>

namespace parser::tokenizer {

bool is_known_function(const std::string& name) {
    // unordered_set даёт O(1) поиск; static — таблица создаётся один раз.
    static const std::unordered_set<std::string> funcs = {"ln", "exp", "sqrt", "abs"};
    return funcs.count(name) > 0;
}

// Читает число, начиная с позиции i. Двигает i до символа после числа.
// Допускает не более одной точки. Поддерживает форматы: 42, 3.14, .5
static Token read_number(const std::string& expression, size_t& i) {
    const size_t n = expression.size();
    const size_t start = i;
    bool has_dot = false;
    while (i < n && (std::isdigit(static_cast<unsigned char>(expression[i])) || expression[i] == '.')) {
        if (expression[i] == '.') {
            if (has_dot) throw ParseError("Некорректное число: две точки");
            has_dot = true;
        }
        ++i;
    }
    std::string num = expression.substr(start, i - start);
    if (num == ".") throw ParseError("Некорректное число: одиночная точка");
    return {TokenType::Number, num, std::stod(num)};
}

// Читает имя функции (последовательность букв) и проверяет, что оно известно.
static Token read_function(const std::string& expression, size_t& i) {
    const size_t n = expression.size();
    const size_t start = i;
    while (i < n && std::isalpha(static_cast<unsigned char>(expression[i]))) ++i;
    std::string name = expression.substr(start, i - start);
    if (!is_known_function(name)) throw ParseError("Неизвестная функция: " + name);
    return {TokenType::Function, name, 0.0};
}

// Решает, является ли минус унарным в текущей позиции.
// Унарный, если перед ним: ничего, оператор, открывающая скобка или функция.
// Иначе бинарный (вычитание).
// Примеры: "-5" (начало), "2*-3" (после оператора), "-(1+2)" (начало),
//          "sqrt(-4)" (после функции и "(").
static bool is_unary_minus_context(const std::vector<Token>& tokens) {
    if (tokens.empty()) return true;
    const Token& prev = tokens.back();
    return prev.type == TokenType::Operator
        || prev.type == TokenType::LeftParen
        || prev.type == TokenType::Function;
}

// Создаёт токен оператора. Для минуса дополнительно решает: унарный или бинарный.
// Также проверяет случай двойного оператора ("2++3").
static Token read_operator(char c, const std::vector<Token>& tokens) {
    std::string op(1, c);
    if (c == '-' && is_unary_minus_context(tokens)) {
        return {TokenType::Operator, "u-", 0.0};
    }
    // Если предыдущий токен — оператор (не унарный минус), значит идут подряд.
    if (!tokens.empty() && tokens.back().type == TokenType::Operator && tokens.back().text != "u-") {
        throw ParseError(std::string("Двойной оператор: ") + tokens.back().text + op);
    }
    return {TokenType::Operator, op, 0.0};
}

std::vector<Token> tokenize(const std::string& expression) {
    std::vector<Token> tokens;
    size_t i = 0;
    const size_t n = expression.size();

    // Главный цикл: смотрим первый символ, по нему решаем, какой токен читать.
    while (i < n) {
        char c = expression[i];

        if (std::isspace(static_cast<unsigned char>(c))) { ++i; continue; }

        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            tokens.push_back(read_number(expression, i));
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(c))) {
            tokens.push_back(read_function(expression, i));
            continue;
        }

        if (c == '(') { tokens.push_back({TokenType::LeftParen, "(", 0.0}); ++i; continue; }
        if (c == ')') { tokens.push_back({TokenType::RightParen, ")", 0.0}); ++i; continue; }

        if (c == '+' || c == '-' || c == '*' || c == '/') {
            tokens.push_back(read_operator(c, tokens));
            ++i;
            continue;
        }

        throw ParseError(std::string("Неизвестный символ: ") + c);
    }

    if (tokens.empty()) throw ParseError("Пустое выражение");
    return tokens;
}

}
