#pragma once

#include <string>
#include <vector>
#include <stdexcept>

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

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

class Parser {
public:
    std::vector<Token> tokenize(const std::string& expression) const;
    std::vector<Token> toRPN(const std::vector<Token>& tokens) const;
    std::vector<Token> parse(const std::string& expression) const;

private:
    static int precedence(const std::string& op);
    static bool isRightAssociative(const std::string& op);
    static bool isFunction(const std::string& name);
};
