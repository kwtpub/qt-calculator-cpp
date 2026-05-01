#pragma once

#include <stdexcept>
#include <string>

namespace eval {

class EvalError : public std::runtime_error {
public:
    explicit EvalError(const std::string& msg) : std::runtime_error(msg) {}
};

}
