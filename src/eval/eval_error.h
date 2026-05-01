#pragma once

// Исключение для ошибок на этапе вычисления (математика).
// Бросается из functions, operators и evaluator.
// Для синтаксических ошибок есть отдельный ParseError в parser/.

#include <stdexcept>
#include <string>

namespace eval {

class EvalError : public std::runtime_error {
public:
    explicit EvalError(const std::string& msg) : std::runtime_error(msg) {}
};

}
