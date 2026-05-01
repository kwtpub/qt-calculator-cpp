#pragma once

#include "parser.h"

#include <stdexcept>
#include <vector>

class EvalError : public std::runtime_error {
public:
    explicit EvalError(const std::string& msg) : std::runtime_error(msg) {}
};

class Evaluator {
public:
    double evaluate(const std::vector<Token>& rpn) const;
    double evaluate(const std::string& expression) const;
};
