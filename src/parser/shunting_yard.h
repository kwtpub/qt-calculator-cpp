#pragma once

// Алгоритм Дейкстры (Shunting-yard).
// Преобразует массив токенов из инфиксной формы (2+3*4) в обратную
// польскую запись / RPN (2 3 4 * +). После этого RPN легко вычисляется
// одним проходом по стеку — этим занимается evaluator.

#include "parser/token.h"

#include <string>
#include <vector>

namespace parser::shunting_yard {

// Приоритет оператора. Чем больше число, тем сильнее связывает.
// u-=4, * / =3, + - =2.
int precedence(const std::string& op);

// true для операторов с правой ассоциативностью.
// У нас такой только унарный минус: --5 = -(-5), а не (-)(-5).
bool is_right_associative(const std::string& op);

// Главная функция модуля. Бросает ParseError при несбалансированных скобках.
std::vector<Token> to_rpn(const std::vector<Token>& tokens);

}
