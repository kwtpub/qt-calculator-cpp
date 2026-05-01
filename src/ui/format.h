#pragma once

// Утилиты форматирования для GUI: преобразование красивых символов
// в ASCII (нужно для парсера) и человекочитаемый вывод результата.

#include <QString>

namespace ui::format {

// ÷ → /, × → *, − → -. Остальное возвращает как есть.
QString operator_to_internal(const QString& display);

// Целые числа печатаем без точки (5 вместо 5.000000),
// дробные — с точностью до 12 значащих цифр.
QString format_result(double r);

}
