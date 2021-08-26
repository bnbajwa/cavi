//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CONVAR_TYPES_HEADER
#define CONVAR_TYPES_HEADER

#include <string>
#include <cstdint>

namespace convar {

template<template <typename> class HYB>
struct var : HYB<var<HYB>> {
protected:
    var() = default;
    var(const var&) = default;
    var(var&&) = default;
};

template<template <typename> class HYB>
struct var_string : var<HYB>, HYB<var_string<HYB>> {
    std::string value;
    var_string(std::string val) : value(std::move(val)) {}
};

template<template <typename> class HYB>
struct var_vector2 : var<HYB>, HYB<var_vector2<HYB>> {
    float x, y;
    var_vector2(float x_, float y_) : x(x_), y(y_) {}
};

template<template <typename> class HYB>
struct var_arith : var<HYB>, HYB<var_arith<HYB>> {
protected:
    var_arith() = default;
    var_arith(const var_arith&) = default;
    var_arith(var_arith&&) = default;
};

template<template <typename> class HYB>
struct var_u32 : var_arith<HYB>, HYB<var_u32<HYB>> {
    uint32_t value;
    var_u32(uint32_t val) : value(val) {}
};

template<template <typename> class HYB>
struct var_flt : var_arith<HYB>, HYB<var_flt<HYB>> {
    float value;
    var_flt(float val) : value(val) {}
};

}

#endif
