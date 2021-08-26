//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef VARIABLE_HY_HEADER
#define VARIABLE_HY_HEADER

#include "cavi/core.h"

using var_HY = cavi::hierarchy_decl<
    cavi::class_<struct var>,
    cavi::class_<struct var_string, var>,
    cavi::class_<struct var_vector2, var>,
    cavi::class_<struct var_arith, var>,
    cavi::class_<struct var_u32, var_arith>,
    cavi::class_<struct var_flt, var_arith>
>;

CAVI_DEF_BAREBONES_BASE(var_HY, var_HYB)

#endif
