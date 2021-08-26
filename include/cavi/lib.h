//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CAVI_HEADER_LIB
#define CAVI_HEADER_LIB

#ifndef __has_builtin
#define CAVI_HAS_BUILTIN(x) 0
#else
#define CAVI_HAS_BUILTIN(x) __has_builtin(x)
#endif

#ifdef __GNUC__
#define GCC_VERSION __GNUC__
#else
#define CAVI_GCC_VERSION 0
#endif

#include <climits>

#if !(defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER))
#include <type_traits>
#endif

#if defined(__clang__) && !CAVI_HAS_BUILTIN(__make_integer_seq) && !defined(_MSC_VER)
# include <utility>
#elif defined(__GNUC__) && !(CAVI_GCC_VERSION > 7)
# include <utility>
#elif !defined(_MSC_VER)
# include <utility>
#endif

#if defined(__clang__) || defined(__GNUC__)
#define CAVI_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define CAVI_UNREACHABLE __assume(false)
#else
#define CAVI_UNREACHABLE
#endif

namespace cavi {
using size_type = decltype(sizeof(char));
// -------------------------------------------------------------------------------------------------
#if defined(__clang__) || (defined(__GNUC__) && CAVI_GCC_VERSION >= 10)
template<typename T1, typename T2>
inline constexpr bool is_same_v = __is_same(T1, T2);
#else
template<typename T1, typename T2> inline constexpr bool is_same_v = false;
template<typename T> inline constexpr bool is_same_v<T, T> = true;
#endif
// -------------------------------------------------------------------------------------------------
template<typename T, typename... Ts>
inline constexpr bool is_same_as_any_of_v = (is_same_v<T, Ts> || ...);
// -------------------------------------------------------------------------------------------------
#if defined(__clang__)
template<typename T>
inline constexpr bool is_const_v = __is_const(T);
#else
template<typename T> inline constexpr bool is_const_v = false;
template<typename T> inline constexpr bool is_const_v<const T> = true;
#endif
// -------------------------------------------------------------------------------------------------
#if defined(__clang__)
template<typename T>
inline constexpr bool is_volatile_v = __is_volatile(T);
#else
template<typename T> inline constexpr bool is_volatile_v = false;
template<typename T> inline constexpr bool is_volatile_v<volatile T> = true;
#endif
// -------------------------------------------------------------------------------------------------
#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER)
template<typename T>
inline constexpr bool is_class_v = __is_class(T);
#else
template<typename T>
inline constexpr bool is_class_v = std::is_class_v<T>;
#endif
// -------------------------------------------------------------------------------------------------
template<typename T> inline constexpr bool is_reference_v = false;
template<typename T> inline constexpr bool is_reference_v<T&> = true;
template<typename T> inline constexpr bool is_reference_v<T&&> = true;
// -------------------------------------------------------------------------------------------------
#if defined(__clang__)
template<typename T>
inline constexpr bool is_pointer_v = __is_pointer(T);
#else
template<typename T> inline constexpr bool is_pointer_v = false;
template<typename T> inline constexpr bool is_pointer_v<T*> = true;
template<typename T> inline constexpr bool is_pointer_v<T* const> = true;
template<typename T> inline constexpr bool is_pointer_v<T* volatile> = true;
template<typename T> inline constexpr bool is_pointer_v<T* const volatile> = true;
#endif
// -------------------------------------------------------------------------------------------------
template<typename T> struct remove_const { using type = T; };
template<typename T> struct remove_const<const T> { using type = T; };
template<typename T> using remove_const_t = typename remove_const<T>::type;
// -------------------------------------------------------------------------------------------------
template<typename T> struct remove_volatile { using type = T; };
template<typename T> struct remove_volatile<volatile T> { using type = T; };
template<typename T> using remove_volatile_t = typename remove_volatile<T>::type;
// -------------------------------------------------------------------------------------------------
template<typename T> struct remove_cv { using type = T; };
template<typename T> struct remove_cv<const T> { using type = T; };
template<typename T> struct remove_cv<volatile T> { using type = T; };
template<typename T> struct remove_cv<const volatile T>  { using type = T; };
template<typename T> using remove_cv_t = typename remove_cv<T>::type;
// -------------------------------------------------------------------------------------------------
template<typename T> struct remove_reference { using type = T; };
template<typename T> struct remove_reference<T&> { using type = T; };
template<typename T> struct remove_reference<T&&> { using type = T; };
template<typename T> using remove_reference_t = typename remove_reference<T>::type;
// -------------------------------------------------------------------------------------------------
template<typename T> struct remove_cvref { using type = remove_cv_t<remove_reference_t<T>>; };
template<typename T> using remove_cvref_t = remove_cv_t<remove_reference_t<T>>;
// -------------------------------------------------------------------------------------------------
template<typename T> struct remove_pointer { using type = T; };
template<typename T> struct remove_pointer<T*> { using type = T; };
template<typename T> struct remove_pointer<T* const> { using type = T; };
template<typename T> struct remove_pointer<T* volatile> { using type = T; };
template<typename T> struct remove_pointer<T* const volatile> { using type = T; };
template<typename T> using remove_pointer_t = typename remove_pointer<T>::type;
// -------------------------------------------------------------------------------------------------
template<typename... Ts>
using void_t = void;
// -------------------------------------------------------------------------------------------------
template<bool B, typename T = void>
struct enable_if {};

template<typename T>
struct enable_if<true, T> {
    using type = T;
};

template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;
// -------------------------------------------------------------------------------------------------
template<typename T, typename = void>
struct add_lvalue_reference { using type = T; };

template<typename T>
struct add_lvalue_reference<T, void_t<T&>> { using type = T&; };

template<typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
// -------------------------------------------------------------------------------------------------
template<typename T, typename = void>
struct add_rvalue_reference { using type = T; };

template<typename T>
struct add_rvalue_reference<T, void_t<T&&>> { using type = T&&; };

template<typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
// -------------------------------------------------------------------------------------------------
template<typename Of, typename To>
struct match_cv { using type = remove_cv_t<To>; };

template<typename Of, typename To>
struct match_cv<const Of, To> { using type = const remove_cv_t<To>; };

template<typename Of, typename To>
struct match_cv<volatile Of, To> { using type = volatile remove_cv_t<To>; };

template<typename Of, typename To>
struct match_cv<const volatile Of, To> { using type = const volatile To; };

template<typename Of, typename To>
using match_cv_t = typename match_cv<Of, To>::type;
// -------------------------------------------------------------------------------------------------
template<typename Of, typename To>
struct match_ref { using type = To; };

template<typename Of, typename To>
struct match_ref<Of, To&> { using type = To; };

template<typename Of, typename To>
struct match_ref<Of, To&&> { using type = To; };

template<typename Of, typename To>
struct match_ref<Of&, To> { using type = add_lvalue_reference_t<To>; };

template<typename Of, typename To>
struct match_ref<Of&&, To> { using type = add_rvalue_reference_t<To>; };

template<typename Of, typename To>
struct match_ref<Of&&, To&> { using type = add_rvalue_reference_t<To>; };

template<typename Of, typename To>
using match_ref_t = typename match_ref<Of, To>::type;
// -------------------------------------------------------------------------------------------------
template<typename Of, typename To>
using match_cvref_t = match_ref_t<Of, match_cv_t<remove_reference_t<Of>, To>>;
// -------------------------------------------------------------------------------------------------
template<typename T>
typename add_rvalue_reference<T>::type declval() noexcept;
// -------------------------------------------------------------------------------------------------
#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER)
template <typename B, typename D>
inline constexpr bool is_base_of_v = __is_base_of(B, D);
#else
template <typename B, typename D>
inline constexpr bool is_base_of_v = std::is_base_of_v<B, D>;
#endif
// -------------------------------------------------------------------------------------------------
template<typename T, T v>
struct integral_constant { static constexpr T value = v; };

template<bool v>
using bool_constant = integral_constant<bool, v>;

using false_type = bool_constant<false>;
using true_type = bool_constant<true>;
// -------------------------------------------------------------------------------------------------
#if defined(__clang__) || defined(_MSC_VER) || (defined(__GNUC__) && CAVI_GCC_VERSION >= 7)
template<typename T>
inline constexpr T* addressof(T& rp) noexcept
{
    return __builtin_addressof(rp);
}
#else
template<typename T>
inline T* addressof(T& rp) noexcept
{
    return
        reinterpret_cast<T*>(
            &const_cast<char&>(
                reinterpret_cast<const volatile char&>(rp)
            )
        );
}
#endif
// ----------------------------------------------------------------
#if (defined(__clang__) && CAVI_HAS_BUILTIN(__make_integer_seq)) || defined(_MSC_VER)
template<typename T, T... Is>
struct integer_sequence {};

template<typename T, T N>
using make_integer_sequence =  __make_integer_seq<integer_sequence, T, N>;

#elif defined(__GNUC__) && GCC_VERSION > 7
template<typename T, T... Is>
struct integer_sequence {};

template<typename T, T N>
using make_integer_sequence =  integer_sequence<T, __integer_pack(N)...>;

#else
template<typename T, T... Is>
using integer_sequence = std::integer_sequence<T, Is...>;

template<typename T, T N>
using make_integer_sequence = std::make_integer_sequence<T, N>;
#endif

template<size_type... Is>
using index_sequence = integer_sequence<size_type, Is...>;

template<size_type N>
using make_index_sequence = make_integer_sequence<size_type, N>;
// -------------------------------------------------------------------------------------------------
template <typename... Ts>
inline constexpr bool dependent_false = false;
// -------------------------------------------------------------------------------------------------
template<typename T>
struct type_identity {
    using type = T;
};

template<typename T>
using type_identity_t = typename type_identity<T>::type;
// -------------------------------------------------------------------------------------------------
template<typename T, size_type N>
struct array {
    static_assert(N > 0, "array of zero size");

    T _data[N];

    constexpr T& operator[](size_type pos) noexcept {
        return _data[pos];
    }

    constexpr const T& operator[](size_type pos) const noexcept {
        return _data[pos];
    }
};
// -------------------------------------------------------------------------------------------------
template<size_type N_of_bits>
class bits_packer {
    static_assert(N_of_bits != 0, "no bits to pack");

    static constexpr size_type bits_per_unit = sizeof(size_type) * CHAR_BIT;
    static constexpr size_type rem = N_of_bits % bits_per_unit;
    static constexpr size_type N_of_units = (N_of_bits / bits_per_unit) + (rem ? 1 : 0);

    array<size_type, N_of_units> _packed_units ={};

public:
    constexpr bits_packer(const array<bool, N_of_bits>& bits) {
        for(size_type i = 0; i < N_of_units; i++) {
            size_type begin_index = bits_per_unit * i;
            size_type bits_left = N_of_bits - begin_index;
            size_type bits_to_set = (bits_left >= bits_per_unit) ? bits_per_unit : bits_left;

            for(size_type j = 0; j < bits_to_set; j++)
                _packed_units[i] |= size_type{(bits[begin_index+j])} << j;
        }
    }

    constexpr bool test_bit(size_type bit_index) const {
        if constexpr(N_of_units == 1) {
            return _packed_units[0] & (size_type(1) << bit_index);
        }
        else {
            size_type unit_index = bit_index / bits_per_unit;
            size_type unit_bit_index = bit_index % bits_per_unit;
            return _packed_units[unit_index] & (size_type(1) << unit_bit_index);
        }
    }
};
// -------------------------------------------------------------------------------------------------
template <typename F, typename T, typename = void>
struct is_pointer_static_cast_ok_impl {
    static constexpr bool value = false;
};

template <typename F, typename T>
struct is_pointer_static_cast_ok_impl<
    F*,
    T*,
    void_t<decltype(static_cast<T*>((F*)nullptr))>>
{
    static constexpr bool value = true;
};

template<typename F, typename T>
struct is_pointer_static_cast_ok {
    static_assert(sizeof(F) >= 0 && sizeof(T) >= 0,
                  "F & T must be a complete type");

    static_assert(is_pointer_v<F> && is_pointer_v<T>,
                  "F & T must be pointer types");

    static constexpr bool value = is_pointer_static_cast_ok_impl<
        remove_cv_t<F>,
        remove_cv_t<T>
    >::value;
};

// F & T must be pointer types (to complete types)
template<typename F, typename T>
inline constexpr bool is_pointer_static_cast_ok_v =
    is_pointer_static_cast_ok<F, T>::value;
// -------------------------------------------------------------------------------------------------
template <typename F, typename T, typename = void>
struct is_pointer_explicit_cast_ok_impl {
    static constexpr bool value = false;
};

template <typename F, typename T>
struct is_pointer_explicit_cast_ok_impl<
    F*,
    T*,
    void_t<decltype((T*)((F*)nullptr))>>
{
    static constexpr bool value = true;
};

template<typename F, typename T>
struct is_pointer_explicit_cast_ok {
    static_assert(sizeof(F) >= 0 && sizeof(T) >= 0,
                  "F & T must be a complete type");

    static_assert(is_pointer_v<F> && is_pointer_v<T>,
                  "F & T must be pointer types");

    static constexpr bool value = is_pointer_explicit_cast_ok_impl<
        remove_cv_t<F>,
        remove_cv_t<T>
    >::value;
};

// F & T must be pointer types (to complete types)
template<typename F, typename T>
inline constexpr bool is_pointer_explicit_cast_ok_v =
    is_pointer_explicit_cast_ok<F, T>::value;

} // namespace cavi

#endif
