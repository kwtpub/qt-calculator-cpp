// Юнит-тесты парсера и вычислителя.
// Не используют Qt — компилируются и запускаются автономно через ctest.
// Каждый тест печатает [OK] или [FAIL] с подробностями.
//
// Хелперы:
//   check_eq(expr, expected)    — выражение должно дать число
//   check_throws(expr, reason)  — выражение должно бросить исключение

#include "parser.h"

#include <cmath>
#include <cstdio>
#include <string>

static int passed = 0;
static int failed = 0;

static void check_eq(const std::string& expr, double expected, double tol = 1e-9) {
    try {
        double r = calc::evaluate(expr);
        if (std::fabs(r - expected) <= tol) {
            ++passed;
            std::printf("[OK]   %s = %.12g\n", expr.c_str(), r);
        } else {
            ++failed;
            std::printf("[FAIL] %s = %.12g, expected %.12g\n", expr.c_str(), r, expected);
        }
    } catch (const std::exception& e) {
        ++failed;
        std::printf("[FAIL] %s threw: %s\n", expr.c_str(), e.what());
    }
}

static void check_throws(const std::string& expr, const std::string& reason) {
    try {
        double r = calc::evaluate(expr);
        ++failed;
        std::printf("[FAIL] %s should throw (%s), got %.12g\n", expr.c_str(), reason.c_str(), r);
    } catch (const std::exception&) {
        ++passed;
        std::printf("[OK]   %s threw as expected (%s)\n", expr.c_str(), reason.c_str());
    }
}

int main() {
    std::printf("=== Базовая арифметика ===\n");
    check_eq("2+3", 5);
    check_eq("10-4", 6);
    check_eq("3*4", 12);
    check_eq("20/4", 5);
    check_eq("2+3*4", 14);
    check_eq("(2+3)*4", 20);
    check_eq("2*(3+4)*5", 70);

    std::printf("\n=== Десятичные дроби ===\n");
    check_eq("3.14+0.86", 4.0);
    check_eq("0.1+0.2", 0.3, 1e-12);
    check_eq("1.5*2", 3);

    std::printf("\n=== Унарный минус ===\n");
    check_eq("-5", -5);
    check_eq("-5+10", 5);
    check_eq("-(2+3)", -5);
    check_eq("3*-4", -12);
    check_eq("--5", 5);

    std::printf("\n=== Скобки ===\n");
    check_eq("((2+3))", 5);
    check_eq("(1+(2*(3+4)))", 15);

    std::printf("\n=== Функции варианта 2 ===\n");
    check_eq("sqrt(16)", 4);
    check_eq("sqrt(2)*sqrt(2)", 2, 1e-12);
    check_eq("abs(-7)", 7);
    check_eq("abs(3-10)", 7);
    check_eq("ln(exp(1))", 1, 1e-12);
    check_eq("exp(0)", 1);
    check_eq("ln(1)", 0);
    check_eq("sqrt(abs(-25))", 5);
    check_eq("exp(ln(5))", 5, 1e-12);

    std::printf("\n=== Композиции ===\n");
    check_eq("2*sqrt(9)+abs(-3)", 9);
    check_eq("ln(exp(2))+sqrt(4)", 4, 1e-12);
    check_eq("(1+2)*sqrt(16)-abs(-5)", 7);

    std::printf("\n=== Ошибки ===\n");
    check_throws("1/0", "деление на 0");
    check_throws("ln(0)", "ln от 0");
    check_throws("ln(-1)", "ln от отрицательного");
    check_throws("sqrt(-4)", "sqrt от отрицательного");
    check_throws("2++3", "двойной оператор");
    check_throws("(2+3", "несбалансированные скобки");
    check_throws("2+3)", "несбалансированные скобки");
    check_throws("", "пустое выражение");
    check_throws("foo(2)", "неизвестная функция");
    check_throws("2..3", "две точки");
    check_throws("2+", "недостаточно операндов");
    check_throws("*5", "оператор без левого операнда");

    std::printf("\n=== Итог ===\n");
    std::printf("Пройдено: %d, провалено: %d\n", passed, failed);
    return failed == 0 ? 0 : 1;
}
