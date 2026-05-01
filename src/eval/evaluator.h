#pragma once

// Вычислитель RPN.
// Принимает результат работы парсера (массив токенов в обратной польской записи)
// и считает значение выражения через стек.

#include "parser/token.h"

#include <string>
#include <vector>

namespace eval {

// Вычисление готового RPN. Бросает EvalError при математических ошибках
// и при структурно некорректном RPN (нехватка операндов и т.п.).
double evaluate(const std::vector<parser::Token>& rpn);

// Удобный фасад: строка → парсер → вычисление.
double evaluate(const std::string& expression);

}
