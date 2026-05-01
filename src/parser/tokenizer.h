#pragma once

#include "parser/token.h"

#include <string>
#include <vector>

namespace parser::tokenizer {

bool is_known_function(const std::string& name);

std::vector<Token> tokenize(const std::string& expression);

}
