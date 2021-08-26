//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CAVI_HEADER_CAST
#define CAVI_HEADER_CAST

#include "core.h"

namespace cavi {
template<typename HYCT, typename F, typename... Ts1, typename... Ts2>
inline decltype(auto) sequential_upcast_impl(F&& from, list<Ts1...>*, list<Ts2...>*) {
    using F_wo_ref = remove_reference_t<F>;
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<HYCT>>;

    void* p = cast_exports<remove_cv_t<F_wo_ref>, mode::standard>::
        cast_to_db(from, hutil::template class_index_v<nth_type_t<0, Ts1...>>);

    ((p = cast_exports<Ts1, mode::standard>::cast_to_db(
        *static_cast<match_cv_t<F_wo_ref, Ts1>*>(p),
        hutil::template class_index_v<Ts2>)
     ), ...
    );

    using tn = match_cv_t<F_wo_ref, nth_type_t<sizeof...(Ts1)-1, Ts1...>>;
    return static_cast<match_ref_t<F&&, tn>>(*static_cast<tn*>(p));
}

// upcast via exported cast_to_db functions
template<typename HYCT, typename F, typename T1, typename... R>
inline decltype(auto) sequential_upcast(F&& from, list<T1, R...>* L1) {
    using LT2 = list<R..., nth_type_t<sizeof...(R), T1, R...>>;
    return sequential_upcast_impl<HYCT>(static_cast<F&&>(from), L1, static_cast<LT2*>(nullptr));
}
// -------------------------------------------------------------------------------------------------
template<typename HYCT, typename... Ts1, typename... Ts2>
inline bool is_direct_base_walk_public_impl(list<Ts1...>*, list<Ts2...>*) {
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<HYCT>>;
    return (cast_exports<
                Ts1,
                mode::standard
            >::is_db_public(hutil::template class_index_v<Ts2>) && ...);
}

template<typename HYCT, typename F, typename... R>
inline bool is_direct_base_walk_public(list<F, R...>* L1) {
    using LT2 = list<R..., nth_type_t<sizeof...(R), F, R...>>;
    return is_direct_base_walk_public_impl<HYCT>(L1, static_cast<LT2*>(nullptr));
}

template<typename HYCT>
inline bool is_direct_base_walk_public(list<>*) {
    return true;
}
// -------------------------------------------------------------------------------------------------
template<typename... ClassTs, typename... BaseSpecLTs, size_type... ClassIs>
struct hierarchy_utility<
    hierarchy_def<class_def<ClassTs, BaseSpecLTs>...>,
    index_sequence<ClassIs...>
>::casting
{
    template<typename T>
    static constexpr array<size_type, num_of_classes> subobject_count_v = {{
        (count_v<referenced_bases_t<ClassTs>, remove_cv_t<T>> +
         is_same_v<remove_cv_t<T>, ClassTs>)...
    }};

    template<typename A, typename B>
    static constexpr array<bool, num_of_classes> contains_subobjs_A_and_B = {{
            (subobject_count_v<A>[ClassIs] != 0 && subobject_count_v<B>[ClassIs] != 0)...
    }};
    // -------------------------------------------------------------------
    // is_native_downcast_feasible — determines for which most derived
    // class types the following downcast via explicit can be done:
    // F* -> T* (or F& -> T&) where F is (direct/indirect) base of T.
    // Essentially it checks if given MDT, is every F a subobject of T
    // in MDT and is there atleast one T in MDT.
    // 
    // Example:
    // struct S1 {};
    // struct S2 : S1 {};
    // struct S3 : S1, S2 {};
    // 
    // Given MDT is S3, S1* -> S2* cannot be done via static_cast (even
    // though static_cast<S2*>(S1*) is well formed) because S1* could
    // point to S1 in S3 that is not a subobject of S2.
    // 
    // T must not be more cv qualified than F otherwise error as point
    // of this is to inform if we can handle cast from F -> T via explicit
    // cast alone (or to otherwise consider crosscast or extened downcast).
    // Returning false for cv-mismatch will take away that info.
    // -------------------------------------------------------------------
    template<typename F, typename T, bool>
    struct is_native_downcast_feasible_impl {
        static constexpr array<bool, num_of_classes> value = {};
    };

    template<typename F, typename T>
    struct is_native_downcast_feasible_impl<F, T, true> {
        // F is base of T AND explicit_cast is well-formed => F is unambiguous,
        // non-virtual, and not a subobject of a virtual object in T.

        // Thus, we can just count 'references' to bases F & T in MDT and check if they
        // are equal. If they are not, there exists a subobject F in MDT that is NOT
        // a subobject of T.

        static constexpr array<size_type, num_of_classes> subobj_T_cnt =
            subobject_count_v<T>;

        static constexpr array<size_type, num_of_classes> subobj_F_cnt =
            subobject_count_v<F>;

        static constexpr array<bool, num_of_classes> value = {{
            (subobj_T_cnt[ClassIs] && (subobj_T_cnt[ClassIs] == subobj_F_cnt[ClassIs]))...
        }};
    };

    template<typename F, typename T>
    struct is_native_downcast_feasible {
        static_assert(is_class_v<F> && is_class_v<T>, "source/target must be of class type");
        static_assert(sizeof(F) >= 0, "From is required to be a complete type");
        static_assert(sizeof(T) >= 0, "To is required to be a complete type");
        static_assert(!is_const_v<F> || is_const_v<T>, "cv qualifier mismatch");
        static_assert(!is_volatile_v<F> || is_volatile_v<T>, "cv qualifier mismatch");

        static constexpr bool F_bo_T = is_base_of_v<F, T>;
        static constexpr bool sc_wf = is_pointer_explicit_cast_ok_v<F*, T*>;

        static constexpr array<bool, num_of_classes> value =
            is_native_downcast_feasible_impl<F, T, sc_wf && F_bo_T>::value;
    };
    // -------------------------------------------------------------------
    template<typename F, typename... TestTs>
    struct contains_implies_native_downcastable {
        static_assert(is_class_v<F> && (is_class_v<TestTs> && ...),
                      "source/target types must be of class type");

        static_assert(sizeof(F) >= 0, "F must be a complete type");
        static_assert((sizeof(TestTs) + ... + 0) >= 0, "TestTs... must be complete");
        static_assert(!is_const_v<F> || (is_const_v<TestTs> && ...),  "cv mismatch");
        static_assert(!is_volatile_v<F> || (is_volatile_v<TestTs> && ...),  "cv mismatch");

        template<typename TT>
        static constexpr array<bool, num_of_classes> cast_feasible_arr =
            is_native_downcast_feasible<F, TT>::value;

        template<typename TT>
        static constexpr array<bool, num_of_classes> contains_F_and_T_arr =
            contains_subobjs_A_and_B<F, TT>;

        template<typename TT>
        static constexpr bool contains_implies_castable =
            ((!contains_F_and_T_arr<TT>[ClassIs] ||
              cast_feasible_arr<TT>[ClassIs]) && ...);

        static constexpr bool value =
            (contains_implies_castable<TestTs> && ...);
    };
    // -------------------------------------------------------------------
    template<typename L>
    struct all_paths_from_to_impl;

    template<typename T>
    struct tpl_identity_helper {
        template<typename To, typename...>
        using type = T;
    };

    template<typename... Ts>
    struct all_paths_from_to_impl<list<Ts...>> {
        template<typename To, typename... Trajectory>
        using type = concat_t<
            typename ternary<
                (is_same_v<To, Ts> || ...)
            >::template proxy_type<
                concat<
                    typename ternary<is_same_v<To, Ts>>::template type<
                        list<list<Trajectory..., To>>,
                        list<>
                    >...
                >,
                type_identity<list<>>
            >,

            typename ternary<
                !is_same_v<To, Ts>
            >::template type<
                all_paths_from_to_impl<
                    base_spec_list_of_t<Ts>
                >,
                tpl_identity_helper<list<>>
            >::template type<To, Trajectory..., Ts>...
        >;
    };

    template<typename F, typename To>
    using all_paths_from_to_t = 
        typename ternary<
            is_same_v<remove_cv_t<F>, remove_cv_t<To>>
        >::template type<
            tpl_identity_helper<list<list<remove_cv_t<F>>>>,
            all_paths_from_to_impl<
                base_spec_list_of_t<F>
            >
        >::template type<remove_cv_t<To>, remove_cv_t<F>>;
};
// -------------------------------------------------------------------------------------------------
template<typename HYCT, typename F, typename R, typename Focal, typename... PathsB>
inline bool ex_downcast_partb(F&& obj, Focal&& focal, R& result, list<PathsB...>*) {
    auto* from_ptr = addressof(obj);
    bool break_cycle =
        (((addressof(sequential_upcast<HYCT>(focal, static_cast<PathsB*>(nullptr))) == from_ptr) ?
          (result =  is_direct_base_walk_public<HYCT>(static_cast<PathsB*>(nullptr)) ?
           addressof(focal) : nullptr, true
          ) :
          false
         ) || ...
        );

    return break_cycle;
}

//PathsA: all paths from MDT -> To
//PathsB: all paths from To -> F
template<typename HYCT, typename F, typename MDT, typename... PathsA, typename... PathsB>
inline auto* ex_downcast(F&& obj, MDT&& md_obj, list<PathsA...>*, list<PathsB...>* pb) {
    match_cv_t<remove_reference_t<MDT>, void>* result = nullptr;
    [[maybe_unused]] bool d =
        (ex_downcast_partb<HYCT>(
            static_cast<F&&>(obj),
            sequential_upcast<HYCT>(static_cast<MDT&&>(md_obj),
                                       static_cast<PathsA*>(nullptr)),
            result,
            pb
         ) || ...
        );

    return result;
}

//PathsA: all paths from MDT -> F
//PathsB: all paths from MDT -> To
template<typename HYCT, typename F, typename MDT, typename... PathsA, typename... PathsB>
inline auto* crosscast(F&& obj, MDT&& md_obj, list<PathsA...>*, list<PathsB...>*) {
    auto* from_ptr = addressof(obj);
    bool obj_is_public_base =
        (((addressof(sequential_upcast<HYCT>(md_obj, static_cast<PathsA*>(nullptr))) == from_ptr &&
           is_direct_base_walk_public<HYCT>(static_cast<PathsA*>(nullptr))) ? true : false) || ...);

    bool any_public_pathsB = (is_direct_base_walk_public<HYCT>(static_cast<PathsB*>(nullptr)) || ...);

    match_cv_t<remove_reference_t<MDT>, void>* result = nullptr;

    if(obj_is_public_base && any_public_pathsB) {
        using first_path = nth_type_t<0, PathsB...>;
        auto* first_addr = addressof(sequential_upcast<HYCT>(
            md_obj,
            static_cast<first_path*>(nullptr)
        ));

        bool unambiguous = 
            ((addressof(sequential_upcast<HYCT>(
                md_obj,
                static_cast<PathsB*>(nullptr)
            )) == first_addr) && ...);

        if(unambiguous)
            result = first_addr;
    }

    return result;
}

template<typename HYCT, typename F, typename MDT, typename... PathsA>
inline auto* crosscast(F&& obj, MDT&& md_obj, list<PathsA...>*, list<>*) {
    return static_cast<match_cv_t<remove_reference_t<MDT>, void>*>(nullptr);
}

// casting::is_native_downcast_feasible requires class types
template<bool B, typename F>
struct native_downcast_feasible_result {
    using hierarchy = deduce_hierarchy_t<F>;

    template<typename T>
    static constexpr array<bool, hierarchy::num_of_classes> value = {};
};

template<typename F>
struct native_downcast_feasible_result<true, F> {
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<F>>;

    template<typename T>
    static constexpr array<bool, hutil::num_of_classes> value =
        hutil::casting::template is_native_downcast_feasible<F, T>::value;
};

template<typename F, typename T>
inline constexpr auto native_downcast_feasible_result_v =
    native_downcast_feasible_result<is_class_v<T>, F>::template value<T>;

template<size_type I, typename T, typename F>
inline T* general_cast_switch_case(F&& obj) {
    using F_wo_ref = remove_reference_t<F>;
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<F_wo_ref>>;
    using MDT = typename hutil::template class_from_index_t<I>;

    if constexpr(is_same_v<remove_cv_t<T>, void>) {
        static_assert(deduce_mode_v<F_wo_ref> == mode::barebones,
                      "other modes should be handled earlier as they have instance ptr");

        return instance_ptr_of<mode::barebones, F>::template f<I>(static_cast<F&&>(obj));
    }
    else if constexpr(!hutil::template contains_base_v<T, MDT>) {
        return nullptr;
    }
    // simple downcast?
    else if constexpr(native_downcast_feasible_result_v<F_wo_ref, T>[I]) {
        if constexpr(is_pointer_static_cast_ok_v<F_wo_ref*, T*>) {
            auto&& result = static_cast<match_ref_t<F&&, T>>(static_cast<F&&>(obj));
            return addressof(result);
        } else {
            return nullptr;
        }
    }
    // crosscast or extended downcast
    else if constexpr(deduce_mode_v<F_wo_ref> != mode::barebones) {
        void* instance_ptr = get_type_info(static_cast<F&&>(obj))._cavi_instance_ptr;

        using MDT = typename hutil::template class_from_index_t<I>;
        using MDT_cvr = match_ref_t<F&&, match_cv_t<F_wo_ref, MDT>>;
        MDT_cvr md_obj = static_cast<MDT_cvr>(*static_cast<MDT*>(instance_ptr));

        using paths_MDT_to_T = typename hutil::casting::template all_paths_from_to_t<MDT, T>;
        using paths_MDT_to_F = typename hutil::casting::template all_paths_from_to_t<MDT, F_wo_ref>;

        auto* ccast = crosscast<F_wo_ref>(
            static_cast<F&&>(obj),
            static_cast<MDT_cvr>(md_obj),
            static_cast<paths_MDT_to_F*>(nullptr),
            static_cast<paths_MDT_to_T*>(nullptr)
        );

        if(ccast) {
            return static_cast<T*>(ccast);
        }
        else if constexpr(is_base_of_v<F_wo_ref, T>) {
            using paths_T_to_F = typename hutil::casting::template all_paths_from_to_t<T, F_wo_ref>;
            return static_cast<T*>(ex_downcast<F_wo_ref>(
                static_cast<F&&>(obj),
                static_cast<MDT_cvr>(md_obj),
                static_cast<paths_MDT_to_T*>(nullptr),
                static_cast<paths_T_to_F*>(nullptr)
            ));
        }
    }

    return nullptr;
}

template<typename T, typename F, typename Is>
struct general_cast_vtable;

template<typename T, typename F, size_type... Is>
struct general_cast_vtable<T, F, index_sequence<Is...>> {
    using F_wo_ref = remove_reference_t<F>;
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<F_wo_ref>>;

    static constexpr size_type always_valid_mdt = hutil::template class_index_v<F_wo_ref>;
    static constexpr array<bool, hutil::num_of_classes> mdt_survey =
        hutil::template contains_base_survey<F_wo_ref>;

    using fn_type = T* (*)(F&&);
    static constexpr array<fn_type, hutil::num_of_classes> vtable = {
        &general_cast_switch_case<
            ternary<mdt_survey[Is]>::template value<
                size_type,
                Is,
                always_valid_mdt
            >,
            T,
            F
        >...
    };
};

template<typename T, typename F>
 inline T* general_cast(F&& obj) {
    using F_wo_ref = remove_reference_t<F>;
    using hierarchy = deduce_hierarchy_t<F_wo_ref>;
    using hutil = hierarchy_utility_t<hierarchy>;

    constexpr bool cv_ok = 
        (!is_const_v<F_wo_ref> || is_const_v<T>) &&
        (!is_volatile_v<F_wo_ref> || is_volatile_v<T>);

    if(!cv_ok) {
        return nullptr;
    }
    else if constexpr(is_base_of_v<T, F_wo_ref>) {
        if constexpr(is_pointer_static_cast_ok_v<F_wo_ref*, T*>) {
            auto&& result = static_cast<match_ref_t<F&&, T>>(static_cast<F&&>(obj));
            return addressof(result);
        } else {
            return nullptr;
        }
    }
    else if constexpr(is_same_v<remove_cv_t<T>, void> &&
                      deduce_mode_v<F_wo_ref> != mode::barebones) {
        return static_cast<T*>(get_type_info(static_cast<F&&>(obj))._cavi_instance_ptr);
    }
    else {
        size_type class_id = get_type_info(static_cast<F&&>(obj))._cavi_type_id;

        constexpr const auto& vtable = general_cast_vtable<
            T,
            F,
            make_index_sequence<hierarchy::num_of_classes>
        >::vtable;

        // reduce most derived type possibilities
        constexpr array<bool, hierarchy::num_of_classes> mdt_survey =
            hutil::template contains_base_survey<F_wo_ref>;

    #define CAVI_GENERAL_CAST_CASE(num, UNUSED1, UNUSED2)                                         \
        case num:                                                                                 \
        if constexpr(num < hierarchy::num_of_classes) {                                           \
            if constexpr(mdt_survey[num])                                                         \
                return vtable[num](static_cast<F&&>(obj));                                        \
        }                                                                                         \
        CAVI_UNREACHABLE;

        if constexpr(hierarchy::num_of_classes <= 16) {
            switch(class_id) {
                CAVI_REPEAT_NUM_HANDLER_16(CAVI_GENERAL_CAST_CASE, 0, 0, 0)
                default: CAVI_UNREACHABLE;
            }
        }
        else if constexpr(hierarchy::num_of_classes <= 64) {
            switch(class_id) {
                CAVI_REPEAT_NUM_HANDLER_64(CAVI_GENERAL_CAST_CASE, 0, 0, 0)
                default: CAVI_UNREACHABLE;
            }
        }
        else if constexpr(hierarchy::num_of_classes <= 256) {
            switch(class_id) {
                CAVI_REPEAT_NUM_HANDLER_256(CAVI_GENERAL_CAST_CASE, 0, 0, 0)
                default: CAVI_UNREACHABLE;
            }
        }
        else {
            return vtable[class_id](static_cast<F&&>(obj));
        }
    }
}
// -------------------------------------------------------------------------------------------------
// casting::contains_implies_native_downcastable requires class types (i.e. resolves for void)
template<bool B, typename F>
struct contains_implies_native_downcastable_check {
    template<typename... TestTs>
    static constexpr bool value = false;
};

template<typename F>
struct contains_implies_native_downcastable_check<true, F> {
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<F>>;

    template<typename... TestTs>
    static constexpr bool value = 
        hutil::casting::template contains_implies_native_downcastable<
            F,
            TestTs...
        >::value;
};

template<typename F, typename... TestTs>
inline constexpr bool contains_implies_native_downcastable_v =
    contains_implies_native_downcastable_check<
        (is_class_v<TestTs> && ...),
        F
    >::template value<TestTs...>;

template<size_type N1, size_type N2>
inline constexpr array<bool, N1>
isa_accessible_contains_disjunction(
    const array<bool, N2>& accessibility_arr,
    const array<const array<bool, N1>, N2>& contains_arr) 
{
    array<bool, N1> res = {};
    for(size_type i = 0; i < N1; i++) {
        for(size_type j = 0; j < N2; j++) {
            if(accessibility_arr[j] && contains_arr[j][i]) {
                res[i] = true;
                break;
            }
        }
    }
    return res;
}

template<typename... TestTs, typename F>
inline bool can_do_native_downcast_to_any(F&& obj) {
    using F_wo_ref = remove_reference_t<F>;
    using hutil = hierarchy_utility_t<deduce_hierarchy_t<F_wo_ref>>;

    constexpr array<bool, sizeof...(TestTs)> accessible_arr = {{
        is_pointer_static_cast_ok_v<F_wo_ref*, TestTs*>...
    }};

    using unit_t = const array<bool, hutil::num_of_classes>;
    constexpr array<unit_t, sizeof...(TestTs)> contains_F_and_TTs_arr = {{
        hutil::casting::template contains_subobjs_A_and_B<F_wo_ref, TestTs>...
    }};

    constexpr array<bool, hutil::num_of_classes> contains_any =
        isa_accessible_contains_disjunction(accessible_arr, contains_F_and_TTs_arr);

    constexpr auto contains_any_bits =
        bits_packer<hutil::num_of_classes>(contains_any);

    size_type class_id = get_type_info(static_cast<F&&>(obj))._cavi_type_id;
    return contains_any_bits.test_bit(class_id);
}

template<typename T, typename F>
inline T* dyn_cast_impl(F&& obj) {
    using F_wo_ref = remove_reference_t<F>;
    call_hierarchy_verified<F_wo_ref>();

    // upcast is also handled by general_cast but that is not ill-formed for an inaccessible/
    // ambiguous base, rather, it is supposed to return nullptr as it is used by isa() aswell.
    if constexpr(is_base_of_v<T, F_wo_ref>) {
        if constexpr(is_pointer_static_cast_ok_v<F_wo_ref*, T*>) {
            auto&& result = static_cast<match_ref_t<F&&, T>>(static_cast<F&&>(obj));
            return addressof(result);
        } else {
            static_assert(dependent_false<T>, "target is inaccessible/ambigious base of source");
        }
    }
    // optimization for simple downcast casts that can be handled by native explicit cast
    // rather than calling more expensive general_cast.
    else if constexpr(contains_implies_native_downcastable_v<F_wo_ref, T>) {
        if(can_do_native_downcast_to_any<T>(static_cast<F&&>(obj))) {
            auto&& result = (match_ref_t<F&&, T>)(static_cast<F&&>(obj));
            return addressof(result);
        } else {
            return nullptr;
        }
    } 
    else {
        return general_cast<T>(static_cast<F&&>(obj));
    }
}

template<typename... TestTs, typename F>
inline bool isa_impl(F&& obj) {
    using F_wo_ref = remove_reference_t<F>;
    call_hierarchy_verified<F_wo_ref>();
    static_assert(sizeof...(TestTs) > 0, "must have atleast one target test type");

    constexpr bool upcast_can_be_done = 
        ((is_base_of_v<TestTs, F_wo_ref> &&
            is_pointer_static_cast_ok_v<F_wo_ref*, TestTs*>
            ) || ...
        );

    if constexpr(upcast_can_be_done) {
        return true;
    }
    else if constexpr((is_same_v<remove_cv_t<TestTs>, void> || ...)) {
        return true;
    }
    else if constexpr(contains_implies_native_downcastable_v<F_wo_ref, TestTs...>) {
        return can_do_native_downcast_to_any<TestTs...>(static_cast<F&&>(obj));
    }
    else {
        return ((general_cast<TestTs>(static_cast<F&&>(obj)) != nullptr) || ...);
    }
}
// -------------------------------------------------------------------------------------------------
template<typename F, typename T>
struct cast_arg_unwrap {
    static_assert(dependent_false<F>, "target and source must be of reference/pointer type");
};

template<typename F, typename T, typename = void>
struct cast_arg_check {
    using hierarchy = deduce_hierarchy_t<F>;
    using hutil = hierarchy_utility_t<hierarchy>;

    static_assert((!is_const_v<F> || is_const_v<T>),
                  "target type loses const qualifier");

    static_assert((!is_volatile_v<F> || is_volatile_v<T>),
                  "target type loses volatile qualifier");
    
    static_assert(is_class_v<F> && is_class_v<T>, 
                  "source and target must be of complete class type");

    static_assert((sizeof(F) + sizeof(T)) >= 0,
                  "source and target must be of complete class type");

    static_assert(hutil::template is_class_declared_v<T>,
                  "target type is not declared in the hierarchy OR"
                  "source and target type are not in the same hierarchy");

    using type = T;
};

template<typename F, typename T>
struct cast_arg_check<F, T, enable_if_t<is_same_v<remove_cv_t<T>, void>>> {
    static_assert((!is_const_v<F> || is_const_v<T>), 
                  "target type loses const qualifier");

    static_assert((!is_volatile_v<F> || is_volatile_v<T>), 
                  "target type loses volatile qualifier");

    static_assert((sizeof(F) >= 0 && is_class_v<F>),
                  "source must be of complete class type");

    using type = T;
};

template<typename F, typename T> struct cast_arg_unwrap<F&, T&> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F&, T&&> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F&, T*> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F&, T* const> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F&, T* volatile> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F&, T* const volatile> : cast_arg_check<F, T> {};


template<typename F, typename T> struct cast_arg_unwrap<F&&, T&> {
    static_assert(dependent_false<F>, "target type is of lvalue reference, "
                  "source must be a lvalue of complete class type");
};

template<typename F, typename T> struct cast_arg_unwrap<F&&, T&&> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F&&, T*> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F&&, T* const> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F&&, T* volatile> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F&&, T* const volatile> : cast_arg_check<F, T> {};


template<typename F, typename T> struct cast_arg_unwrap<F*, T&> {
    static_assert(dependent_false<F>, "target type is of lvalue reference, "
                  "source must be a lvalue of complete class type");
};

template<typename F, typename T> struct cast_arg_unwrap<F*, T&&> {
    static_assert(dependent_false<F>, "target type is of rvalue reference, "
                  "source must be a lvalue or xvalue of complete class type");
};

template<typename F, typename T> struct cast_arg_unwrap<F*, T*> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F*, T* const> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F*, T* volatile> : cast_arg_check<F, T> {};
template<typename F, typename T> struct cast_arg_unwrap<F*, T* const volatile> : cast_arg_check<F, T> {};

template<typename... TestTs, typename F>
inline bool isa(F&& obj) noexcept {
    return isa_impl<typename cast_arg_unwrap<F&&, TestTs>::type...>(
        static_cast<F&&>(obj)
    );
}

template<typename... TestTs, typename F>
inline bool isa(F* obj) noexcept {
    if(obj != nullptr) {
        return isa_impl<typename cast_arg_unwrap<F*, TestTs>::type...>(*obj);
    } else {
        return false;
    }
}

[[noreturn]] void throw_bad_cast();

template<typename T, typename F>
inline T cast(F&& obj) {
    auto* ptr = dyn_cast_impl<typename cast_arg_unwrap<F&&, T>::type>(
        static_cast<F&&>(obj)
    );

    if constexpr(is_reference_v<T>) {
        if(ptr)
            return static_cast<T>(*ptr);
        else
            cavi::throw_bad_cast();
    } else {
        return ptr;
    }
}

template<typename T, typename F>
inline T cast(F* obj) noexcept {
    if(obj != nullptr) {
        return dyn_cast_impl<typename cast_arg_unwrap<F*, T>::type>(*obj);
    } else {
        return nullptr;
    }
}

} // namespace cavi

#endif
