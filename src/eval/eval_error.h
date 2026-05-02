// =====================================================================
// eval_error.h — исключение для МАТЕМАТИЧЕСКИХ ошибок вычисления.
// Бросается из functions, operators и evaluator:
//   - деление на ноль,
//   - ln/sqrt от числа вне области определения,
//   - переполнение (inf/nan),
//   - структурные ошибки RPN (нехватка операндов).
//
// Для СИНТАКСИЧЕСКИХ ошибок (двойные точки, лишние скобки, и т.п.)
// используется отдельный класс ParseError из parser/parse_error.h.
// =====================================================================

#pragma once

#include <stdexcept>
#include <string>

namespace eval {

// Тот же приём, что и у ParseError: наследуемся от runtime_error,
// чтобы можно было ловить как ParseError, так и EvalError одним
// catch (const std::exception&) — у обоих общий базовый класс.
class EvalError : public std::runtime_error {
public:
    explicit EvalError(const std::string& msg) : std::runtime_error(msg) {}
};

}  // namespace eval
