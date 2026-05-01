#pragma once

#include <string>

namespace eval::operators {

double apply_binary(const std::string& op, double a, double b);
double apply_unary(const std::string& op, double a);

}
