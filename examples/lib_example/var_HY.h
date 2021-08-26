//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef VAR_HY_HEADER
#define VAR_HY_HEADER

#include "convar/convar_hierarchy.h"
namespace myns {
// forward declaration of base
template<typename>
class var_HYBext;

using var = convar::var<var_HYBext>;
using var_string = convar::var_string<var_HYBext>;
using var_vector2 = convar::var_vector2<var_HYBext>;
using var_arith = convar::var_arith<var_HYBext>;
using var_u32 = convar::var_u32<var_HYBext>;
using var_flt = convar::var_flt<var_HYBext>;

using var_HYext = cavi::extend_hierarchy_t<
    convar::convar_hierarchy,
    cavi::class_<struct var_bool, var_arith>
>;

CAVI_DEF_BAREBONES_BASE(var_HYext, var_HYBext)
};

#endif
