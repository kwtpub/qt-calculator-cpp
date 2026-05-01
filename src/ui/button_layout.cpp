#include "ui/button_layout.h"

namespace ui {

const std::vector<ButtonSpec>& button_specs() {
    using K = ButtonKind;
    using A = ButtonAction;
    // Раскладка 7×4. Кнопка "0" растянута на 2 колонки, "=" — на всю ширину.
    // Юникод-символы ÷ × − на кнопках конвертируются в ASCII /, *, -
    // через format::operator_to_internal перед попаданием в выражение.
    static const std::vector<ButtonSpec> specs = {
        {"C",     0, 0, 1, 1, K::Action,   A::Clear},
        {"⌫",    0, 1, 1, 1, K::Action,   A::Backspace},
        {"(",     0, 2, 1, 1, K::Action,   A::Paren},
        {")",     0, 3, 1, 1, K::Action,   A::Paren},

        {"ln",    1, 0, 1, 1, K::Function, A::Function},
        {"exp",   1, 1, 1, 1, K::Function, A::Function},
        {"sqrt",  1, 2, 1, 1, K::Function, A::Function},
        {"abs",   1, 3, 1, 1, K::Function, A::Function},

        {"7",     2, 0, 1, 1, K::Digit,    A::Digit},
        {"8",     2, 1, 1, 1, K::Digit,    A::Digit},
        {"9",     2, 2, 1, 1, K::Digit,    A::Digit},
        {"÷",     2, 3, 1, 1, K::Operator, A::Operator},

        {"4",     3, 0, 1, 1, K::Digit,    A::Digit},
        {"5",     3, 1, 1, 1, K::Digit,    A::Digit},
        {"6",     3, 2, 1, 1, K::Digit,    A::Digit},
        {"×",     3, 3, 1, 1, K::Operator, A::Operator},

        {"1",     4, 0, 1, 1, K::Digit,    A::Digit},
        {"2",     4, 1, 1, 1, K::Digit,    A::Digit},
        {"3",     4, 2, 1, 1, K::Digit,    A::Digit},
        {"−",     4, 3, 1, 1, K::Operator, A::Operator},

        {"0",     5, 0, 1, 2, K::Digit,    A::Digit},
        {".",     5, 2, 1, 1, K::Digit,    A::Dot},
        {"+",     5, 3, 1, 1, K::Operator, A::Operator},

        {"=",     6, 0, 1, 4, K::Equals,   A::Equals},
    };
    return specs;
}

}
