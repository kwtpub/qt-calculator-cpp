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

// Решает, надо ли вытолкнуть оператор top из стека операторов перед тем,
// как положить туда current. Это ядро алгоритма Дейкстры.
// Выталкиваем, если top связывает сильнее или (равно и текущий — левоассоциативный).
static bool should_pop(const Token& top, const Token& current) {
    if (top.type == TokenType::Function) return true;  // функция всегда сильнее любого оператора
    if (precedence(top.text) > precedence(current.text)) return true;
    if (precedence(top.text) == precedence(current.text) && !is_right_associative(current.text)) return true;
    return false;
}

std::vector<Token> to_rpn(const std::vector<Token>& tokens) {
    std::vector<Token> output;     // итоговая RPN-последовательность
    std::stack<Token> ops;         // временный стек операторов и скобок

    for (const Token& t : tokens) {
        switch (t.type) {
            case TokenType::Number:
                // Числа сразу попадают в выход.
                output.push_back(t);
                break;

            case TokenType::Function:
                // Функция кладётся в стек и ждёт своих аргументов в скобках.
                ops.push(t);
                break;

            case TokenType::Operator:
                // Выталкиваем всё, что связывает сильнее (или равно для левоассоц.),
                // пока не упёрлись в открывающую скобку.
                while (!ops.empty() && ops.top().type != TokenType::LeftParen && should_pop(ops.top(), t)) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                ops.push(t);
                break;

            case TokenType::LeftParen:
                // Открывающая скобка — маркер в стеке для парного ')'.
                ops.push(t);
                break;

            case TokenType::RightParen: {
                // Выталкиваем всё в выход до парной '('. Саму '(' выкидываем.
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
                // Если на вершине осталась функция — это была её скобка с аргументом, тоже в выход.
                if (!ops.empty() && ops.top().type == TokenType::Function) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                break;
            }
        }
    }

    // Дочищаем хвост: всё что осталось в стеке — в выход.
    // Если осталась '(' — её парная ')' так и не пришла.
    while (!ops.empty()) {
        if (ops.top().type == TokenType::LeftParen) throw ParseError("Несбалансированные скобки: лишняя '('");
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}

}
