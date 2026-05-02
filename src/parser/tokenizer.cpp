// =====================================================================
// tokenizer.cpp — реализация лексического анализатора.
// Алгоритм: один линейный проход по строке, диспетчеризация по
// первому символу очередного токена.
// =====================================================================

#include "parser/tokenizer.h"
#include "parser/parse_error.h"   // для бросания ParseError при ошибках

// <cctype> — std::isdigit, std::isalpha, std::isspace.
// Эти функции работают с int и не любят char напрямую — поэтому
// ниже везде делаем static_cast<unsigned char>(c).
#include <cctype>

// unordered_set нужен для O(1)-проверки имени функции
// (в is_known_function).
#include <unordered_set>

namespace parser::tokenizer {

// Проверка имени функции по таблице поддерживаемых.
bool is_known_function(const std::string& name) {
    // static — таблица создаётся один раз при первом вызове функции,
    // дальше переиспользуется. Без static она пересоздавалась бы
    // на каждом вызове, что заметно медленнее в горячем цикле.
    static const std::unordered_set<std::string> funcs = {"ln", "exp", "sqrt", "abs"};

    // count() возвращает 0 или 1 (потому что в set'е элементы уникальны).
    // > 0 → имя есть в множестве.
    return funcs.count(name) > 0;
}

// ---------------------------------------------------------------------
// Чтение числа, начиная с позиции i.
// После работы i указывает на первый символ ПОСЛЕ числа.
// Допускает не более одной точки. Поддерживает "42", "3.14", ".5".
// static — функция видна только в этом .cpp (внутренний помощник).
// ---------------------------------------------------------------------
static Token read_number(const std::string& expression, size_t& i) {
    const size_t n = expression.size();   // длина строки — кэшируем
    const size_t start = i;               // запомним стартовую позицию
    bool has_dot = false;                 // встречалась ли точка

    // Идём вперёд, пока символ — цифра или точка.
    while (i < n && (std::isdigit(static_cast<unsigned char>(expression[i])) || expression[i] == '.')) {
        if (expression[i] == '.') {
            // Вторая точка в числе — синтаксическая ошибка.
            if (has_dot) throw ParseError("Некорректное число: две точки");
            has_dot = true;
        }
        ++i;
    }

    // Вырезаем подстроку — само число как текст.
    std::string num = expression.substr(start, i - start);

    // Защита от случая ".": одна точка без цифр — это не число.
    if (num == ".") throw ParseError("Некорректное число: одиночная точка");

    // std::stod парсит строку в double. Бросит исключение, если внутри
    // что-то совсем плохое, но мы уже отфильтровали все странные случаи.
    return {TokenType::Number, num, std::stod(num)};
}

// ---------------------------------------------------------------------
// Чтение имени функции (последовательность букв) и проверка, что оно
// есть в таблице поддерживаемых.
// ---------------------------------------------------------------------
static Token read_function(const std::string& expression, size_t& i) {
    const size_t n = expression.size();
    const size_t start = i;

    // Буквы подряд — имя функции. Цифры и подчёркивания не допускаем,
    // потому что их в названиях наших функций нет.
    while (i < n && std::isalpha(static_cast<unsigned char>(expression[i]))) ++i;

    std::string name = expression.substr(start, i - start);

    // Если имя не из списка — ошибка. Без этой проверки токенизатор
    // спокойно дошёл бы до evaluate, и пользователь получил бы менее
    // понятное сообщение от диспетчера функций.
    if (!is_known_function(name)) throw ParseError("Неизвестная функция: " + name);

    return {TokenType::Function, name, 0.0};
}

// ---------------------------------------------------------------------
// Решает, является ли минус УНАРНЫМ в текущем контексте.
// Унарный, если перед ним:
//   - ничего (минус в начале выражения),
//   - оператор ("2 * -3"),
//   - открывающая скобка ("-(1+2)"),
//   - функция ("sqrt(-4)").
// Иначе минус — бинарный (вычитание).
// ---------------------------------------------------------------------
static bool is_unary_minus_context(const std::vector<Token>& tokens) {
    // Если токенов ещё нет — это самое начало выражения.
    if (tokens.empty()) return true;

    const Token& prev = tokens.back();   // предыдущий токен
    return prev.type == TokenType::Operator
        || prev.type == TokenType::LeftParen
        || prev.type == TokenType::Function;
}

// ---------------------------------------------------------------------
// Создаёт токен оператора, в т.ч. распознаёт унарный минус и проверяет
// случай двойного оператора ("2++3" → ошибка).
// ---------------------------------------------------------------------
static Token read_operator(char c, const std::vector<Token>& tokens) {
    // Превращаем char в std::string из одного символа.
    std::string op(1, c);

    // Минус в "унарном контексте" получает специальное имя "u-",
    // чтобы shunting_yard и evaluator могли отличить его от бинарного "-".
    if (c == '-' && is_unary_minus_context(tokens)) {
        return {TokenType::Operator, "u-", 0.0};
    }

    // Если предыдущий токен — тоже бинарный оператор, значит идут подряд:
    // "2++3", "5*/2" и т.п. (Унарный минус "u-" сюда не попадает —
    // он легитимно может стоять после оператора.)
    if (!tokens.empty() && tokens.back().type == TokenType::Operator && tokens.back().text != "u-") {
        throw ParseError(std::string("Двойной оператор: ") + tokens.back().text + op);
    }

    return {TokenType::Operator, op, 0.0};
}

// =====================================================================
// Главный цикл токенизатора.
// =====================================================================
std::vector<Token> tokenize(const std::string& expression) {
    std::vector<Token> tokens;     // выходной массив токенов
    size_t i = 0;                  // текущая позиция в строке
    const size_t n = expression.size();

    // Идём по строке. На каждом шаге i может сдвигаться разными
    // подфункциями (read_number сама двигает i до конца числа и т.д.),
    // поэтому в цикле НЕТ универсального ++i — каждая ветка сама
    // отвечает за продвижение.
    while (i < n) {
        char c = expression[i];

        // Пробелы просто пропускаем — они для пользователя, не для парсера.
        if (std::isspace(static_cast<unsigned char>(c))) { ++i; continue; }

        // Цифра или точка — начало числа. read_number дочитает до конца.
        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            tokens.push_back(read_number(expression, i));
            continue;
        }

        // Буква — начало имени функции.
        if (std::isalpha(static_cast<unsigned char>(c))) {
            tokens.push_back(read_function(expression, i));
            continue;
        }

        // Скобки — однозначные одиночные токены.
        if (c == '(') { tokens.push_back({TokenType::LeftParen,  "(", 0.0}); ++i; continue; }
        if (c == ')') { tokens.push_back({TokenType::RightParen, ")", 0.0}); ++i; continue; }

        // Один из четырёх операторов. read_operator решит, унарный ли минус.
        if (c == '+' || c == '-' || c == '*' || c == '/') {
            tokens.push_back(read_operator(c, tokens));
            ++i;
            continue;
        }

        // Дошли сюда — символ не подпал ни под одну категорию.
        // Это какой-то посторонний знак ('@', '$', '?' и т.п.).
        throw ParseError(std::string("Неизвестный символ: ") + c);
    }

    // Защита от пустого ввода (или строки только из пробелов).
    if (tokens.empty()) throw ParseError("Пустое выражение");

    return tokens;
}

}  // namespace parser::tokenizer
