//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "var.h"
#include "convar/convar_lib.h"
#include <iostream>

namespace myns {
void print_var_impl(const var_bool& v) {
    const char* vstr = v.value ? "true" : "false";
    std::cout << vstr;
}
}

int main() {
    myns::var_flt v2{3.14f};
    myns::var_u32 v1{150};
    myns::var_string v3{"Hello World"};
    myns::var_vector2 v4{10, 15};
    myns::var_bool v5{true};

    convar::print_var(v1);
    convar::print_var(v2);
    convar::print_var(v3);
    convar::print_var(v4);
    convar::print_var(v5);

    return 0;
}
