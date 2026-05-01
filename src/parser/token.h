#pragma once

// Описание токена — атомарной единицы выражения.
// Токены — это результат работы токенизатора и вход для shunting_yard.

#include <string>

namespace parser {

// Тип токена. По нему вычислитель и shunting_yard понимают,
// что делать с конкретным элементом.
enum class TokenType {
    Number,       // 2, 3.14, 100
    Operator,     // +, -, *, /, u-  (u- — унарный минус)
    Function,     // ln, exp, sqrt, abs
    LeftParen,    // (
    RightParen    // )
};

// Один токен: его тип, исходный текст и (для чисел) разобранное значение.
// Для не-чисел поле value не используется.
struct Token {
    TokenType type;
    std::string text;
    double value = 0.0;
};

}
