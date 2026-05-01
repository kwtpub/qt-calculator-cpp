#pragma once

#include <string>

namespace eval::functions {

double apply_ln(double x);
double apply_exp(double x);
double apply_sqrt(double x);
double apply_abs(double x);

double apply(const std::string& name, double x);

}
