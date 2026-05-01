#include "ui/format.h"

#include <cmath>

namespace ui::format {

QString operator_to_internal(const QString& display) {
    if (display == "÷") return "/";
    if (display == "×") return "*";
    if (display == "−") return "-";
    return display;
}

QString format_result(double r) {
    if (r == static_cast<long long>(r) && std::abs(r) < 1e15) {
        return QString::number(static_cast<long long>(r));
    }
    return QString::number(r, 'g', 12);
}

}
