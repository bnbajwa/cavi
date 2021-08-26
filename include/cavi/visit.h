//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CAVI_HEADER_VISIT
#define CAVI_HEADER_VISIT

#include "core.h"

namespace cavi {
template<typename OWoR>
inline constexpr size_type num_of_alternatives_v = deduce_hierarchy_t<OWoR>::num_of_classes;

template<typename... OsWoR>
struct mapper {
    static_assert(((num_of_alternatives_v<OsWoR> > 0) && ...),
                  "all objects must have atleast one alternative");

    static constexpr size_type N = sizeof...(OsWoR);

    static constexpr array<size_type, N> num_of_alternatives_arr = {{
        num_of_alternatives_v<OsWoR>...
    }};

    static constexpr array<size_type, N> coeffs_arr = []() constexpr {
        array<size_type, N> coefficients_arr = {{ 1 }};
        size_type value = num_of_alternatives_arr[0];
        for(size_type i = 1; i < N; i++) {
            coefficients_arr[i] = value;
            value *= num_of_alternatives_arr[i];
        }
        return coefficients_arr;
    }();

    static constexpr size_type map(const array<size_type, N>& indexes_arr) {
        size_type mapped_value = 0;
        for(size_type i = 0; i < N; i++)
            mapped_value += indexes_arr[i] * coeffs_arr[i];
        return mapped_value;
    }

    static constexpr array<size_type, N> inverse(size_type mapped_value) {
        array<size_type, N> res = {};
        size_type value = mapped_value;
        for(size_type i = N-1; i < N; i--) {
            res[i] = value / coeffs_arr[i];
            value -= res[i] * coeffs_arr[i];
        }
        return res;
    }

    static constexpr size_type range_v = (num_of_alternatives_v<OsWoR> * ... * 1);
};

template<typename OWoR>
struct mapper<OWoR> {
    static_assert(num_of_alternatives_v<OWoR> > 0, "object must have atleast one alternative");
    static constexpr size_type range_v = num_of_alternatives_v<OWoR>;
    static constexpr size_type map(const array<size_type, 1>& indexes_arr) {
        return indexes_arr[0];
    }
    static constexpr array<size_type, 1> inverse(size_type mapped_value) {
        return {{ mapped_value }};
    }
};

template<size_type I, typename OWoR>
using nth_alternative_of_t = 
    typename hierarchy_utility_t<
        deduce_hierarchy_t<OWoR>
    >::template class_from_index_t<I>;

template<size_type state, typename... OsWoR>
static constexpr array<size_type, sizeof...(OsWoR)> indexes_for_v =
    mapper<OsWoR...>::inverse(state);

template<typename Is, typename... OsWoR>
struct is_state_possible;

template<size_type... Is, typename... OsWoR>
struct is_state_possible<index_sequence<Is...>, OsWoR...> {
    template<typename T>
    static constexpr array<bool, num_of_alternatives_v<T>> base_survey =
        hierarchy_utility_t<deduce_hierarchy_t<T>>::template contains_base_survey<
            T
        >;

    template<size_type state>
    static constexpr bool value = 
        ((base_survey<OsWoR>)[indexes_for_v<state, OsWoR...>[Is]] && ...);
};

template<size_type state, typename... OsWoR>
inline constexpr bool is_state_possible_v =
    is_state_possible<
        make_index_sequence<sizeof...(OsWoR)>,
        OsWoR...
    >::template value<state>;

template<typename... OsWoR>
inline constexpr array<size_type, sizeof...(OsWoR)> one_valid_indexes_combo = {{
    hierarchy_utility_t<
        deduce_hierarchy_t<OsWoR>
    >::template class_index_v<
        OsWoR
    >...
}};

template<typename... OsWoR>
inline constexpr size_type one_valid_state =
    mapper<OsWoR...>::map(one_valid_indexes_combo<OsWoR...>);

template<
    typename Visitor, 
    typename SIs, 
    typename OIs, 
    typename... Os>
struct visitor_return_types_same;

template<
    typename Visitor,
    size_type... SIs,
    size_type... OIs,
    typename... Os>
struct visitor_return_types_same<
    Visitor, 
    index_sequence<SIs...>,
    index_sequence<OIs...>,
    Os...> 
{
    template<size_type state>
    using ret_type_for_impl = decltype(
        declval<Visitor&&>()(
            declval<
                match_cvref_t<
                    Os&&,
                    nth_alternative_of_t<
                        indexes_for_v<
                            state,
                            remove_reference_t<Os>...
                        >[OIs],
                        remove_reference_t<Os>
                    >
                >
            >()...
        )
    );

    static constexpr size_type always_valid_state = 
        one_valid_state<remove_reference_t<Os>...>;

    template<size_type state>
    using ret_type_for = ret_type_for_impl<
        ternary<
            is_state_possible_v<
                state,
                remove_reference_t<Os>...
            >
        >::template value<
            size_type,
            state,
            always_valid_state
        >
    >;

    using one_ret_type = ret_type_for<always_valid_state>;
    static constexpr bool value = (is_same_v<one_ret_type, ret_type_for<SIs>> && ...);
};

template<typename Visitor, typename... Os>
inline constexpr bool visitor_return_types_same_v = 
    visitor_return_types_same<
        Visitor,
        make_index_sequence<mapper<remove_reference_t<Os>...>::range_v>,
        make_index_sequence<sizeof...(Os)>,
        Os...
    >::value;

template<typename Visitor, size_type State, typename... Os>
using ret_type_for_visitor_invocation_t = 
    typename visitor_return_types_same<
        Visitor,
        make_index_sequence<mapper<remove_reference_t<Os>...>::range_v>,
        make_index_sequence<sizeof...(Os)>,
        Os...
    >::template ret_type_for<State>;

template<
    typename Ret, 
    typename Visitor,
    typename OIs,
    typename SIs,
    typename... Os>
struct make_visit_vtable;

template<
    typename Ret,
    typename Visitor,
    size_type... OIs,
    size_type... SIs,
    typename... Os>
struct make_visit_vtable<
    Ret,
    Visitor,
    index_sequence<OIs...>,
    index_sequence<SIs...>,
    Os...> 
{
    using fn_type = Ret (*) (Visitor&&, Os&& ...);

    template<size_type state>
    static decltype(auto) invoke_visitor_impl(Visitor&& visitor, Os&& ... os) {
        constexpr auto& indexes = indexes_for_v<state, remove_reference_t<Os>...>;
        return static_cast<Visitor&&>(visitor)(
            static_cast<
                match_cvref_t<
                    Os&&,
                    nth_alternative_of_t<
                        indexes[OIs],
                        remove_reference_t<Os>
                    >
                >
            >(
                *static_cast<
                    nth_alternative_of_t<
                        indexes[OIs],
                        remove_reference_t<Os>
                    >*
                >(
                    instance_ptr_of<
                        deduce_mode_v<remove_reference_t<Os>>,
                        Os
                    >::template f<
                        (deduce_mode_v<remove_reference_t<Os>> == mode::barebones) ?
                        indexes[OIs] : 0
                    >(static_cast<Os&&>(os))
                )
            )...
        );
    }

    template<size_type state>
    static Ret invoke_visitor(Visitor&& visitor, Os&& ... os) {
        if constexpr(is_same_v<remove_cv_t<Ret>, void>) {
            static_cast<void>(
                invoke_visitor_impl<state>(
                    static_cast<Visitor&&>(visitor),
                    static_cast<Os&&>(os)...
                )
            );
        }
        else {
            return invoke_visitor_impl<state>(
                static_cast<Visitor&&>(visitor),
                static_cast<Os&&>(os)...
            );
        }
    }

    static constexpr size_type always_valid_state =
        one_valid_state<remove_reference_t<Os>...>;

    template<size_type state>
    static constexpr fn_type get_fn_ptr_for = &invoke_visitor<
        ternary<
            is_state_possible_v<
                state,
                remove_reference_t<Os>...
            >
        >::template value<
            size_type,
            state,
            always_valid_state
        >
    >;

    static constexpr array<fn_type, sizeof...(SIs)> table = {{
        get_fn_ptr_for<SIs>...
    }};
};

template<typename R, typename Visitor, typename... Os>
inline R visit_impl(Visitor&& visitor, Os&& ... os) {
    array<size_type, sizeof...(Os)> indexes_arr = {{
        (get_type_info(static_cast<Os&&>(os))._cavi_type_id)...
    }};

    using mapper_type =  mapper<remove_reference_t<Os>...>;
    size_type mapped_value = mapper_type::map(indexes_arr);
    constexpr size_type mapping_range = mapper_type::range_v;

    using fn_type = R (*)(Visitor&&, Os&&...);
    constexpr const array<fn_type, mapping_range>& table = make_visit_vtable<
        R,
        Visitor,
        make_index_sequence<sizeof...(Os)>,
        make_index_sequence<mapping_range>,
        Os...
    >::table;

#define CAVI_VISIT_CASE(n, UNUSED1, UNUSED2)                                                      \
    case n:                                                                                       \
    if constexpr(n < mapping_range &&                                                             \
                is_state_possible_v<(n < mapping_range) ? n : 0, remove_reference_t<Os>...>) {    \
        return table[n](static_cast<Visitor&&>(visitor), static_cast<Os&&>(os)...);               \
    }                                                                                             \
    CAVI_UNREACHABLE;

    if constexpr(mapping_range <= 16) {
        switch(mapped_value) {
            CAVI_REPEAT_NUM_HANDLER_16(CAVI_VISIT_CASE, 0, 0, 0)
            default: CAVI_UNREACHABLE;
        }
    }
    else if constexpr(mapping_range <= 64) {
        switch(mapped_value) {
            CAVI_REPEAT_NUM_HANDLER_64(CAVI_VISIT_CASE, 0, 0, 0)
            default: CAVI_UNREACHABLE;
        }
    }
    else if constexpr(mapping_range <= 256) {
        switch(mapped_value) {
            CAVI_REPEAT_NUM_HANDLER_256(CAVI_VISIT_CASE, 0, 0, 0)
            default: CAVI_UNREACHABLE;
        }
    }
    else
    {
        return table[mapped_value](
            static_cast<Visitor&&>(visitor),
            static_cast<Os&&>(os)...
        );
    }
}

template<typename Visitor, typename... Os>
inline decltype(auto) visit(Visitor&& visitor, Os&& ... os) {
    (call_hierarchy_verified<remove_reference_t<Os>>(), ...);
    static_assert(!(is_pointer_v<remove_reference_t<Os>> || ...), 
                  "objects must not be of pointer type");
    static_assert(visitor_return_types_same_v<Visitor, Os...>,
                  "all invocations of visitor must have the same return type and value cateogry");

    using R = ret_type_for_visitor_invocation_t<
        Visitor,
        one_valid_state<remove_reference_t<Os>...>,
        Os...
    >;

    return visit_impl<R>(static_cast<Visitor&&>(visitor),
                         static_cast<Os&&>(os)...);
}

template<typename Visitor>
inline decltype(auto) visit(Visitor&& visitor) {
    return static_cast<Visitor&&>(visitor)();
}

template<typename R, typename Visitor, typename... Os>
inline R visit(Visitor&& visitor, Os&& ... os) {
    (call_hierarchy_verified<remove_reference_t<Os>>(), ...);
    static_assert(!(is_pointer_v<remove_reference_t<Os>> || ...),
                  "objects must not be of pointer type");

    return visit_impl<R>(static_cast<Visitor&&>(visitor),
                         static_cast<Os&&>(os)...);
}

template<typename R, typename Visitor>
inline R visit(Visitor&& visitor) {
    if constexpr(is_same_v<remove_cv_t<R>, void>) {
        static_cast<void>(static_cast<Visitor&&>(visitor)());
    }
    else {
        return static_cast<Visitor&&>(visitor)();
    }
}

} // namespace cavi

#endif
