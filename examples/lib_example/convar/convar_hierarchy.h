//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CONVAR_HY_HEADER
#define CONVAR_HY_HEADER

#include "cavi/core.h"

namespace convar {

template<template <typename> class HYB> struct var;
template<template <typename> class HYB> struct var_string;
template<template <typename> class HYB> struct var_vector2;
template<template <typename> class HYB> struct var_vector2;
template<template <typename> class HYB> struct var_arith;
template<template <typename> class HYB> struct var_u32;
template<template <typename> class HYB> struct var_flt;

using convar_hierarchy = cavi::hierarchy_decl<
    cavi::class_tpl<var>,
    cavi::class_tpl<var_string, var>,
    cavi::class_tpl<var_vector2, var>,
    cavi::class_tpl<var_arith, var>,
    cavi::class_tpl<var_u32, var_arith>,
    cavi::class_tpl<var_flt, var_arith>
>;

}

#endif
