//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CAVI_HEADER_INSTANTIATE
#define CAVI_HEADER_INSTANTIATE

#include "core.h"

namespace cavi {
template<typename... Ts>
struct uniqueness_checker : type_identity<Ts>... {};

template<typename hutil, typename T, mode tim, typename BaseTsL>
struct base_list_verifier;

template<typename hutil, typename T, mode tim, typename... BaseTs>
struct base_list_verifier<hutil, T, tim, list<BaseTs...>> {
    uniqueness_checker<BaseTs...> uc;

    static_assert(((sizeof(BaseTs) >= 0) && ...), "incomplete type");

    static_assert((is_class_v<BaseTs> && ...), "base types must be of class type");

    static_assert(((!is_const_v<BaseTs> && !is_volatile_v<BaseTs>) && ...),
                  "base types cannot be const/volatile");

    static_assert((is_base_of_v<BaseTs, T> && ...),
                  "base class specified is not a base class");

    static_assert((tim != mode::barebones || sizeof...(BaseTs) <= 1),
                  "barebones mode requires a single inheritance hierarchy");

    static_assert((!is_same_v<T, BaseTs> && ...), "class can't inherit itself");

    static_assert((hutil::template is_class_declared_v<BaseTs> && ...),
                  "base class type doesn't have a class declaration in the hierarchy decl");

    static constexpr size_type value = sizeof...(BaseTs);
};

template<template <typename> class HBase, mode tim, typename hierarchy>
struct hierarchy_verifier;

template<template <typename> class HBase, mode tim, typename... ClassTs, typename... BaseTsL>
struct hierarchy_verifier<HBase, tim, hierarchy_def<class_def<ClassTs, BaseTsL>...>> {
    uniqueness_checker<ClassTs...> uc;

    static_assert(((sizeof(ClassTs) >= 0) && ...), "incomplete type");

    static_assert((is_class_v<ClassTs> && ...), "declared type must be a class type");

    static_assert(((!is_const_v<ClassTs> && !is_volatile_v<ClassTs>) && ...),
                  "declared class type cannot be const/volatile");

    static_assert((is_base_of_v<HBase<ClassTs>, ClassTs> && ...),
                  "class does not inherit from cavi base");

    using hutil = hierarchy_utility_t<hierarchy_def<class_def<ClassTs, BaseTsL>...>>;

    // dummy value (no use for it)
    static constexpr size_type value = 
        (base_list_verifier<hutil, ClassTs, tim, BaseTsL>::value + ... + 0);
};

// -------------------------------------------------------------------------------------------------
template<typename T>
T& cast_exports<T, mode::barebones, static_cast<size_type>(-1)>::
cast_to_mdt(const volatile void* topmost_base_obj) {
    static_assert(sizeof(T) >= 0, "incomplete type");

    using hutil = hierarchy_utility_t<deduce_hierarchy_t<T>>;
    using base_type = typename hutil::template any_topmost_base_t<T>;
    static_assert(is_base_of_v<base_type, T>,
                  "base class specified is not a base class");

    return 
        (T&)(const_cast<base_type&>(
            *static_cast<const volatile base_type*>(topmost_base_obj)
        ));
};
// -------------------------------------------------------------------------------------------------
template<typename T, typename... BaseTs>
constexpr void* cast_to_db_impl(const volatile T& from, size_type to_id, list<BaseTs...>*) {
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<T>>;

    static_assert(sizeof(T) >= 0, "incomplete class type");
    static_assert((is_base_of_v<BaseTs, T> && ...),
                  "base class specified is not a base class");
    
    if(to_id == hutil::template class_index_v<T>) {
        return addressof(const_cast<T&>(from));
    } else {
        void* result = nullptr;
        [[maybe_unused]] bool d =
            (((to_id == hutil::template class_index_v<BaseTs>) ?
              (result = addressof((BaseTs&)(const_cast<T&>(from))), true) :
              false
             ) || ...
            );

        return result;
    }
}

template<typename T>
void* cast_exports<T, mode::standard, static_cast<size_type>(-1)>::
cast_to_db(const volatile T& from, size_type to_id) {
    static_assert(sizeof(T) >= 0, "incomplete class type");
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<T>>;
    using base_specs = typename hutil::template base_spec_list_of_t<T>;

    return cast_to_db_impl(
        from,
        to_id,
        static_cast<base_specs*>(nullptr)
    );
}
// -------------------------------------------------------------------------------------------------
template<typename T, typename... BaseTs>
constexpr bool is_db_public_impl(size_type to_id, list<BaseTs...>*) {
    using hierarchy = deduce_hierarchy_t<T>;
    using hutil = hierarchy_utility_t<hierarchy>;

    static_assert(sizeof(T) >= 0, "incomplete class type");
    static_assert((is_base_of_v<BaseTs, T> && ...),
                  "base class specified is not a base class");

    if(to_id == hutil::template class_index_v<T>) {
        return true;
    } else {
        bool res = false;
        [[maybe_unused]] bool dummy =
            (((to_id == hutil::template class_index_v<BaseTs>) ?
              (res = is_pointer_static_cast_ok_v<T*, BaseTs*>, true) :
              false
             ) || ...
            );

        return res;
    }
}

template<typename T>
bool cast_exports<T, mode::standard, static_cast<size_type>(-1)>::
is_db_public(size_type to_id) {
    static_assert(sizeof(T) >= 0, "incomplete class type");
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<T>>;
    using base_specs = typename hutil::template base_spec_list_of_t<T>;

    return is_db_public_impl<T>(to_id, static_cast<base_specs*>(nullptr));
}
// -------------------------------------------------------------------------------------------------
template<typename HDecl, template <typename> class HBase>
struct instantiate_helper {
    using hutil = hierarchy_utility_t<
        transform_hierarchy_decl_t<HDecl, HBase>
    >;

    static constexpr size_type N = hutil::num_of_classes;

    template<size_type I>
    using nth_classtype_t = typename hutil::template class_from_index_t<(I < N) ? I : 0>;

    static constexpr mode tim = deduce_mode_v<nth_classtype_t<0>>;

    template<size_type I>
    static constexpr size_type dup_param = (I < N) ? static_cast<size_type>(-1) : I;
};
}; // namespace cavi

#define CAVI_VERIFIER_FUNC_DEF(hierarchydcl, hierarchy_base)                                      \
template<>                                                                                        \
cavi::size_type cavi::hierarchy_verified<hierarchy_base>() {                                      \
    using H = cavi::transform_hierarchy_decl_t<hierarchydcl, hierarchy_base>;                     \
    using hutil = cavi::hierarchy_utility_t<H>;                                                   \
    constexpr mode tim = cavi::deduce_mode_v<typename hutil::template class_from_index_t<0>>;     \
                                                                                                  \
    return cavi::hierarchy_verifier<hierarchy_base, tim, H>{}.value;                              \
}

#define CAVI_INSTANTIATE_CASE(I, H, B)                                                            \
template struct cavi::cast_exports<                                                               \
    typename cavi::instantiate_helper<H, B>::template nth_classtype_t<I>,                         \
    cavi::instantiate_helper<H, B>::tim,                                                          \
    cavi::instantiate_helper<H, B>::template dup_param<I>                                         \
>;

#define CAVI_INSTANTIATE(hierarchydcl, hierarchy_base)                                            \
CAVI_VERIFIER_FUNC_DEF(hierarchydcl, hierarchy_base)                                              \
CAVI_REPEAT_NUM_HANDLER_256(CAVI_INSTANTIATE_CASE, 0, hierarchydcl, hierarchy_base)


#endif