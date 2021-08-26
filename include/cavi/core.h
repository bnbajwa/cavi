//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CAVI_HEADER_CAVI_H
#define CAVI_HEADER_CAVI_H

#include "lib.h"
#include "mp.h"

namespace cavi {
template<typename T, typename BaseTsL>
struct class_def;

template<typename... ClassDefTs>
struct hierarchy_def;

template<typename... ClassTs, typename... BaseSpecLTs>
struct hierarchy_def<class_def<ClassTs, BaseSpecLTs>...> {
    static constexpr size_type num_of_classes = sizeof...(ClassTs);
};

template<template <template <typename> class> class... Ts>
struct class_tpl_list;

template<typename T, typename BL>
struct class_decl;

template<template <template <typename> class> class T, typename L>
struct class_tpl_decl;

template<typename T, typename... BaseTs>
using class_ = class_decl<T, list<BaseTs...>>;

template<
    template <template <typename> class> class T,
    template <template <typename> class> class... BaseTs>
using class_tpl = class_tpl_decl<T, class_tpl_list<BaseTs...>>;

template<typename... ClassDeclTs>
struct hierarchy_decl;

template<typename TL1, typename NTL2>
struct hierarchy_dissected_decl;
// -------------------------------------------------------------------------------------------------
template<typename HDecl, template <typename> class HBase>
struct transform_hierarchy_decl;

template<template <typename> class HBase, typename... CTs, typename... CBLs>
struct transform_hierarchy_decl<hierarchy_decl<class_decl<CTs, CBLs>...>, HBase> {
    using type = hierarchy_def<
        class_def<
            CTs,
            CBLs
        >...
    >;
};

template<template <typename> class HBase, typename L>
struct instantiate_tpl_base_list;

template<template <typename> class HBase, template <template <typename> class> class... BaseTs>
struct instantiate_tpl_base_list<HBase, class_tpl_list<BaseTs...>> {
    using type = list<BaseTs<HBase>...>;
};

template<
    template <typename> class HBase,
    template <template <typename> class> class... CTs,
    typename... CBLs>
struct transform_hierarchy_decl<hierarchy_decl<class_tpl_decl<CTs, CBLs>...>, HBase> {
    using type = hierarchy_def<
        class_def<
            CTs<HBase>,
            typename instantiate_tpl_base_list<HBase, CBLs>::type
        >...
    >;
};

template<
    template <typename> class HBase,
    template <template <typename> class> class... TCTs,
    typename... TCBLs,
    typename... NTCTs,
    typename... NTCBLs>
struct transform_hierarchy_decl<
    hierarchy_dissected_decl<
        list<class_tpl_decl<TCTs, TCBLs>...>,
        list<class_decl<NTCTs, NTCBLs>...>
    >,
    HBase
> {
    using type = hierarchy_def<
        class_def<
            TCTs<HBase>,
            typename instantiate_tpl_base_list<HBase, TCBLs>::type
        >...,
        class_def<
            NTCTs,
            NTCBLs
        >...            
    >;
};

template<typename HDecl, template <typename> class HBase>
using transform_hierarchy_decl_t = typename transform_hierarchy_decl<HDecl, HBase>::type;
// -------------------------------------------------------------------------------------------------
template<typename HDecl, typename... ClassDeclTs>
struct extend_hierarchy_impl;

template<
    typename... CTs1,
    typename... CBLs1,
    typename... CTs2, 
    typename... CBLs2>
struct extend_hierarchy_impl<
    hierarchy_decl<class_decl<CTs1, CBLs1>...>,
    class_decl<CTs2, CBLs2>...
> {
    using type = hierarchy_decl<
        class_decl<CTs1, CBLs1>...,
        class_decl<CTs2, CBLs2>...
    >;
};

template<
    template <template <typename> class> class... TCTs1,
    template <template <typename> class> class... TCTs2,
    typename... TCBLs1,
    typename... TCBLs2>
struct extend_hierarchy_impl<
    hierarchy_decl<class_tpl_decl<TCTs1, TCBLs1>...>,
    class_tpl_decl<TCTs2, TCBLs2>...
> {
    using type = hierarchy_decl<
        class_tpl_decl<TCTs1, TCBLs1>...,
        class_tpl_decl<TCTs2, TCBLs2>...
    >;
};

template<
    template <template <typename> class> class... TCTs,
    typename... TCBLs,
    typename... NTCTs,
    typename... NTCBLs>
struct extend_hierarchy_impl<
    hierarchy_decl<class_tpl_decl<TCTs, TCBLs>...>,
    class_decl<NTCTs, NTCBLs>...
> {
    using type = hierarchy_dissected_decl<
        list<class_tpl_decl<TCTs, TCBLs>...>,
        list<class_decl<NTCTs, NTCBLs>...>
    >;
};

template<
    template <template <typename> class> class... TCTs,
    typename... TCBLs,
    typename... NTCTs1,
    typename... NTCBLs1,
    typename... NTCTs2,
    typename... NTCBLs2>
struct extend_hierarchy_impl<
    hierarchy_dissected_decl<
        list<class_tpl_decl<TCTs, TCBLs>...>,
        list<class_decl<NTCTs1, NTCBLs1>...>
    >,
    class_decl<NTCTs2, NTCBLs2>...
> {
    using type = hierarchy_dissected_decl<
        list<class_tpl_decl<TCTs, TCBLs>...>,
        list<
            class_decl<NTCTs1, NTCBLs1>...,
            class_decl<NTCTs2, NTCBLs2>...
        >
    >;
};

template<
    template <template <typename> class> class... TCTs1,
    template <template <typename> class> class... TCTs2,
    typename... TCBLs1,
    typename... TCBLs2,
    typename... NTCTs,
    typename... NTCBLs>
struct extend_hierarchy_impl<
    hierarchy_dissected_decl<
        list<class_tpl_decl<TCTs1, TCBLs1>...>,
        list<class_decl<NTCTs, NTCBLs>...>
    >,
    class_tpl_decl<TCTs2, TCBLs2>...
> {
    using type = hierarchy_dissected_decl<
        list<
            class_tpl_decl<TCTs1, TCBLs1>...,
            class_tpl_decl<TCTs2, TCBLs2>...
        >,
        list<class_decl<NTCTs, NTCBLs>...>
    >;
};

template<
    template <template <typename> class> class... TCTs,
    typename... TCBLs>
struct extend_hierarchy_impl<hierarchy_decl<class_tpl_decl<TCTs, TCBLs>...>> {
    using type = hierarchy_decl<
        class_tpl_decl<TCTs, TCBLs>...
    >;
};

template<typename HDecl, typename... ClassDeclTs>
using extend_hierarchy_t = typename extend_hierarchy_impl<HDecl, ClassDeclTs...>::type;
// -------------------------------------------------------------------------------------------------
template<typename hierarchy, typename Is>
struct hierarchy_utility;

template<typename T, typename BSL, size_type O>
struct lookup_structure {
    using type = T;
    using base_spec_list = BSL;
    static constexpr size_type ordinal = O;
};

template<typename... ClassTs, typename... BaseSpecLTs, size_type... ClassIs>
struct hierarchy_utility<
    hierarchy_def<class_def<ClassTs, BaseSpecLTs>...>,
    index_sequence<ClassIs...>>
    : lookup_structure<ClassTs, BaseSpecLTs, ClassIs>... 
{
    static constexpr size_type num_of_classes = sizeof...(ClassTs);

    struct has_class_helper : type_identity<ClassTs>... {};

    template<typename T>
    static constexpr bool is_class_declared_v =
        is_base_of_v<type_identity<remove_cv_t<T>>, has_class_helper>;

    template<typename T, typename BSL, size_type O>
    static lookup_structure<T, BSL, O> lookup_from_class(lookup_structure<T, BSL, O>*);

    template<size_type O, typename T, typename BSL>
    static lookup_structure<T, BSL, O> lookup_from_ordinal(lookup_structure<T, BSL, O>*);

    template<typename T>
    using base_spec_list_of_t = typename decltype(
        lookup_from_class<remove_cv_t<T>>(static_cast<hierarchy_utility*>(nullptr))
    )::base_spec_list;

    template<typename T>
    static constexpr size_type class_index_v = decltype(
        lookup_from_class<remove_cv_t<T>>(static_cast<hierarchy_utility*>(nullptr))
    )::ordinal;

    template<size_type I>
    using class_from_index_t = typename decltype(
        lookup_from_ordinal<I>(static_cast<hierarchy_utility*>(nullptr))
    )::type;

    // -------------------------------------------------------------------
    // The list returned may have duplicates.
    template<typename L>
    struct referenced_bases;

    template<typename... Ts>
    struct referenced_bases<list<Ts...>> {
        using type = concat_t<
            list<Ts...>,
            typename referenced_bases<
                base_spec_list_of_t<Ts>
            >::type...
        >;
    };

    template<typename T>
    using referenced_bases_t =
        typename referenced_bases<
            base_spec_list_of_t<remove_cv_t<T>>
        >::type;
    // -------------------------------------------------------------------
    template<typename Base, typename Derived>
    static constexpr bool contains_base_v = 
        is_same_v<remove_cv_t<Base>, remove_cv_t<Derived>> ||
        has_v<
            referenced_bases_t<Derived>,
            remove_cv_t<Base>
        >;

    template<typename T>
    static constexpr array<bool, num_of_classes> contains_base_survey = {{
        contains_base_v<T, ClassTs>...
    }};

    // -------------------------------------------------------------------
    // Topmost bases are those with zero bases themselves.
    template<typename L>
    struct all_paths_to_topmost_bases_impl;

    template<typename... Ts>
    struct all_paths_to_topmost_bases_impl<list<Ts...>> {
        template<typename... Trajectory>
        using type = typename ternary<sizeof...(Ts) != 0>::template type<
            concat_t<
                typename all_paths_to_topmost_bases_impl<
                    base_spec_list_of_t<Ts>
                >::template type<Trajectory..., Ts>...
            >,
            list<list<Trajectory...>>
        >;
    };

    template<typename T>
    using all_paths_to_topmost_bases_t =
        typename all_paths_to_topmost_bases_impl<
            base_spec_list_of_t<T>
        >::template type<remove_cv_t<T>>;

    // -------------------------------------------------------------------
    // The last boolean parameter is there because C++ Core DR 727 is
    // not (yet) implemented in GCC.
    template<typename T, bool B>
    struct path_to_any_topmost_base_impl;

    template<typename FT, typename... Ts, bool B>
    struct path_to_any_topmost_base_impl<list<FT, Ts...>, B> {
        template<typename... Trajectory>
        using type = typename path_to_any_topmost_base_impl<
            base_spec_list_of_t<FT>,
            false
        >::template type<Trajectory..., FT>;
    };

    template<bool B>
    struct path_to_any_topmost_base_impl<list<>, B> {
        template<typename... Trajectory>
        using type = list<Trajectory...>;
    };

    template<typename T>
    using path_to_any_topmost_base_t = typename path_to_any_topmost_base_impl<
        base_spec_list_of_t<T>,
        false
    >::template type<remove_cv_t<T>>;

    // -------------------------------------------------------------------
    // Note, that for a single inheritance inhierarchy, there  is only one
    // topmost base for any class in the hierarchy. The last bool parameter
    // is there because C++ Core DR 727 is not (yet) implemented in GCC.
    template<typename T, bool B>
    struct any_topmost_base_impl;

    template<typename FT, typename... R, bool B>
    struct any_topmost_base_impl<list<FT, R...>, B> {
        template<typename Prev>
        using type = typename any_topmost_base_impl<
            base_spec_list_of_t<FT>,
            false
        >::template type<FT>;
    };

    template<bool B>
    struct any_topmost_base_impl<list<>, B> {
        template<typename Prev>
        using type = Prev;
    };

    template<typename T>
    using any_topmost_base_t = typename any_topmost_base_impl<
        base_spec_list_of_t<T>,
        false
    >::template type<remove_cv_t<T>>;

    struct casting;
};

template<typename hierarchy>
using hierarchy_utility_t = hierarchy_utility<
    hierarchy,
    make_index_sequence<hierarchy::num_of_classes>
>;
// -------------------------------------------------------------------------------------------------
template<typename T, typename... Ts1, typename... Ts2>
inline constexpr decltype(auto) native_sequential_upcast_impl(T&& d, list<Ts1...>*, list<Ts2...>*) {
    static_assert((is_base_of_v<Ts2, Ts1> && ...), "invalid path");

    using T1 = nth_type_t<0, Ts1...>;
    match_cv_t<T1, void>* p = addressof((T1&)(d));
    ((p = addressof((Ts2&)(*static_cast<Ts1*>(p)))), ...);

    using tn = match_cv_t<T1, nth_type_t<sizeof...(Ts1)-1, Ts1...>>;
    return static_cast<match_ref_t<T&&, tn>>(*static_cast<tn*>(p));
}

template<typename T, typename F, typename... R>
inline constexpr decltype(auto) native_sequential_upcast(T&& d, list<F, R...>* L1) {
    static_assert(sizeof(remove_reference_t<T>) >= 0, "source must be of complete type");

    using LT1 = list<
        match_cv_t<remove_reference_t<T>, F>,
        match_cv_t<remove_reference_t<T>, R>...
    >;

    using LT2 = list<
        match_cv_t<remove_reference_t<T>, R>...,
        match_cv_t<remove_reference_t<T>, nth_type_t<sizeof...(R), F, R...>>
    >;

    return native_sequential_upcast_impl(static_cast<T&&>(d),
                                   static_cast<LT1*>(nullptr),
                                   static_cast<LT2*>(nullptr));
}
// -------------------------------------------------------------------------------------------------
enum class mode
{
    barebones,
    standard
};

template<bool, size_type N, mode tim, typename derived, typename derived_base>
class type_info_bag;

template<typename derived_base, mode m>
struct hy_deduction_helper {
    static constexpr mode tim = m;
    using hierarchy = typename derived_base::_cavi_hierarchy;
    using base = derived_base;
};

template<typename derived, bool B, size_type N, mode tim, typename derived_base>
inline hy_deduction_helper<derived_base, tim>
deduce_hierarchy_helper_func(type_info_bag<B, N, tim, derived, derived_base>*);

template<typename derived>
struct deduce_hierarchy_impl {
    static_assert(sizeof(derived) >= 0, "incomplete type");
    using type = decltype(
        deduce_hierarchy_helper_func<remove_cv_t<derived>>(
            static_cast<remove_cv_t<derived>*>(nullptr)
        )
    );
};

template<typename derived>
using deduce_hierarchy_t = typename deduce_hierarchy_impl<derived>::type::hierarchy;

template<typename derived>
inline constexpr mode deduce_mode_v = deduce_hierarchy_impl<derived>::type::tim;

template<typename derived>
using deduce_base_t = typename deduce_hierarchy_impl<derived>::type::base;
// -------------------------------------------------------------------------------------------------
template<size_type N>
using id_integral_t = 
    typename ternary<N == static_cast<unsigned char>(N)>::template type<
        unsigned char,
        typename ternary<N == static_cast<unsigned short>(N)>::template type<
            unsigned short,
            typename ternary<N == static_cast<unsigned long>(N)>::template type<
                unsigned long,
                unsigned long long
            >
        >
    >;

template<size_type N_of_classes_in_hy, mode>
struct type_info;

template<size_type N>
struct type_info<N, mode::barebones> {
    id_integral_t<N> _cavi_type_id;

    constexpr type_info(void* ptr, id_integral_t<N> tid) :
        _cavi_type_id(tid) {}
};

template<size_type N>
struct type_info<N, mode::standard> {
    void* _cavi_instance_ptr;
    id_integral_t<N> _cavi_type_id;

    constexpr type_info(void* ptr, id_integral_t<N> tid) :
        _cavi_instance_ptr(ptr), _cavi_type_id(tid) {}
};

// -------------------------------------------------------------------------------------------------
template<bool, size_type N_of_classes_in_hy, mode, typename derived, typename derived_base>
class type_info_bag {
public:
    constexpr type_info_bag() {}
};

template<size_type N, mode tim, typename derived, typename derived_base>
class type_info_bag<true, N, tim, derived, derived_base> : private type_info<N, tim> {
public:
    constexpr type_info_bag() :
        type_info<N, tim>(
            (derived*)this,
            static_cast<id_integral_t<N>>(
                hierarchy_utility_t<deduce_hierarchy_t<derived>>::template class_index_v<derived>
            )
        )
    {
        // both to validate the cast above
        static_assert(sizeof(derived) >= 0, 
                      "derived type is incomplete");

        static_assert(is_base_of_v<type_info_bag, derived>,
                      "class does not inherit cavi base");
    }
};

template<typename hierarchy, mode tim, typename derived, typename derived_base>
using type_info_bag_t = type_info_bag<
    length_v<
        typename hierarchy_utility_t<hierarchy>::template base_spec_list_of_t<derived>
    > == 0,
    hierarchy::num_of_classes,
    tim,
    derived,
    derived_base
>;
// -------------------------------------------------------------------------------------------------
template<typename derived, typename id_type, typename... bases>
inline constexpr void set_type_info_from_path(derived* d, id_type id, list<bases...>* path)
{
    auto& casted = native_sequential_upcast(*d, path);

    using hierarchy = deduce_hierarchy_t<derived>;
    constexpr mode tim = deduce_mode_v<derived>;
    using ti_type = type_info<hierarchy::num_of_classes, tim>;

    using last = remove_reference_t<decltype(casted)>;
    static_assert(is_base_of_v<ti_type, last>,
                  "a class does not inherit from cavi base");

    ti_type& ti = (ti_type&)casted;
    ti._cavi_type_id = id;
    if constexpr(tim != mode::barebones)
        ti._cavi_instance_ptr = d;
}

template<mode tim, size_type N>
inline constexpr void set_instance_ptr(type_info<N, tim>& ti, void* ptr) {
    if constexpr(tim != mode::barebones)
        ti._cavi_instance_ptr = ptr;
}

template<typename derived, typename id_type, typename... Paths>
inline constexpr void set_type_info_multiple(derived* d, id_type id, list<Paths...>*) {
    (set_type_info_from_path(d, id, static_cast<Paths*>(nullptr)), ...);
}
// -------------------------------------------------------------------------------------------------
#define CAVI_MAKE_FRIEND template<typename> friend struct cavi::deduce_hierarchy_impl;

#define CAVI_INTERNAL_BASE_IMPL(hierarchydcl, base_name, base_mode)                               \
template<typename derived>                                                                        \
class base_name    : public cavi::type_info_bag_t<                                                \
    cavi::transform_hierarchy_decl_t<hierarchydcl, base_name>,                                    \
    base_mode,                                                                                    \
    derived,                                                                                      \
    base_name<derived>                                                                            \
> {                                                                                               \
    friend derived;                                                                               \
    friend cavi::hy_deduction_helper<base_name<derived>, base_mode>;                              \
                                                                                                  \
    using _cavi_hierarchy = cavi::transform_hierarchy_decl_t<hierarchydcl, base_name>;            \
    static constexpr cavi::mode _cavi_tim = base_mode;                                            \
                                                                                                  \
    constexpr base_name() noexcept :                                                              \
    cavi::type_info_bag_t<                                                                        \
        cavi::transform_hierarchy_decl_t<hierarchydcl, base_name>,                                \
        base_mode,                                                                                \
        derived,                                                                                  \
        base_name<derived>                                                                        \
    >() {                                                                                         \
        using hutil = cavi::hierarchy_utility_t<_cavi_hierarchy>;                                 \
        using ti_type = cavi::type_info<_cavi_hierarchy::num_of_classes, base_mode>;              \
        using id_type = cavi::id_integral_t<_cavi_hierarchy::num_of_classes>;                     \
                                                                                                  \
        static_assert(hutil::template is_class_declared_v<derived>,                               \
                      "class is not declared in the hierarchy");                                  \
                                                                                                  \
        derived* d = (derived*)(this);                                                            \
        cavi::size_type type_index = hutil::template class_index_v<derived>;                      \
        id_type type_id = static_cast<id_type>(type_index);                                       \
                                                                                                  \
        static_assert(cavi::is_base_of_v<ti_type, derived>,                                       \
                      "a class does not inherit from cavi base");                                 \
                                                                                                  \
        if constexpr(cavi::is_pointer_explicit_cast_ok_v<derived*, ti_type*>) {                   \
            ti_type& ti = (ti_type&)(*d);                                                         \
            ti._cavi_type_id = type_id;                                                           \
            cavi::set_instance_ptr(ti, d);                                                        \
        } else {                                                                                  \
            /* multiple instances of ti_type - set them all    */                                 \
            using paths = typename hutil::template all_paths_to_topmost_bases_t<derived>;         \
            cavi::set_type_info_multiple(d, type_id, static_cast<paths*>(nullptr));               \
        }                                                                                         \
    }                                                                                             \
                                                                                                  \
public:                                                                                           \
    constexpr base_name(const base_name&) noexcept : base_name() {}                               \
    constexpr base_name(base_name&&) noexcept : base_name() {}                                    \
    base_name& operator = (const base_name&) noexcept {}                                          \
    base_name& operator = (base_name&&) noexcept {}                                               \
    ~base_name() noexcept = default;                                                              \
};

#define CAVI_DEF_BAREBONES_BASE(hierarchydcl, base_name) \
CAVI_INTERNAL_BASE_IMPL(hierarchydcl, base_name, cavi::mode::barebones)

#define CAVI_DEF_STANDARD_BASE(hierarchydcl, base_name) \
CAVI_INTERNAL_BASE_IMPL(hierarchydcl, base_name, cavi::mode::standard)
// -------------------------------------------------------------------------------------------------
#define CAVI_REPEAT_NUM_HANDLER_4(handler, n, a1, a2)                                             \
    handler((n + 0), a1, a2)                                                                      \
    handler((n + 1), a1, a2)                                                                      \
    handler((n + 2), a1, a2)                                                                      \
    handler((n + 3), a1, a2)                                                                      \

#define CAVI_REPEAT_NUM_HANDLER_16(handler, n, a1, a2)                                            \
    CAVI_REPEAT_NUM_HANDLER_4(handler, (n + 0), a1, a2)                                           \
    CAVI_REPEAT_NUM_HANDLER_4(handler, (n + 4), a1, a2)                                           \
    CAVI_REPEAT_NUM_HANDLER_4(handler, (n + 8), a1, a2)                                           \
    CAVI_REPEAT_NUM_HANDLER_4(handler, (n + 12), a1, a2)

#define CAVI_REPEAT_NUM_HANDLER_64(handler, n, a1, a2)                                            \
    CAVI_REPEAT_NUM_HANDLER_16(handler, (n + 0), a1, a2)                                          \
    CAVI_REPEAT_NUM_HANDLER_16(handler, (n + 16), a1, a2)                                         \
    CAVI_REPEAT_NUM_HANDLER_16(handler, (n + 32), a1, a2)                                         \
    CAVI_REPEAT_NUM_HANDLER_16(handler, (n + 48), a1, a2)

#define CAVI_REPEAT_NUM_HANDLER_256(handler, n, a1, a2)                                           \
    CAVI_REPEAT_NUM_HANDLER_64(handler, (n + 0), a1, a2)                                          \
    CAVI_REPEAT_NUM_HANDLER_64(handler, (n + 64), a1, a2)                                         \
    CAVI_REPEAT_NUM_HANDLER_64(handler, (n + 128), a1, a2)                                        \
    CAVI_REPEAT_NUM_HANDLER_64(handler, (n + 192), a1, a2)
// -------------------------------------------------------------------------------------------------
template<template <typename> class HBase>
size_type hierarchy_verified();

template<typename B>
struct call_hierarchy_verified_helper;

template<typename T, template <typename> class HBase>
struct call_hierarchy_verified_helper<HBase<T>> {
    static void func() {
        hierarchy_verified<HBase>();
    }
};

template<typename T>
inline void call_hierarchy_verified() {
    using derived_base = deduce_base_t<T>;
    call_hierarchy_verified_helper<derived_base>::func();
}

template<typename T, mode hmode, size_type I = static_cast<size_type>(-1)>
struct cast_exports {};

template<typename T>
struct cast_exports<T, mode::barebones, static_cast<size_type>(-1)> {
    static T& cast_to_mdt(const volatile void* topmost_base_obj);
};

template<typename T>
struct cast_exports<T, mode::standard, static_cast<size_type>(-1)> {
    static void* cast_to_db(const volatile T& from, size_type to_id);
    static bool is_db_public(size_type to_id);
};
// -------------------------------------------------------------------------------------------------
template<typename T>
inline const auto& get_type_info(const T& obj) {
    using hierarchy = deduce_hierarchy_t<T>;
    constexpr mode tim = deduce_mode_v<T>;
    using deduced_ti_base = type_info<hierarchy::num_of_classes, tim>;
    using hutil = hierarchy_utility_t<hierarchy>;

    using ti_type =  typename ternary<is_volatile_v<T>>::template type<
        const volatile deduced_ti_base,
        const deduced_ti_base
    >;

    static_assert(is_base_of_v<deduced_ti_base, T>,
                  "a class does not inherit from cavi base");

    if constexpr(is_pointer_explicit_cast_ok_v<const T*, ti_type*>) {
        const ti_type& ti = (const ti_type&)obj;
        return ti;
    }
    else {
        // has multiple instances of ti_type - pick any one of them
        using path = typename hutil::template path_to_any_topmost_base_t<T>;
        auto& casted = native_sequential_upcast(obj, static_cast<path*>(nullptr));

        using last = remove_reference_t<decltype(casted)>;
        static_assert(is_base_of_v<deduced_ti_base, last>,
                      "a class does not inherit from cavi base");

        const ti_type& ti = (const ti_type&)casted;
        return ti;
    }
}
// -------------------------------------------------------------------------------------------------
template<mode, typename O>
struct instance_ptr_of {
    template<size_type dummy>
    static void* f(O&& obj) {
        return get_type_info(static_cast<O&&>(obj))._cavi_instance_ptr;
    }
};

template<typename O>
struct instance_ptr_of<mode::barebones, O> {
    template<size_type MDT_INDEX>
    static void* f(O&& obj) {
        using O_wo_ref = remove_reference_t<O>;
        using hutil = hierarchy_utility_t<deduce_hierarchy_t<O_wo_ref>>;
        using MDT = typename hutil::template class_from_index_t<MDT_INDEX>;
        using topmost_base_type = typename hutil::template any_topmost_base_t<O_wo_ref>;

        static_assert(sizeof(O_wo_ref) >= 0, "incomplete type");
        static_assert(is_base_of_v<topmost_base_type, O_wo_ref>,
                      "base class specified in hierarchy is not a base class");

        auto* ptr = addressof((const volatile topmost_base_type&)(obj));
        return addressof(cast_exports<MDT, mode::barebones>::cast_to_mdt(ptr));
    }
};

}; // namespace cavi

#endif
