#pragma once

// Исключение для ошибок на этапе разбора выражения (синтаксис).
// Бросается из tokenizer и shunting_yard.
// Для математических ошибок (деление на 0 и т.п.) есть отдельный EvalError.

#include <stdexcept>
#include <string>

namespace parser {

class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

}
