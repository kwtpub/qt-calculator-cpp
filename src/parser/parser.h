#pragma once

#include "parser/token.h"
#include "parser/parse_error.h"
#include "parser/tokenizer.h"
#include "parser/shunting_yard.h"

#include <string>
#include <vector>

namespace parser {

inline std::vector<Token> parse(const std::string& expression) {
    return shunting_yard::to_rpn(tokenizer::tokenize(expression));
}

}
