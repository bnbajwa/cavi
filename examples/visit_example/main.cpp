//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "var.h"
#include "cavi/visit.h"
#include <iostream>
#include <cassert>
#include <string>

void print_var(const var& vr) {
    cavi::visit([](const auto& v) {
        using T = cavi::remove_cvref_t<decltype(v)>;
        if constexpr(cavi::is_same_as_any_of_v<T, var, var_arith>) {
            // var & var_arith can't be instantiated on their own - only derived classes can construct them.
            // if that changes in the future we want to catch that so this implementation can be updated too.
            // a compile time assert ensuring var & var_arith can't be instantiated would be even better if possible
            assert(false);
        }
        else if constexpr(cavi::is_same_v<T, var_vector2>) {
            std::cout << '(' << v.x << ", " << v.y << ")\n";
        }
        else if constexpr(cavi::is_same_as_any_of_v<T, var_string, var_u32, var_flt>) {
            std::cout << v.value << '\n';
        }
        else {
            static_assert(cavi::dependent_false<T>, "unhandled type");
        }
    }, vr);
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void print_var2(const var& vr) {
    cavi::visit(overloaded {
        [](const var&) { assert(false); },
        [](const var_arith&) { assert(false); },
        [](const var_vector2& v) { std::cout << '(' << v.x << ", " << v.y << ")\n"; },
        [](const var_string& v) { std::cout << v.value << '\n'; },
        [](const var_u32& v) { std::cout << v.value << '\n'; },
        [](const var_flt& v) { std::cout << v.value << '\n'; },
        [](auto&& v) { static_assert(cavi::dependent_false<decltype(v)>, "unhandled type"); }
    }, vr);
}

std::string arith_to_string_representation(const var_arith& r) {
    return cavi::visit(overloaded {
        [](const var_arith&) { assert(false); return std::string(""); },
        [](const var_u32& v) { return std::to_string(v.value); },
        [](const var_flt& v) { return std::to_string(v.value); },
        [](auto&& v) { static_assert(cavi::dependent_false<decltype(v)>, "unhandled type"); }
    }, r);
}

std::string arith_str_to_string_representation(const var_arith& r1, const var_string& r2) {
    return cavi::visit<std::string>(overloaded {
        [](const var_arith&, const var_string&) { 
            assert(false); 
            return "";
        },
        [](const var_u32& v1, const var_string& v2) {
            return std::string("v1: ") + std::to_string(v1.value) + std::string(", v2: ") + v2.value;
        },
        [](const var_flt& v1, const var_string& v2) {
            return std::string("v1: ") + std::to_string(v1.value) + std::string(", v2: ") + v2.value;
        },
        [](auto&& v1, auto&& v2) {
            static_assert(cavi::dependent_false<decltype(v1)>, "unhandled type");
        }
    }, r1, r2);
}

int main() {
    var_u32 v1{150};
    var_flt v2{3.14f};
    var_string v3{"Hello World"};
    var_vector2 v4{1.5f, 3.0f};

    print_var(v1);
    print_var(v2);
    print_var(v3);
    print_var(v4);

    std::cout << '\n';
    print_var2(v1);
    print_var2(v2);
    print_var2(v3);
    print_var2(v4);

    std::cout << '\n';
    std::cout << arith_to_string_representation(v1) << '\n';
    std::cout << arith_to_string_representation(v2) << '\n';

    std::cout << '\n';
    std::cout << arith_str_to_string_representation(v1, v3) << '\n';
    std::cout << arith_str_to_string_representation(v2, v3) << '\n';

    return 0;
}
