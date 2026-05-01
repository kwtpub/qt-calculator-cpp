# Calculator — Variant 2

> Калькулятор на C++/Qt6 с поддержкой `ln`, `exp`, `sqrt`, `abs`.
> Проектная деятельность, группа М3О-103БВ-25, вариант 2.

Этот README — **подробное руководство для команды**: как собрать, как запустить, как устроен код, как менять каждую часть. Если ты в команде и читаешь это впервые — читай по порядку, ничего не пропуская.

---

## Содержание

1. [Состав команды и роли](#1-состав-команды-и-роли)
2. [Что должна делать программа](#2-что-должна-делать-программа)
3. [Установка и сборка на macOS](#3-установка-и-сборка-на-macos)
4. [Запуск программы и тестов](#4-запуск-программы-и-тестов)
5. [Структура проекта](#5-структура-проекта)
6. [Архитектура: 3 модуля + UI](#6-архитектура-3-модуля--ui)
7. [Парсер: токенизация](#7-парсер-этап-1--токенизация)
8. [Парсер: Shunting-yard и RPN](#8-парсер-этап-2--shunting-yard-и-rpn)
9. [Вычислитель RPN](#9-вычислитель-rpn)
10. [Обработка ошибок](#10-обработка-ошибок)
11. [GUI: устройство Qt](#11-gui-устройство-qt)
12. [GUI: стилизация QSS](#12-gui-стилизация-qss)
13. [Как добавить новую функцию](#13-как-добавить-новую-функцию-например-cos)
14. [Как добавить новую кнопку](#14-как-добавить-новую-кнопку)
15. [Как добавить новый тест](#15-как-добавить-новый-тест)
16. [FAQ — частые вопросы](#16-faq--частые-вопросы)
17. [Глоссарий](#17-глоссарий)

---

## 1. Состав команды и роли

| ФИО | Роль | Что делать |
|---|---|---|
| **Гаценко А. О.** | Тимлид, интеграция | Сборка, помощь команде, финальный отчёт |
| **Загорный Т. М.** | Парсер | Разбираться в `src/parser/`, писать раздел отчёта |
| **Калинин К. Е.** | Вычислитель + ошибки | Разбираться в `src/eval/`, писать раздел отчёта |
| **Михалёв К. Д.** | GUI | Разбираться в `src/ui/`, писать раздел отчёта |
| **Рыбин Т. В.** | Тестирование | Запустить программу, сделать 16 скриншотов |
| **Шеянов Я. В.** | Отчёт | Собрать `report.md`, нарисовать блок-схемы |

Подробные чек-листы → `tasks.md`.

---

## 2. Что должна делать программа

### Базовые требования (из `task.md`)

- ✅ Поле ввода математического выражения (с возможностью стирания)
- ✅ Поле результата
- ✅ Цифры 0–9
- ✅ Операции `+`, `-`, `*`, `/`
- ✅ `C` (сброс), `=` (равно), `.` (точка)
- ✅ Скобки `(` `)`
- ✅ Парсер с обработкой ошибок
- ✅ **Вариант 2:** функции `ln`, `exp`, `sqrt`, `abs`

### Дополнительно реализовано

- Унарный минус: `-5`, `-(2+3)`, `3*-4`
- Кнопка ⌫ (Backspace) для удаления последнего символа
- Поддержка ввода с клавиатуры (Enter = `=`, Esc = `C`)
- История последнего выражения
- Цветные сообщения об ошибках (красный) и результат (зелёный)
- 41 юнит-тест парсера и вычислителя

---

## 3. Установка и сборка на macOS

### Шаг 1. Установить инструменты

```bash
# Если нет Homebrew, поставить:
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Установить Qt6 и CMake:
brew install qt cmake
```

Проверка: `qmake --version` должно показать версию Qt 6.x.

### Шаг 2. Клонировать и собрать

```bash
git clone https://github.com/kwtpub/qt-calculator-cpp.git
cd qt-calculator-cpp
cmake -B build -S .
cmake --build build -j
```

### Шаг 3. Если Qt не находится

Если `cmake` ругается «Could not find Qt6», явно укажи путь:

```bash
cmake -B build -S . -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt
```

(на Intel Mac путь обычно `/usr/local/opt/qt`)

### Шаг 4. Сборка на Linux (если у кого-то Linux)

```bash
sudo apt install qt6-base-dev cmake build-essential
cmake -B build -S .
cmake --build build -j
```

---

## 4. Запуск программы и тестов

### Запуск GUI

```bash
# macOS (получается .app-бандл):
open ./build/calculator.app

# или напрямую:
./build/calculator.app/Contents/MacOS/calculator

# Linux:
./build/calculator
```

### Запуск тестов

```bash
cd build
ctest --output-on-failure
```

Должно быть `100% tests passed`. В подробном выводе — `Пройдено: 41, провалено: 0`.

Можно запустить тесты напрямую (с подробным выводом каждого кейса):

```bash
./build/parser_tests
```

---

## 5. Структура проекта

```
qt-calculator-cpp/
├── CMakeLists.txt              # сборка
├── README.md                   # ← ты здесь
├── task.md                     # исходное задание
├── tasks.md                    # распределение работы по людям
├── report.md                   # заготовка отчёта
├── .gitignore
│
├── src/                        # исходный код, разбитый на модули
│   ├── main.cpp                # точка входа
│   │
│   ├── parser/                 # МОДУЛЬ ПАРСЕРА (namespace parser::)
│   │   ├── token.h             # struct Token, enum TokenType
│   │   ├── parse_error.h       # class ParseError
│   │   ├── tokenizer.{h,cpp}   # строка → массив токенов
│   │   ├── shunting_yard.{h,cpp} # токены → RPN (алгоритм Дейкстры)
│   │   └── parser.h            # фасад: parse() = композиция
│   │
│   ├── eval/                   # МОДУЛЬ ВЫЧИСЛИТЕЛЯ (namespace eval::)
│   │   ├── eval_error.h        # class EvalError
│   │   ├── functions.{h,cpp}   # apply_ln, apply_exp, apply_sqrt, apply_abs
│   │   ├── operators.{h,cpp}   # apply_binary, apply_unary
│   │   └── evaluator.{h,cpp}   # evaluate() — стек, делегирует в functions/operators
│   │
│   └── ui/                     # МОДУЛЬ GUI (namespace ui::)
│       ├── main_window.{h,cpp} # класс главного окна (Qt требует класс)
│       ├── theme.{h,cpp}       # QSS-стили (Catppuccin)
│       ├── button_layout.{h,cpp} # описание всех кнопок как ДАННЫХ
│       └── format.{h,cpp}      # ÷→/, ×→*, форматирование числа результата
│
├── tests/
│   └── parser_tests.cpp        # 41 юнит-тест (без Qt, автономно)
│
└── build/                      # генерируется CMake, в git не попадает
    ├── calculator.app          # собранная программа (на Mac)
    └── parser_tests            # бинарник тестов
```

### Стиль кода

- **Файлы и идентификаторы:** `snake_case` (`main_window.cpp`, `apply_ln`).
- **Классы и enum:** `PascalCase` (`MainWindow`, `Token`, `ButtonKind`).
- **Namespace:** `lowercase` (`parser`, `eval`, `ui`).
- **Парсер и вычислитель — функциональный стиль:** свободные функции в namespace, без классов-обёрток. Каждая функция чистая (не мутирует входы, всё через возвращаемое значение).
- **GUI — ОО:** Qt требует наследования от `QMainWindow` для сигналов/слотов. Класс держим тонким, логику делегируем в свободные функции.

---

## 6. Архитектура: 3 модуля + UI

```
   ┌─────────────────────────────────────────────────┐
   │                ui::MainWindow                   │
   │       Qt-виджеты, кнопки, поле ввода            │
   │                                                 │
   │   Пользователь нажал «=»  ──┐                   │
   └─────────────────────────────┼───────────────────┘
                                 │ строка "2+3*4"
                                 ▼
   ┌─────────────────────────────────────────────────┐
   │              eval::evaluate(string)             │
   │                                                 │
   │   1. parser::tokenizer::tokenize(s)             │
   │      "2+3*4" → [2] [+] [3] [*] [4]              │
   │                                                 │
   │   2. parser::shunting_yard::to_rpn(tokens)      │
   │      [2] [+] [3] [*] [4] → [2] [3] [4] [*] [+]  │
   │                                                 │
   │   3. eval::evaluate(rpn)                        │
   │      [2] [3] [4] [*] [+] → 14                   │
   │      └ применяет eval::operators::apply_binary  │
   │      └ применяет eval::functions::apply         │
   └─────────────────────────────┬───────────────────┘
                                 │ double 14.0
                                 ▼
   ┌─────────────────────────────────────────────────┐
   │  ui::MainWindow показывает result_label_        │
   │  с форматированием через ui::format::format_result │
   └─────────────────────────────────────────────────┘
```

**Почему так разделили:**

- **Каждый модуль независим.** Тесты в `tests/parser_tests.cpp` вообще не подключают Qt — компилируются и запускаются автономно. Парсер не знает про вычислитель, а вычислитель не знает про GUI.
- **Каждая функция-операция изолирована.** Например `apply_sqrt` лежит в `eval/functions.cpp` отдельно от остальных — её легко найти, заменить или добавить новую (см. раздел 13).
- **Кнопки — данные, а не код.** Все кнопки описаны массивом в `ui/button_layout.cpp`. Чтобы добавить кнопку, не нужно лезть в логику окна — просто добавь строку в массив.
- **Если завтра захотим консольный UI** — заменим только `ui/`, парсер и вычислитель не тронем.
- **Каждый человек в команде отвечает за свою папку** (`src/parser/`, `src/eval/`, `src/ui/`).

---

## 7. Парсер. Этап 1 — токенизация

**Файлы:** `src/parser/tokenizer.{h,cpp}`. **Namespace:** `parser::tokenizer`.

**Задача:** превратить строку (например `"2+3*sqrt(16)"`) в массив **токенов** — атомов, с которыми удобно работать дальше.

### Типы токенов (см. `src/parser/token.h`)

```cpp
namespace parser {

enum class TokenType {
    Number,      // 2, 3.14, 100
    Operator,    // +, -, *, /, u-  (u- = унарный минус)
    Function,    // ln, exp, sqrt, abs
    LeftParen,   // (
    RightParen   // )
};

struct Token {
    TokenType type;
    std::string text;   // как было в исходнике: "+", "sqrt", "3.14"
    double value = 0.0; // для чисел: 3.14
};

}
```

### Публичный API

```cpp
namespace parser::tokenizer {

bool is_known_function(const std::string& name);
std::vector<Token> tokenize(const std::string& expression);

}
```

### Как работает алгоритм

Идём по строке слева направо. Для каждого символа решаем:

| Символ | Что делаем | Какая внутренняя функция |
|---|---|---|
| Пробел | Пропускаем | — |
| Цифра или `.` | Читаем все следующие цифры/точки → один Number-токен | `read_number` |
| Буква | Читаем все следующие буквы → проверяем что это `ln`/`exp`/`sqrt`/`abs` → Function-токен | `read_function` |
| `(` или `)` | Один токен LeftParen/RightParen | inline |
| `+` `-` `*` `/` | Operator-токен. Для `-` решаем — унарный или бинарный | `read_operator` |
| Что-то другое | Бросаем `ParseError` | — |

`read_number`, `read_function`, `read_operator` — это `static`-функции внутри `tokenizer.cpp`, не торчат наружу.

### Унарный минус — самое сложное место

Минус может быть бинарным (вычитание `5-3`) или унарным (отрицание `-5`).
Различаем по контексту: **если перед минусом ничего нет, или там оператор / `(` / функция — это унарный минус** (помечаем как `u-`):

```cpp
static bool is_unary_minus_context(const std::vector<Token>& tokens) {
    if (tokens.empty()) return true;
    const Token& prev = tokens.back();
    return prev.type == TokenType::Operator
        || prev.type == TokenType::LeftParen
        || prev.type == TokenType::Function;
}
```

### Пример трассировки

Вход: `"2+3*sqrt(16)"`

| Шаг | Позиция | Символ | Действие | Результат |
|---|---|---|---|---|
| 1 | 0 | `2` | цифра, читаем | Number "2" |
| 2 | 1 | `+` | оператор | Operator "+" |
| 3 | 2 | `3` | цифра | Number "3" |
| 4 | 3 | `*` | оператор | Operator "*" |
| 5 | 4–7 | `sqrt` | буквы → функция | Function "sqrt" |
| 6 | 8 | `(` | скобка | LeftParen |
| 7 | 9–10 | `16` | цифры | Number "16" |
| 8 | 11 | `)` | скобка | RightParen |

Итог: `[2] [+] [3] [*] [sqrt] [(] [16] [)]`

---

## 8. Парсер. Этап 2 — Shunting-yard и RPN

**Файлы:** `src/parser/shunting_yard.{h,cpp}`. **Namespace:** `parser::shunting_yard`.

### Что такое RPN (обратная польская запись)

Обычная (инфиксная) запись: `2 + 3 * 4`
RPN: `2 3 4 * +`

В RPN **операторы стоят после своих операндов**, скобки не нужны, порядок вычисления однозначен. Это формат, удобный для компьютера — вычисляется одним проходом по стеку (см. раздел 9).

### Публичный API

```cpp
namespace parser::shunting_yard {

int  precedence(const std::string& op);              // 4 / 3 / 2 / 0
bool is_right_associative(const std::string& op);    // только u-
std::vector<Token> to_rpn(const std::vector<Token>& tokens);

}
```

### Алгоритм Дейкстры (Shunting-yard)

Заводим выходной массив `output` и стек операторов `ops`. Идём по токенам:

| Токен | Действие |
|---|---|
| Number | Добавляем в `output` |
| Function | Кладём в стек `ops` |
| Operator | Пока на вершине `ops` стоит то, что **связывает сильнее** (см. `should_pop`) — выталкиваем в `output`. Потом кладём текущий оператор в `ops` |
| `(` | Кладём в `ops` |
| `)` | Выталкиваем всё из `ops` в `output` до первой `(`. Саму `(` выкидываем. Если на вершине осталась функция — её тоже в `output` |

В конце: вытолкнуть всё из `ops` в `output`. Если встретили `(` — несбалансированные скобки.

### Условие выталкивания (ядро алгоритма)

Внутренняя функция `should_pop(top, current)` отвечает на вопрос: «надо ли вытолкнуть `top` перед тем, как положить `current`?». Логика:

```cpp
static bool should_pop(const Token& top, const Token& current) {
    if (top.type == TokenType::Function) return true;       // функция всегда сильнее любого оператора
    if (precedence(top.text) > precedence(current.text)) return true;
    if (precedence(top.text) == precedence(current.text)
        && !is_right_associative(current.text)) return true; // равный приоритет + левоассоциативный
    return false;
}
```

### Таблица приоритетов

| Оператор | Приоритет | Ассоциативность |
|---|---|---|
| `u-` (унарный минус) | 4 | правая |
| `*`, `/` | 3 | левая |
| `+`, `-` | 2 | левая |

**Левая ассоциативность** (`+`): `1+2+3` = `(1+2)+3`. Поэтому при равном приоритете выталкиваем то, что было раньше.
**Правая ассоциативность** (`u-`): `--5` = `-(-5)`. При равном приоритете НЕ выталкиваем — пусть копится.

### Пример: `2+3*4` → RPN

| Токен | output | ops | Комментарий |
|---|---|---|---|
| `2` | `[2]` | `[]` | число → output |
| `+` | `[2]` | `[+]` | оператор → ops |
| `3` | `[2, 3]` | `[+]` | число → output |
| `*` | `[2, 3]` | `[+, *]` | `*` приоритетнее `+`, не выталкиваем |
| `4` | `[2, 3, 4]` | `[+, *]` | число → output |
| (конец) | `[2, 3, 4, *, +]` | `[]` | вытолкнули всё |

Результат: `2 3 4 * +`

### Пример с функцией: `sqrt(16)+1`

| Токен | output | ops |
|---|---|---|
| `sqrt` | `[]` | `[sqrt]` |
| `(` | `[]` | `[sqrt, (]` |
| `16` | `[16]` | `[sqrt, (]` |
| `)` | `[16, sqrt]` | `[]` (нашли `(`, выкинули; sqrt тоже в output) |
| `+` | `[16, sqrt]` | `[+]` |
| `1` | `[16, sqrt, 1]` | `[+]` |
| (конец) | `[16, sqrt, 1, +]` | `[]` |

### Удобный фасад

Если нужно «строка → сразу RPN» одним вызовом:

```cpp
#include "parser/parser.h"
auto rpn = parser::parse("2+3*4");   // = shunting_yard::to_rpn(tokenizer::tokenize(s))
```

---

## 9. Вычислитель RPN

**Файлы:** `src/eval/evaluator.{h,cpp}`, `src/eval/functions.{h,cpp}`, `src/eval/operators.{h,cpp}`. **Namespace:** `eval`.

### Разделение на 3 файла

| Файл | Что внутри |
|---|---|
| `functions.cpp` | `apply_ln`, `apply_exp`, `apply_sqrt`, `apply_abs` + диспатчер `apply(name, x)` по имени |
| `operators.cpp` | `apply_binary(op, a, b)` для `+ - * /`, `apply_unary(op, a)` для `u-` |
| `evaluator.cpp` | `evaluate(rpn)` — главный цикл со стеком, делегирует в `functions` и `operators` |

Каждая функция-операция — отдельная маленькая функция. Например, `apply_sqrt`:

```cpp
double apply_sqrt(double x) {
    if (x < 0.0) throw EvalError("sqrt от отрицательного числа");
    return std::sqrt(x);
}
```

### Главный алгоритм (`evaluate`)

```
заводим пустой стек чисел
для каждого токена в RPN:
    если число:
        кладём на стек
    если бинарный оператор (+, -, *, /):
        снимаем 2 числа: b (верхнее), a
        результат = operators::apply_binary(op, a, b), кладём
    если унарный минус (u-):
        снимаем 1 число
        результат = operators::apply_unary("u-", a), кладём
    если функция (ln/exp/sqrt/abs):
        снимаем 1 число
        результат = functions::apply(name, a), кладём
в конце на стеке должно остаться ровно 1 число — это ответ
```

### Пример: вычисляем `2 3 4 * +` (= `2+3*4`)

| Шаг | Токен | Стек до | Действие | Стек после |
|---|---|---|---|---|
| 1 | 2 | `[]` | push | `[2]` |
| 2 | 3 | `[2]` | push | `[2, 3]` |
| 3 | 4 | `[2, 3]` | push | `[2, 3, 4]` |
| 4 | `*` | `[2, 3, 4]` | b=4, a=3, push 3*4 | `[2, 12]` |
| 5 | `+` | `[2, 12]` | b=12, a=2, push 2+12 | `[14]` |

**Ответ: 14.**

### Важно про порядок операндов

При снятии двух операндов **верхний — это правый**:

```cpp
double b = st.top(); st.pop();   // правый операнд
double a = st.top(); st.pop();   // левый операнд
double r = operators::apply_binary(op, a, b);
```

Это критично для `-` и `/`: `10 3 -` это `10 - 3 = 7`, а не `3 - 10 = -7`.

### Защита от переполнения

После каждой операции прогоняем результат через `check_finite(r)`:

```cpp
static double check_finite(double r) {
    if (!std::isfinite(r)) throw EvalError("Переполнение");
    return r;
}
```

Это ловит inf/nan от `exp(1000)`, `1/0` (если бы не было явной проверки) и т.п.

### Удобный фасад

```cpp
#include "eval/evaluator.h"
double r = eval::evaluate("2*sqrt(9)+abs(-3)");   // = evaluate(parser::parse(s))
```

---

## 10. Обработка ошибок

Используем 2 класса исключений:

| Класс | Файл | Где бросается | Примеры |
|---|---|---|---|
| `parser::ParseError` | `src/parser/parse_error.h` | в `tokenizer` и `shunting_yard` (синтаксис) | `2++3`, `(2+3`, `foo(2)` |
| `eval::EvalError` | `src/eval/eval_error.h` | в `functions`, `operators`, `evaluator` (математика) | `1/0`, `ln(0)`, `sqrt(-4)` |

Оба наследуются от `std::runtime_error`, поэтому в `MainWindow::compute()` ловим оба одним `catch`:

```cpp
try {
    double r = eval::evaluate(expr.toStdString());
    show_result(format::format_result(r), false);
} catch (const std::exception& e) {
    show_result(QString::fromStdString(e.what()), true);
}
```

### Полный список обрабатываемых ошибок

| Ввод | Сообщение | Где бросается |
|---|---|---|
| `1/0` | Деление на ноль | `eval::operators::apply_binary` |
| `ln(0)`, `ln(-2)` | ln от неположительного числа | `eval::functions::apply_ln` |
| `sqrt(-4)` | sqrt от отрицательного числа | `eval::functions::apply_sqrt` |
| `2++3` | Двойной оператор: ++ | `parser::tokenizer::read_operator` |
| `(2+3` | Несбалансированные скобки: лишняя '(' | `parser::shunting_yard::to_rpn` |
| `2+3)` | Несбалансированные скобки: лишняя ')' | `parser::shunting_yard::to_rpn` |
| `(пусто)` | Пустое выражение | `parser::tokenizer::tokenize` |
| `foo(2)` | Неизвестная функция: foo | `parser::tokenizer::read_function` |
| `2..3` | Некорректное число: две точки | `parser::tokenizer::read_number` |
| `2+` | Недостаточно операндов для + | `eval::evaluator::apply_operator` |
| `*5` | Двойной оператор / нет операнда | `parser::tokenizer::read_operator` |
| `exp(1000)` | Переполнение | `eval::evaluator::check_finite` |

---

## 11. GUI: устройство Qt

**Файлы:** `src/ui/main_window.{h,cpp}`, `src/ui/button_layout.{h,cpp}`, `src/ui/format.{h,cpp}`, `src/main.cpp`. **Namespace:** `ui`.

### Точка входа

```cpp
// src/main.cpp
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);     // обязательно для любого Qt-приложения
    ui::MainWindow w;
    w.show();
    return app.exec();                // запуск цикла событий Qt
}
```

`app.exec()` — это бесконечный цикл, который слушает события (клики, нажатия клавиш) и вызывает обработчики. Завершается, когда закрыли окно.

### Иерархия виджетов

```
QMainWindow (ui::MainWindow)
└── QWidget central                       (центральный контейнер)
    └── QVBoxLayout main_layout           (вертикальная компоновка)
        ├── QWidget displayPanel          (тёмная панель дисплея)
        │   └── QVBoxLayout display_layout
        │       ├── QLabel history_label_   (предыдущее выражение, серым)
        │       ├── QLineEdit expression_edit_ (текущий ввод, белым)
        │       └── QLabel result_label_  (результат, зелёным или красным)
        └── QGridLayout grid              (сетка кнопок 7×4)
            ├── QPushButton "C"
            ├── QPushButton "⌫"
            ├── ...
            └── QPushButton "="           (большая, занимает всю строку)
```

### Кнопки как данные (`button_layout.cpp`)

Все кнопки описаны массивом `ButtonSpec`:

```cpp
struct ButtonSpec {
    QString label;          // что написано на кнопке
    int row, col;           // координаты в сетке
    int row_span, col_span; // сколько ячеек занимает
    ButtonKind kind;        // стилевая категория для QSS (Digit/Operator/...)
    ButtonAction action;    // что делать при клике (Digit/Operator/Function/...)
};

const std::vector<ButtonSpec>& button_specs();   // полный список кнопок
```

Это позволяет добавлять/перемещать кнопки, не трогая логику окна.

### Сигналы и слоты — как кнопка узнаёт о клике

Это ключевой механизм Qt. Когда пользователь кликает кнопку, Qt **испускает сигнал** `clicked()`. Мы подключаем сигнал к слоту через `QSignalMapper` — он позволяет одному слоту обрабатывать клики со всех кнопок:

```cpp
auto* mapper = new QSignalMapper(this);
for (size_t i = 0; i < specs.size(); ++i) {
    auto* btn = make_button(specs[i]);
    connect(btn, &QPushButton::clicked, mapper, qOverload<>(&QSignalMapper::map));
    mapper->setMapping(btn, static_cast<int>(i));   // привязываем индекс
}
connect(mapper, &QSignalMapper::mappedInt, this, &MainWindow::handle_action);
```

Слот `handle_action(int action_index)` смотрит в `button_specs()[action_index]` и по полю `action` решает, что делать:

```cpp
void MainWindow::handle_action(int action_index) {
    const auto& spec = button_specs()[action_index];
    switch (spec.action) {
        case ButtonAction::Digit:     append(spec.label);                              break;
        case ButtonAction::Operator:  append(format::operator_to_internal(spec.label)); break;
        case ButtonAction::Function:  append(spec.label + "(");                        break;
        case ButtonAction::Paren:     append(spec.label);                              break;
        case ButtonAction::Dot:       append(".");                                     break;
        case ButtonAction::Clear:     clear_display();                                 break;
        case ButtonAction::Backspace: backspace();                                     break;
        case ButtonAction::Equals:    compute();                                       break;
    }
}
```

### Отображение операторов (`format.cpp`)

В коде хранятся ASCII-символы (`/`, `*`, `-`), но кнопки показывают красивые символы (`÷`, `×`, `−`). Маппинг — в `ui::format::operator_to_internal`:

```cpp
QString operator_to_internal(const QString& display) {
    if (display == "÷") return "/";
    if (display == "×") return "*";
    if (display == "−") return "-";
    return display;
}
```

В строку выражения попадает ASCII-вариант — парсер понимает только его.

### Форматирование результата (`format.cpp`)

Целые числа печатаем без точки, дробные — с точностью 12:

```cpp
QString format_result(double r) {
    if (r == static_cast<long long>(r) && std::abs(r) < 1e15) {
        return QString::number(static_cast<long long>(r));
    }
    return QString::number(r, 'g', 12);
}
```

### Обработка клавиатуры

Метод `keyPressEvent()` переопределён — Qt его вызывает на каждое нажатие клавиши:

```cpp
void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (key == Qt::Key_Return)    compute();        // Enter = "="
    else if (key == Qt::Key_Escape)    clear_display();  // Esc = "C"
    else if (key == Qt::Key_Backspace) backspace();
    // цифры, операторы, буквы — добавить в строку
}
```

---

## 12. GUI: стилизация QSS

**Файл:** `src/ui/theme.{h,cpp}`. Стиль — отдельная функция `ui::theme::stylesheet()`, чтобы не засорять `main_window.cpp` многострочным литералом.

QSS = **Qt Style Sheets**, синтаксис почти как CSS в браузере. Применяем через `setStyleSheet(theme::stylesheet())`.

### Цветовая палитра (Catppuccin Mocha — популярная тёмная тема)

| Цвет | HEX | Где используется |
|---|---|---|
| Фон окна | `#1e1e2e` | central |
| Фон дисплея | `#181825` | displayPanel |
| Текст | `#cdd6f4` | expression_edit_ |
| Зелёный (результат) | `#a6e3a1` | result_label_ |
| Красный (ошибка) | `#f38ba8` | result_label_ при error |
| Серый (история) | `#6c7086` | history_label_ |
| Цифры | `#313244` фон / `#cdd6f4` текст | кнопки 0-9, . |
| Операторы | `#f9e2af` (жёлтый) | +, -, *, / |
| Функции | `#89b4fa` (синий) | ln, exp, sqrt, abs |
| Действия | `#585b70` фон / `#f5c2e7` текст | C, ⌫, (, ) |
| Equals | `#a6e3a1` (зелёный) | = |

### Группировка через property selector

Каждой кнопке выставляем свойство `kind` в `make_button()`:

```cpp
btn->setProperty("kind", "digit");   // или "operator", "function", "action", "equals"
```

В QSS используем селектор по свойству:

```css
QPushButton[kind="digit"] {
    background-color: #313244;
    color: #cdd6f4;
}
QPushButton[kind="digit"]:hover {
    background-color: #45475a;
}
```

Это позволяет стилизовать группы кнопок без присвоения каждой уникального имени.

### Обновление стиля по событию (ошибка)

Когда выражение даёт ошибку, мы выставляем `error=true` на `result_label_`. Чтобы Qt **перерисовал** виджет с новым стилем, нужно явно вызвать `unpolish` + `polish`:

```cpp
void MainWindow::show_result(const QString& text, bool is_error) {
    result_label_->setText(text);
    result_label_->setProperty("error", is_error);
    result_label_->style()->unpolish(result_label_);
    result_label_->style()->polish(result_label_);
}
```

Без этого Qt будет кешировать старый стиль и цвет не сменится.

---

## 13. Как добавить новую функцию (например, `cos`)

Допустим, хотим добавить `cos(x)`. Нужно изменить **4 места**:

### 1. Зарегистрировать имя в токенизаторе

`src/parser/tokenizer.cpp`, функция `is_known_function`:

```cpp
static const std::unordered_set<std::string> funcs = {"ln", "exp", "sqrt", "abs", "cos"};
```

### 2. Реализовать вычисление

`src/eval/functions.h` — объявить:

```cpp
double apply_cos(double x);
```

`src/eval/functions.cpp` — реализовать и добавить в диспатчер:

```cpp
double apply_cos(double x) { return std::cos(x); }

double apply(const std::string& name, double x) {
    if (name == "ln")   return apply_ln(x);
    if (name == "exp")  return apply_exp(x);
    if (name == "sqrt") return apply_sqrt(x);
    if (name == "abs")  return apply_abs(x);
    if (name == "cos")  return apply_cos(x);
    throw EvalError("Неизвестная функция: " + name);
}
```

### 3. Добавить кнопку

`src/ui/button_layout.cpp`, в массив `specs` (например рядом с другими функциями):

```cpp
{"cos", 1, 4, 1, 1, K::Function, A::Function},
```

Возможно придётся передвинуть колонки или добавить ряд. Сетка `grid_cols`/`grid_rows` в `button_layout.h` тоже может потребовать обновления.

### 4. Добавить тест

`tests/parser_tests.cpp`:

```cpp
check_eq("cos(0)", 1);
check_eq("cos(3.14159265)", -1, 1e-7);
```

Пересобрать: `cmake --build build -j` → `ctest --output-on-failure`.

---

## 14. Как добавить новую кнопку

Кнопки описаны массивом в `src/ui/button_layout.cpp`:

```cpp
static const std::vector<ButtonSpec> specs = {
    {"C",     0, 0, 1, 1, K::Action,   A::Clear},
    {"⌫",    0, 1, 1, 1, K::Action,   A::Backspace},
    ...
};
```

Поля `ButtonSpec`:
- `label` — что написано на кнопке (можно юникод: `÷`, `√`, ...)
- `row`, `col` — координаты в сетке (0,0 — верхний левый угол)
- `row_span`, `col_span` — сколько ячеек занимает (обычно 1×1, у `=` 1×4, у `0` 1×2)
- `kind` — стилевая группа: `Digit` / `Operator` / `Function` / `Action` / `Equals`
- `action` — поведение при клике: `Digit` / `Operator` / `Function` / `Paren` / `Dot` / `Clear` / `Backspace` / `Equals`

Просто добавь строку в массив. Layout перестроится автоматически. Если используешь юникод-символ оператора — добавь маппинг в `src/ui/format.cpp`.

---

## 15. Как добавить новый тест

`tests/parser_tests.cpp` — есть 2 хелпера:

```cpp
check_eq("выражение", ожидаемое_число);             // должно вычислиться
check_eq("выражение", ожидаемое, 1e-7);              // с точностью
check_throws("выражение", "почему ожидаем ошибку");  // должно бросить исключение
```

Пример:

```cpp
check_eq("2*sqrt(9)+abs(-3)", 9);
check_throws("ln(0)", "ln от 0");
```

Пересобрать тесты: `cmake --build build --target parser_tests -j`
Запустить: `./build/parser_tests`

---

## 16. FAQ — частые вопросы

### Q: При сборке: `Qt6 not found`

```bash
cmake -B build -S . -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt
```

### Q: Программа собралась, но не запускается

```bash
# проверь что бандл на месте:
ls -la ./build/calculator.app

# запусти из терминала чтобы увидеть ошибки:
./build/calculator.app/Contents/MacOS/calculator
```

### Q: Тесты падают, что делать

Запусти `./build/parser_tests` напрямую — каждый тест печатает `[OK]` или `[FAIL] выражение = реальное, expected ожидаемое`. По строке `[FAIL]` сразу видно что сломано.

### Q: Я хочу запустить только один конкретный тест

Тесты сейчас простые — все 41 в одном `main()`. Чтобы запустить один — закомментируй остальные `check_eq` / `check_throws` в `tests/parser_tests.cpp`.

### Q: Как посмотреть какие токены парсер вернул для моего выражения

Добавь временно в `MainWindow::compute()`:

```cpp
auto tokens = parser::tokenizer::tokenize(expr.toStdString());
for (const auto& t : tokens) std::cout << t.text << " ";
std::cout << std::endl;
auto rpn = parser::shunting_yard::to_rpn(tokens);
for (const auto& t : rpn) std::cout << t.text << " ";
std::cout << std::endl;
```

И запускай программу из терминала — вывод увидишь там.

### Q: У меня IDE подчёркивает красным `#include <QMainWindow>` или `#include "parser/token.h"`

Это нормально, если у тебя в IDE не настроен путь к Qt и к `src/`. На сборку через `cmake` это не влияет. В CLion / Qt Creator открывай проект через `CMakeLists.txt` — IDE сам подхватит пути.

### Q: Почему в коде `*`, `/`, `-`, а на кнопках `×`, `÷`, `−`?

Парсер работает с ASCII (быстро, надёжно). Юникод-символы — только для красоты на кнопках. Маппинг см. в `ui::format::operator_to_internal` (`src/ui/format.cpp`).

### Q: Что такое `Q_OBJECT` в `main_window.h`?

Макрос Qt, обязательный для классов с сигналами/слотами. Без него `connect()` не будет работать. Не удаляй.

### Q: Почему парсер и вычислитель — функции в namespace, а GUI — класс?

Парсер и вычислитель **не имеют состояния** (на вход данные → на выход данные). Заворачивать их в класс не имеет смысла — это просто папка для функций. Поэтому используем namespace.

GUI **обязан быть классом** — Qt требует наследования от `QMainWindow` и макроса `Q_OBJECT` для системы сигналов/слотов. Класс держим тонким, всю реальную работу делегируем в `eval::evaluate()`, `format::*`, `theme::*`.

### Q: Где QSS-стили?

В `src/ui/theme.cpp`, функция `stylesheet()`. Палитра — Catppuccin Mocha (см. таблицу в разделе 12).

---

## 17. Глоссарий

- **Токен** — атомарная единица выражения: число, оператор, скобка, имя функции.
- **Токенизация (лексический анализ)** — разбиение строки на токены. У нас — `parser::tokenizer::tokenize`.
- **Парсинг (синтаксический анализ)** — превращение токенов в форму, удобную для вычисления (у нас — в RPN). У нас — `parser::shunting_yard::to_rpn`.
- **RPN (Reverse Polish Notation, обратная польская запись)** — запись выражения, где операторы стоят после операндов. Без скобок. Вычисляется одним проходом по стеку.
- **Shunting-yard algorithm** — алгоритм Дейкстры для преобразования инфиксной записи в RPN. Назван «алгоритм сортировочной станции» по аналогии с железнодорожной сортировочной горкой.
- **Стек (LIFO)** — структура данных «последним пришёл — первым вышел». В C++ — `std::stack`.
- **Унарный оператор** — оператор с одним операндом (`-5` — унарный минус).
- **Бинарный оператор** — оператор с двумя операндами (`5-3` — бинарное вычитание).
- **Ассоциативность** — порядок группировки при равных приоритетах. Левая: `1+2+3` = `(1+2)+3`. Правая: степень `2^3^2` = `2^(3^2)`.
- **Приоритет (precedence)** — кто вычисляется раньше. У `*` приоритет выше чем у `+`.
- **Namespace** — пространство имён. Функция `parser::tokenizer::tokenize` лежит в namespace `parser::tokenizer`. Не путать с классом — namespace не создаёт объект, это просто префикс к именам.
- **QSS** — Qt Style Sheets, аналог CSS для Qt-виджетов.
- **Сигнал** — событие в Qt (например, кнопка испускает `clicked()` при клике).
- **Слот** — метод-обработчик, подключённый к сигналу через `connect()`.
- **`QSignalMapper`** — Qt-утилита, позволяющая привязать много сигналов к одному слоту с числовым параметром (мы используем для всех кнопок).
- **CMake** — система сборки. По `CMakeLists.txt` генерирует Makefile или Xcode-проект.

---

## Контакты

- Вопросы по коду / сборке → **Гаценко Александр**
- Вопросы по отчёту → **Шеянов Ярослав**
- Преподаватель: **Полтавский Дмитрий Сергеевич**, кабинет **440**
