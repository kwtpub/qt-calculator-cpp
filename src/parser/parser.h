#pragma once

// Фасад парсера: композиция tokenizer + shunting_yard.
// Удобный одношаговый API для тех, кому не нужен промежуточный массив токенов.

#include "parser/token.h"
#include "parser/parse_error.h"
#include "parser/tokenizer.h"
#include "parser/shunting_yard.h"

#include <string>
#include <vector>

namespace parser {

// Строка → токены → RPN.
inline std::vector<Token> parse(const std::string& expression) {
    return shunting_yard::to_rpn(tokenizer::tokenize(expression));
}

}
