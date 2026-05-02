// =====================================================================
// parser.h — фасад модуля parser.
// Композиция tokenizer + shunting_yard в одну функцию parse().
// Удобен тем, кому не нужен промежуточный массив токенов и кто
// сразу хочет получить RPN из строки.
// =====================================================================

#pragma once

// Подключаем все публичные части парсера: типы, исключения и обе фазы.
#include "parser/token.h"
#include "parser/parse_error.h"
#include "parser/tokenizer.h"
#include "parser/shunting_yard.h"

#include <string>
#include <vector>

namespace parser {

// Полный конвейер: строка → токены → RPN.
// inline — функция определена в заголовке, чтобы можно было включать
// этот файл в несколько единиц трансляции без linker-ошибки
// "multiple definition".
//
// Бросает ParseError при любых синтаксических проблемах
// (внутри tokenize или to_rpn). Вызывающий код ловит их одним catch'ем.
inline std::vector<Token> parse(const std::string& expression) {
    return shunting_yard::to_rpn(tokenizer::tokenize(expression));
}

}  // namespace parser
