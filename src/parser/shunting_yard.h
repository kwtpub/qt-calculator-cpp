#pragma once

#include "parser/token.h"

#include <string>
#include <vector>

namespace parser::shunting_yard {

int precedence(const std::string& op);
bool is_right_associative(const std::string& op);

std::vector<Token> to_rpn(const std::vector<Token>& tokens);

}
