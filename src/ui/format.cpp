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
    // Если число фактически целое и помещается в long long — печатаем как целое.
    // Граница 1e15 — за пределами этого double теряет точность для целых.
    if (r == static_cast<long long>(r) && std::abs(r) < 1e15) {
        return QString::number(static_cast<long long>(r));
    }
    return QString::number(r, 'g', 12);
}

}
