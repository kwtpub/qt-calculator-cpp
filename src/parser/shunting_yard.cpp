// =====================================================================
// shunting_yard.cpp — реализация алгоритма Дейкстры.
//
// Идея алгоритма (1961, Эдсгер Дейкстра):
//   - идём по входным токенам слева направо;
//   - числа сразу пишем в выход;
//   - операторы кладём во временный стек, но перед этим вытаскиваем
//     из стека всё, что связывает СИЛЬНЕЕ или РАВНО (для левоассоц.);
//   - открывающую скобку кладём как маркер;
//   - на закрывающей скобке вытаскиваем всё до парной открывающей.
// =====================================================================

#include "parser/shunting_yard.h"
#include "parser/parse_error.h"

// std::stack — обёртка над контейнером с операциями push/pop/top.
// Используется как временное хранилище операторов и скобок.
#include <stack>

namespace parser::shunting_yard {

// Приоритет оператора. Чем больше — тем сильнее связывает.
int precedence(const std::string& op) {
    if (op == "u-") return 4;             // унарный минус — самый сильный
    if (op == "*" || op == "/") return 3; // умножение и деление
    if (op == "+" || op == "-") return 2; // сложение и вычитание
    return 0;                              // всё остальное (скобки)
}

// Правоассоциативные у нас только унарный минус.
bool is_right_associative(const std::string& op) {
    return op == "u-";
}

// ---------------------------------------------------------------------
// Решает: нужно ли вытолкнуть оператор top (вершина стека) перед тем,
// как положить туда current. Это и есть СЕРДЦЕ алгоритма Дейкстры.
//
// Выталкиваем, если:
//   1) top — функция (функции всегда сильнее любого оператора),
//   2) ИЛИ приоритет top строго выше приоритета current,
//   3) ИЛИ приоритеты равны и current левоассоциативный.
// ---------------------------------------------------------------------
static bool should_pop(const Token& top, const Token& current) {
    if (top.type == TokenType::Function) return true;
    if (precedence(top.text) > precedence(current.text)) return true;
    if (precedence(top.text) == precedence(current.text) && !is_right_associative(current.text)) return true;
    return false;
}

// =====================================================================
// Главный цикл преобразования инфикс → RPN.
// =====================================================================
std::vector<Token> to_rpn(const std::vector<Token>& tokens) {
    std::vector<Token> output;     // итоговая RPN-последовательность
    std::stack<Token> ops;         // временный стек операторов и скобок

    // Идём по входу слева направо.
    for (const Token& t : tokens) {
        switch (t.type) {

            case TokenType::Number:
                // Число сразу попадает в выход.
                output.push_back(t);
                break;

            case TokenType::Function:
                // Функция кладётся в стек и ждёт там, пока не закроется
                // её скобка с аргументом — тогда её "вытащит" RightParen.
                ops.push(t);
                break;

            case TokenType::Operator:
                // Перед тем как положить новый оператор, вытаскиваем
                // из стека всё, что связывает СИЛЬНЕЕ.
                // Условие "ops.top().type != LeftParen" — чтобы не залезать
                // за маркер скобки (внутри неё своя зона приоритета).
                while (!ops.empty() && ops.top().type != TokenType::LeftParen && should_pop(ops.top(), t)) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                ops.push(t);
                break;

            case TokenType::LeftParen:
                // Открывающую скобку кладём как маркер. Она НЕ попадает
                // в выход — RPN скобок не содержит, порядок задаётся самой
                // структурой массива.
                ops.push(t);
                break;

            case TokenType::RightParen: {
                // На закрывающей скобке выкидываем в выход всё, что лежит
                // в стеке поверх парной открывающей. Саму открывающую — выкидываем.
                bool found_left = false;
                while (!ops.empty()) {
                    if (ops.top().type == TokenType::LeftParen) {
                        ops.pop();         // скобку убираем со стека
                        found_left = true; // нашли парную — отметили
                        break;
                    }
                    output.push_back(ops.top());
                    ops.pop();
                }
                // Не нашли парную — значит лишняя ')' в выражении.
                if (!found_left) throw ParseError("Несбалансированные скобки: лишняя ')'");

                // Если на вершине осталась функция — это была её скобка
                // с аргументом. Функцию тоже выкидываем в выход:
                // её аргумент уже там лежит, и evaluator это поймёт.
                if (!ops.empty() && ops.top().type == TokenType::Function) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                break;
            }
        }
    }

    // Дочистка: всё что осталось в стеке после конца входа — в выход.
    // Если осталась '(' — её парная ')' так и не пришла.
    while (!ops.empty()) {
        if (ops.top().type == TokenType::LeftParen) throw ParseError("Несбалансированные скобки: лишняя '('");
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}

}  // namespace parser::shunting_yard
