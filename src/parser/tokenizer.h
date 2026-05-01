#pragma once

// Токенизатор (лексический анализатор).
// Превращает строку выражения в массив токенов.
// Пример: "2+3*sqrt(16)" → [Number 2, Op +, Number 3, Op *, Func sqrt, ( , Number 16, )]

#include "parser/token.h"

#include <string>
#include <vector>

namespace parser::tokenizer {

// Проверяет, поддерживается ли функция с таким именем.
// На текущий момент: ln, exp, sqrt, abs (вариант 2).
bool is_known_function(const std::string& name);

// Главная функция модуля. Бросает ParseError при синтаксических ошибках:
// двойные точки, неизвестная функция, неизвестный символ, пустое выражение.
std::vector<Token> tokenize(const std::string& expression);

}
