//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CONVAR_LIB_HEADER
#define CONVAR_LIB_HEADER

#include "cavi/visit.h"
#include "convar_types.h"
#include <cassert>
#include <iostream>

namespace convar {
template<
    typename T,
    template <template <typename> class> class TPL>
struct is_specialization_of_impl {
    static constexpr bool value = false;
};

template<
    template <typename> class HB,
    template <template <typename> class> class TPL>
struct is_specialization_of_impl<TPL<HB>, TPL> {
    static constexpr bool value = true;
};

template<
    typename T,
    template <template <typename> class> class TPL>
inline constexpr bool is_specialization_of_v =
is_specialization_of_impl<cavi::remove_cv_t<T>, TPL>::value;

template<
    typename T,
    template <template <typename> class> class... TPLs>
inline constexpr bool is_specialization_of_any_of_v = (is_specialization_of_v<T, TPLs> || ...);

template<typename TV>
void print_var_impl(const TV& v) {
    using T = cavi::remove_volatile_t<TV>;
    if constexpr(is_specialization_of_any_of_v<T, var, var_arith>) {
        assert(false);
    }
    else if constexpr(is_specialization_of_v<T, var_vector2>) {
        std::cout << '(' << v.x << ", " << v.y << ")\n";
    }
    else if constexpr(is_specialization_of_any_of_v<T, var_string, var_u32, var_flt>) {
        std::cout << v.value << '\n';
    }
    else {
        static_assert(cavi::dependent_false<T>, "unhandled type");
    }
}

template<template <typename> class HB>
void print_var(const var<HB>& vr) {
    cavi::visit([](const auto& v) {
        print_var_impl(v);
    }, vr);
}

}

#endif
