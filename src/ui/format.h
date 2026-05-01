#pragma once

#include <QString>

namespace ui::format {

QString operator_to_internal(const QString& display);
QString format_result(double r);

}
