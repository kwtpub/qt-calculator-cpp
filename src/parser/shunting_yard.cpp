#include "parser/shunting_yard.h"
#include "parser/parse_error.h"

#include <stack>

namespace parser::shunting_yard {

int precedence(const std::string& op) {
    if (op == "u-") return 4;
    if (op == "*" || op == "/") return 3;
    if (op == "+" || op == "-") return 2;
    return 0;
}

bool is_right_associative(const std::string& op) {
    return op == "u-";
}

static bool should_pop(const Token& top, const Token& current) {
    if (top.type == TokenType::Function) return true;
    if (precedence(top.text) > precedence(current.text)) return true;
    if (precedence(top.text) == precedence(current.text) && !is_right_associative(current.text)) return true;
    return false;
}

std::vector<Token> to_rpn(const std::vector<Token>& tokens) {
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

            case TokenType::Operator:
                while (!ops.empty() && ops.top().type != TokenType::LeftParen && should_pop(ops.top(), t)) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                ops.push(t);
                break;

            case TokenType::LeftParen:
                ops.push(t);
                break;

            case TokenType::RightParen: {
                bool found_left = false;
                while (!ops.empty()) {
                    if (ops.top().type == TokenType::LeftParen) {
                        ops.pop();
                        found_left = true;
                        break;
                    }
                    output.push_back(ops.top());
                    ops.pop();
                }
                if (!found_left) throw ParseError("Несбалансированные скобки: лишняя ')'");
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

}
