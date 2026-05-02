// =====================================================================
// parse_error.h — исключение для СИНТАКСИЧЕСКИХ ошибок выражения.
// Бросается из tokenizer (двойные точки, неизвестная функция, лишний
// символ) и из shunting_yard (несбалансированные скобки).
//
// Для МАТЕМАТИЧЕСКИХ ошибок (деление на 0, ln отрицательного и т.п.)
// используется отдельный класс EvalError из eval/eval_error.h.
// Разделение позволяет ловить их одинаково через std::exception, но
// при желании отличать по типу.
// =====================================================================

#pragma once

// runtime_error — стандартный базовый класс из <stdexcept> для
// "ошибок времени выполнения" (в отличие от logic_error). Хранит
// строку с сообщением, доступную через метод what().
#include <stdexcept>
#include <string>

namespace parser {

// Наследуемся ОТКРЫТО (public), чтобы ParseError можно было ловить
// catch'ем по std::runtime_error и по std::exception.
class ParseError : public std::runtime_error {
public:
    // explicit — запрещает неявное преобразование std::string в ParseError.
    // Защищает от случая throw "ошибка"; — компилятор заставит писать
    // throw ParseError("ошибка");
    //
    // : std::runtime_error(msg) — передаём сообщение в базовый класс,
    // он сохранит его и отдаст обратно через what().
    explicit ParseError(const std::string& msg) : std::runtime_error(msg) {}
};

}  // namespace parser
