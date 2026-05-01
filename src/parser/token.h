#pragma once

#include <string>

namespace parser {

enum class TokenType {
    Number,
    Operator,
    Function,
    LeftParen,
    RightParen
};

struct Token {
    TokenType type;
    std::string text;
    double value = 0.0;
};

}
