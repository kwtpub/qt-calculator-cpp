#pragma once

#include <stdexcept>
#include <string>

namespace parser {

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

}
