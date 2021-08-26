//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef VARIABLE_HEADER
#define VARIABLE_HEADER

#include "var_HY.h"

#include <string>
#include <cstdint>

struct var : var_HYB<var> {
protected:
    var() = default;
    var(const var&) = default;
    var(var&&) = default;
};

struct var_string : var, var_HYB<var_string> {
    std::string value;
    var_string(std::string val) : value(std::move(val)) {}
};

struct var_vector2 : var, var_HYB<var_vector2> {
    float x, y;
    var_vector2(float x_, float y_) : x(x_), y(y_) {}
};

struct var_arith : var, var_HYB<var_arith> {
protected:
    var_arith() = default;
    var_arith(const var_arith&) = default;
    var_arith(var_arith&&) = default;
};

struct var_u32 : var_arith, var_HYB<var_u32> {
    uint32_t value;
    var_u32(uint32_t val) : value(val) {}
};

struct var_flt : var_arith, var_HYB<var_flt> {
    float value;
    var_flt(float val) : value(val) {}
};

#endif
