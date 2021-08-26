//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef VAR_HEADER
#define VAR_HEADER

#include "var_HY.h"
#include "convar/convar_types.h"

namespace myns {
struct var_bool : var_arith, var_HYBext<var_bool> {
    bool value;
    var_bool(bool v) : value(v) {}
};
}

#endif
