#pragma once

#include "parser/token.h"

#include <string>
#include <vector>

namespace eval {

double evaluate(const std::vector<parser::Token>& rpn);
double evaluate(const std::string& expression);

}
