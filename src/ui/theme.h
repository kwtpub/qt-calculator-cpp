#pragma once

// Тема оформления (Catppuccin Mocha) — выделена в отдельный модуль,
// чтобы main_window.cpp не разрастался QSS-литералом и можно было
// в будущем добавить переключение тем.

#include <QString>

namespace ui::theme {

// Возвращает QSS-таблицу стилей для всего окна.
QString stylesheet();

}
