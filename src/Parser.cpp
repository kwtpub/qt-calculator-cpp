#include "Parser.h"

#include <cctype>
#include <stack>
#include <unordered_set>

int Parser::precedence(const std::string& op) {
    if (op == "u-") return 4;
    if (op == "*" || op == "/") return 3;
    if (op == "+" || op == "-") return 2;
    return 0;
}

bool Parser::isRightAssociative(const std::string& op) {
    return op == "u-";
}

bool Parser::isFunction(const std::string& name) {
    static const std::unordered_set<std::string> funcs = {"ln", "exp", "sqrt", "abs"};
    return funcs.count(name) > 0;
}

std::vector<Token> Parser::tokenize(const std::string& expression) const {
    std::vector<Token> tokens;
    size_t i = 0;
    const size_t n = expression.size();

    while (i < n) {
        char c = expression[i];

        if (std::isspace(static_cast<unsigned char>(c))) {
            ++i;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            size_t start = i;
            bool hasDot = false;
            while (i < n && (std::isdigit(static_cast<unsigned char>(expression[i])) || expression[i] == '.')) {
                if (expression[i] == '.') {
                    if (hasDot) throw ParseError("Некорректное число: две точки");
                    hasDot = true;
                }
                ++i;
            }
            std::string num = expression.substr(start, i - start);
            if (num == ".") throw ParseError("Некорректное число: одиночная точка");
            Token t{TokenType::Number, num, std::stod(num)};
            tokens.push_back(t);
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(c))) {
            size_t start = i;
            while (i < n && std::isalpha(static_cast<unsigned char>(expression[i]))) ++i;
            std::string name = expression.substr(start, i - start);
            if (!isFunction(name)) throw ParseError("Неизвестная функция: " + name);
            tokens.push_back({TokenType::Function, name, 0.0});
            continue;
        }

        if (c == '(') {
            tokens.push_back({TokenType::LeftParen, "(", 0.0});
            ++i;
            continue;
        }
        if (c == ')') {
            tokens.push_back({TokenType::RightParen, ")", 0.0});
            ++i;
            continue;
        }

        if (c == '+' || c == '-' || c == '*' || c == '/') {
            std::string op(1, c);
            bool unary = false;
            if (c == '-') {
                if (tokens.empty()) {
                    unary = true;
                } else {
                    const Token& prev = tokens.back();
                    if (prev.type == TokenType::Operator || prev.type == TokenType::LeftParen || prev.type == TokenType::Function) {
                        unary = true;
                    }
                }
            }
            if (unary) {
                tokens.push_back({TokenType::Operator, "u-", 0.0});
            } else {
                if (!tokens.empty() && tokens.back().type == TokenType::Operator && tokens.back().text != "u-") {
                    throw ParseError(std::string("Двойной оператор: ") + tokens.back().text + op);
                }
                tokens.push_back({TokenType::Operator, op, 0.0});
            }
            ++i;
            continue;
        }

        throw ParseError(std::string("Неизвестный символ: ") + c);
    }

    if (tokens.empty()) throw ParseError("Пустое выражение");
    return tokens;
}

std::vector<Token> Parser::toRPN(const std::vector<Token>& tokens) const {
    std::vector<Token> output;
    std::stack<Token> ops;

    for (const Token& t : tokens) {
        switch (t.type) {
            case TokenType::Number:
                output.push_back(t);
                break;
            case TokenType::Function:
                ops.push(t);
                break;
            case TokenType::Operator: {
                while (!ops.empty() && ops.top().type != TokenType::LeftParen) {
                    const Token& top = ops.top();
                    bool topIsFunc = top.type == TokenType::Function;
                    bool higher = topIsFunc || precedence(top.text) > precedence(t.text);
                    bool equalLeftAssoc = !topIsFunc && precedence(top.text) == precedence(t.text) && !isRightAssociative(t.text);
                    if (higher || equalLeftAssoc) {
                        output.push_back(top);
                        ops.pop();
                    } else {
                        break;
                    }
                }
                ops.push(t);
                break;
            }
            case TokenType::LeftParen:
                ops.push(t);
                break;
            case TokenType::RightParen: {
                bool foundLeft = false;
                while (!ops.empty()) {
                    if (ops.top().type == TokenType::LeftParen) {
                        ops.pop();
                        foundLeft = true;
                        break;
                    }
                    output.push_back(ops.top());
                    ops.pop();
                }
                if (!foundLeft) throw ParseError("Несбалансированные скобки: лишняя ')'");
                if (!ops.empty() && ops.top().type == TokenType::Function) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                break;
            }
        }
    }

    while (!ops.empty()) {
        if (ops.top().type == TokenType::LeftParen) throw ParseError("Несбалансированные скобки: лишняя '('");
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}

std::vector<Token> Parser::parse(const std::string& expression) const {
    return toRPN(tokenize(expression));
}
