//          Copyright Baber Nawaz 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef CAVI_HEADER_MP
#define CAVI_HEADER_MP

#include "lib.h"

namespace cavi {
template<typename... Ts>
struct list;
// -------------------------------------------------------------------------------------------------
template<typename L>
struct length;

template<typename... Ts>
struct length<list<Ts...>> {
    static constexpr size_type value = sizeof...(Ts);
};

template<typename L>
inline constexpr size_type length_v = length<L>::value;
// -------------------------------------------------------------------------------------------------
template<typename L>
struct has;

template<typename... Ts>
struct has<list<Ts...>> {
    template<typename T>
    static constexpr bool value = (is_same_v<T, Ts> || ...);
};

template<typename L, typename T>
inline constexpr bool has_v = has<L>::template value<T>;
// -------------------------------------------------------------------------------------------------
template<typename L>
struct count;

template<typename... Ts>
struct count<list<Ts...>> {
    template<typename T>
    static constexpr size_type value = (is_same_v<T, Ts> + ... + 0);
};

template<typename L, typename T>
static constexpr size_type count_v = count<L>::template value<T>;
// -------------------------------------------------------------------------------------------------
inline constexpr list<>* l0p = static_cast<list<>*>(nullptr);

template<size_type num>
struct concat_helper;

inline constexpr size_type concat_select(size_type N) {
    return
        N <= 8 ? 0 :
        N <= 32 ? 1 :
        N <= 128 ? 2 : 3;
}

// concat (N <= 8)
template<>
struct concat_helper<0> {
    template<
        typename... Ts1, typename... Ts2, typename... Ts3, typename... Ts4, typename... Ts5,
        typename... Ts6, typename... Ts7, typename... Ts8>
    static list<
        Ts1..., Ts2..., Ts3..., Ts4..., Ts5..., Ts6..., Ts7..., Ts8...>
    func(
        list<Ts1...>* = l0p, list<Ts2...>* = l0p, list<Ts3...>* = l0p, list<Ts4...>* = l0p,
        list<Ts5...>* = l0p, list<Ts6...>* = l0p, list<Ts7...>* = l0p, list<Ts8...>* = l0p);
};

// concat (N <= 32)
template<>
struct concat_helper<1> {
    template<
        typename... Ts1, typename... Ts2, typename... Ts3, typename... Ts4, typename... Ts5,
        typename... Ts6, typename... Ts7, typename... Ts8, typename... Ts9, typename... Ts10,
        typename... Ts11, typename... Ts12, typename... Ts13, typename... Ts14, typename... Ts15,
        typename... Ts16, typename... Ts17, typename... Ts18, typename... Ts19, typename... Ts20,
        typename... Ts21, typename... Ts22, typename... Ts23, typename... Ts24, typename... Ts25,
        typename... Ts26, typename... Ts27, typename... Ts28, typename... Ts29, typename... Ts30,
        typename... Ts31, typename... Ts32>
    static list<
        Ts1..., Ts2..., Ts3..., Ts4..., Ts5..., Ts6..., Ts7..., Ts8..., Ts9..., Ts10..., Ts11...,
        Ts12..., Ts13..., Ts14..., Ts15..., Ts16..., Ts17..., Ts18..., Ts19..., Ts20..., Ts21...,
        Ts22..., Ts23..., Ts24..., Ts25..., Ts26..., Ts27..., Ts28..., Ts29..., Ts30..., Ts31...,
        Ts32...>
    func(
        list<Ts1...>* = l0p, list<Ts2...>* = l0p, list<Ts3...>* = l0p, list<Ts4...>* = l0p,
        list<Ts5...>* = l0p, list<Ts6...>* = l0p, list<Ts7...>* = l0p, list<Ts8...>* = l0p,
        list<Ts9...>* = l0p, list<Ts10...>* = l0p, list<Ts11...>* = l0p, list<Ts12...>* = l0p,
        list<Ts13...>* = l0p, list<Ts14...>* = l0p, list<Ts15...>* = l0p, list<Ts16...>* = l0p,
        list<Ts17...>* = l0p, list<Ts18...>* = l0p, list<Ts19...>* = l0p, list<Ts20...>* = l0p,
        list<Ts21...>* = l0p, list<Ts22...>* = l0p, list<Ts23...>* = l0p, list<Ts24...>* = l0p,
        list<Ts25...>* = l0p, list<Ts26...>* = l0p, list<Ts27...>* = l0p, list<Ts28...>* = l0p,
        list<Ts29...>* = l0p, list<Ts30...>* = l0p, list<Ts31...>* = l0p, list<Ts32...>* = l0p);
};

// concat (N <= 128)
template<>
struct concat_helper<2> {
    template<
        typename... Ts1, typename... Ts2, typename... Ts3, typename... Ts4, typename... Ts5,
        typename... Ts6, typename... Ts7, typename... Ts8, typename... Ts9, typename... Ts10,
        typename... Ts11, typename... Ts12, typename... Ts13, typename... Ts14, typename... Ts15,
        typename... Ts16, typename... Ts17, typename... Ts18, typename... Ts19, typename... Ts20,
        typename... Ts21, typename... Ts22, typename... Ts23, typename... Ts24, typename... Ts25,
        typename... Ts26, typename... Ts27, typename... Ts28, typename... Ts29, typename... Ts30,
        typename... Ts31, typename... Ts32, typename... Ts33, typename... Ts34, typename... Ts35,
        typename... Ts36, typename... Ts37, typename... Ts38, typename... Ts39, typename... Ts40,
        typename... Ts41, typename... Ts42, typename... Ts43, typename... Ts44, typename... Ts45,
        typename... Ts46, typename... Ts47, typename... Ts48, typename... Ts49, typename... Ts50,
        typename... Ts51, typename... Ts52, typename... Ts53, typename... Ts54, typename... Ts55,
        typename... Ts56, typename... Ts57, typename... Ts58, typename... Ts59, typename... Ts60,
        typename... Ts61, typename... Ts62, typename... Ts63, typename... Ts64, typename... Ts65,
        typename... Ts66, typename... Ts67, typename... Ts68, typename... Ts69, typename... Ts70,
        typename... Ts71, typename... Ts72, typename... Ts73, typename... Ts74, typename... Ts75,
        typename... Ts76, typename... Ts77, typename... Ts78, typename... Ts79, typename... Ts80,
        typename... Ts81, typename... Ts82, typename... Ts83, typename... Ts84, typename... Ts85,
        typename... Ts86, typename... Ts87, typename... Ts88, typename... Ts89, typename... Ts90,
        typename... Ts91, typename... Ts92, typename... Ts93, typename... Ts94, typename... Ts95,
        typename... Ts96, typename... Ts97, typename... Ts98, typename... Ts99, typename... Ts100,
        typename... Ts101, typename... Ts102, typename... Ts103, typename... Ts104, typename... Ts105,
        typename... Ts106, typename... Ts107, typename... Ts108, typename... Ts109, typename... Ts110,
        typename... Ts111, typename... Ts112, typename... Ts113, typename... Ts114, typename... Ts115,
        typename... Ts116, typename... Ts117, typename... Ts118, typename... Ts119, typename... Ts120,
        typename... Ts121, typename... Ts122, typename... Ts123, typename... Ts124, typename... Ts125,
        typename... Ts126, typename... Ts127, typename... Ts128>
    static list<
        Ts1..., Ts2..., Ts3..., Ts4..., Ts5..., Ts6..., Ts7..., Ts8..., Ts9..., Ts10..., Ts11...,
        Ts12..., Ts13..., Ts14..., Ts15..., Ts16..., Ts17..., Ts18..., Ts19..., Ts20..., Ts21...,
        Ts22..., Ts23..., Ts24..., Ts25..., Ts26..., Ts27..., Ts28..., Ts29..., Ts30..., Ts31...,
        Ts32..., Ts33..., Ts34..., Ts35..., Ts36..., Ts37..., Ts38..., Ts39..., Ts40..., Ts41...,
        Ts42..., Ts43..., Ts44..., Ts45..., Ts46..., Ts47..., Ts48..., Ts49..., Ts50..., Ts51...,
        Ts52..., Ts53..., Ts54..., Ts55..., Ts56..., Ts57..., Ts58..., Ts59..., Ts60..., Ts61...,
        Ts62..., Ts63..., Ts64..., Ts65..., Ts66..., Ts67..., Ts68..., Ts69..., Ts70..., Ts71...,
        Ts72..., Ts73..., Ts74..., Ts75..., Ts76..., Ts77..., Ts78..., Ts79..., Ts80..., Ts81...,
        Ts82..., Ts83..., Ts84..., Ts85..., Ts86..., Ts87..., Ts88..., Ts89..., Ts90..., Ts91...,
        Ts92..., Ts93..., Ts94..., Ts95..., Ts96..., Ts97..., Ts98..., Ts99..., Ts100..., Ts101...,
        Ts102..., Ts103..., Ts104..., Ts105..., Ts106..., Ts107..., Ts108..., Ts109..., Ts110...,
        Ts111..., Ts112..., Ts113..., Ts114..., Ts115..., Ts116..., Ts117..., Ts118..., Ts119...,
        Ts120..., Ts121..., Ts122..., Ts123..., Ts124..., Ts125..., Ts126..., Ts127..., Ts128...>
    func(
        list<Ts1...>* = l0p, list<Ts2...>* = l0p, list<Ts3...>* = l0p, list<Ts4...>* = l0p,
        list<Ts5...>* = l0p, list<Ts6...>* = l0p, list<Ts7...>* = l0p, list<Ts8...>* = l0p,
        list<Ts9...>* = l0p, list<Ts10...>* = l0p, list<Ts11...>* = l0p, list<Ts12...>* = l0p,
        list<Ts13...>* = l0p, list<Ts14...>* = l0p, list<Ts15...>* = l0p, list<Ts16...>* = l0p,
        list<Ts17...>* = l0p, list<Ts18...>* = l0p, list<Ts19...>* = l0p, list<Ts20...>* = l0p,
        list<Ts21...>* = l0p, list<Ts22...>* = l0p, list<Ts23...>* = l0p, list<Ts24...>* = l0p,
        list<Ts25...>* = l0p, list<Ts26...>* = l0p, list<Ts27...>* = l0p, list<Ts28...>* = l0p,
        list<Ts29...>* = l0p, list<Ts30...>* = l0p, list<Ts31...>* = l0p, list<Ts32...>* = l0p,
        list<Ts33...>* = l0p, list<Ts34...>* = l0p, list<Ts35...>* = l0p, list<Ts36...>* = l0p,
        list<Ts37...>* = l0p, list<Ts38...>* = l0p, list<Ts39...>* = l0p, list<Ts40...>* = l0p,
        list<Ts41...>* = l0p, list<Ts42...>* = l0p, list<Ts43...>* = l0p, list<Ts44...>* = l0p,
        list<Ts45...>* = l0p, list<Ts46...>* = l0p, list<Ts47...>* = l0p, list<Ts48...>* = l0p,
        list<Ts49...>* = l0p, list<Ts50...>* = l0p, list<Ts51...>* = l0p, list<Ts52...>* = l0p,
        list<Ts53...>* = l0p, list<Ts54...>* = l0p, list<Ts55...>* = l0p, list<Ts56...>* = l0p,
        list<Ts57...>* = l0p, list<Ts58...>* = l0p, list<Ts59...>* = l0p, list<Ts60...>* = l0p,
        list<Ts61...>* = l0p, list<Ts62...>* = l0p, list<Ts63...>* = l0p, list<Ts64...>* = l0p,
        list<Ts65...>* = l0p, list<Ts66...>* = l0p, list<Ts67...>* = l0p, list<Ts68...>* = l0p,
        list<Ts69...>* = l0p, list<Ts70...>* = l0p, list<Ts71...>* = l0p, list<Ts72...>* = l0p,
        list<Ts73...>* = l0p, list<Ts74...>* = l0p, list<Ts75...>* = l0p, list<Ts76...>* = l0p,
        list<Ts77...>* = l0p, list<Ts78...>* = l0p, list<Ts79...>* = l0p, list<Ts80...>* = l0p,
        list<Ts81...>* = l0p, list<Ts82...>* = l0p, list<Ts83...>* = l0p, list<Ts84...>* = l0p,
        list<Ts85...>* = l0p, list<Ts86...>* = l0p, list<Ts87...>* = l0p, list<Ts88...>* = l0p,
        list<Ts89...>* = l0p, list<Ts90...>* = l0p, list<Ts91...>* = l0p, list<Ts92...>* = l0p,
        list<Ts93...>* = l0p, list<Ts94...>* = l0p, list<Ts95...>* = l0p, list<Ts96...>* = l0p,
        list<Ts97...>* = l0p, list<Ts98...>* = l0p, list<Ts99...>* = l0p, list<Ts100...>* = l0p,
        list<Ts101...>* = l0p, list<Ts102...>* = l0p, list<Ts103...>* = l0p, list<Ts104...>* = l0p,
        list<Ts105...>* = l0p, list<Ts106...>* = l0p, list<Ts107...>* = l0p, list<Ts108...>* = l0p,
        list<Ts109...>* = l0p, list<Ts110...>* = l0p, list<Ts111...>* = l0p, list<Ts112...>* = l0p,
        list<Ts113...>* = l0p, list<Ts114...>* = l0p, list<Ts115...>* = l0p, list<Ts116...>* = l0p,
        list<Ts117...>* = l0p, list<Ts118...>* = l0p, list<Ts119...>* = l0p, list<Ts120...>* = l0p,
        list<Ts121...>* = l0p, list<Ts122...>* = l0p, list<Ts123...>* = l0p, list<Ts124...>* = l0p,
        list<Ts125...>* = l0p, list<Ts126...>* = l0p, list<Ts127...>* = l0p, list<Ts128...>* = l0p);
};

// concat (N > 128 Recursive)
template<>
struct concat_helper<3> {
    template<
        typename... Ts1, typename... Ts2, typename... Ts3, typename... Ts4, typename... Ts5,
        typename... Ts6, typename... Ts7, typename... Ts8, typename... Ts9, typename... Ts10,
        typename... Ts11, typename... Ts12, typename... Ts13, typename... Ts14, typename... Ts15,
        typename... Ts16, typename... Ts17, typename... Ts18, typename... Ts19, typename... Ts20,
        typename... Ts21, typename... Ts22, typename... Ts23, typename... Ts24, typename... Ts25,
        typename... Ts26, typename... Ts27, typename... Ts28, typename... Ts29, typename... Ts30,
        typename... Ts31, typename... Ts32, typename... Ts33, typename... Ts34, typename... Ts35,
        typename... Ts36, typename... Ts37, typename... Ts38, typename... Ts39, typename... Ts40,
        typename... Ts41, typename... Ts42, typename... Ts43, typename... Ts44, typename... Ts45,
        typename... Ts46, typename... Ts47, typename... Ts48, typename... Ts49, typename... Ts50,
        typename... Ts51, typename... Ts52, typename... Ts53, typename... Ts54, typename... Ts55,
        typename... Ts56, typename... Ts57, typename... Ts58, typename... Ts59, typename... Ts60,
        typename... Ts61, typename... Ts62, typename... Ts63, typename... Ts64, typename... Ts65,
        typename... Ts66, typename... Ts67, typename... Ts68, typename... Ts69, typename... Ts70,
        typename... Ts71, typename... Ts72, typename... Ts73, typename... Ts74, typename... Ts75,
        typename... Ts76, typename... Ts77, typename... Ts78, typename... Ts79, typename... Ts80,
        typename... Ts81, typename... Ts82, typename... Ts83, typename... Ts84, typename... Ts85,
        typename... Ts86, typename... Ts87, typename... Ts88, typename... Ts89, typename... Ts90,
        typename... Ts91, typename... Ts92, typename... Ts93, typename... Ts94, typename... Ts95,
        typename... Ts96, typename... Ts97, typename... Ts98, typename... Ts99, typename... Ts100,
        typename... Ts101, typename... Ts102, typename... Ts103, typename... Ts104, typename... Ts105,
        typename... Ts106, typename... Ts107, typename... Ts108, typename... Ts109, typename... Ts110,
        typename... Ts111, typename... Ts112, typename... Ts113, typename... Ts114, typename... Ts115,
        typename... Ts116, typename... Ts117, typename... Ts118, typename... Ts119, typename... Ts120,
        typename... Ts121, typename... Ts122, typename... Ts123, typename... Ts124, typename... Ts125,
        typename... Ts126, typename... Ts127, typename... Ts128, typename... R>
    static auto func(
        list<Ts1...>* = l0p, list<Ts2...>* = l0p, list<Ts3...>* = l0p, list<Ts4...>* = l0p,
        list<Ts5...>* = l0p, list<Ts6...>* = l0p, list<Ts7...>* = l0p, list<Ts8...>* = l0p,
        list<Ts9...>* = l0p, list<Ts10...>* = l0p, list<Ts11...>* = l0p, list<Ts12...>* = l0p,
        list<Ts13...>* = l0p, list<Ts14...>* = l0p, list<Ts15...>* = l0p, list<Ts16...>* = l0p,
        list<Ts17...>* = l0p, list<Ts18...>* = l0p, list<Ts19...>* = l0p, list<Ts20...>* = l0p,
        list<Ts21...>* = l0p, list<Ts22...>* = l0p, list<Ts23...>* = l0p, list<Ts24...>* = l0p,
        list<Ts25...>* = l0p, list<Ts26...>* = l0p, list<Ts27...>* = l0p, list<Ts28...>* = l0p,
        list<Ts29...>* = l0p, list<Ts30...>* = l0p, list<Ts31...>* = l0p, list<Ts32...>* = l0p,
        list<Ts33...>* = l0p, list<Ts34...>* = l0p, list<Ts35...>* = l0p, list<Ts36...>* = l0p,
        list<Ts37...>* = l0p, list<Ts38...>* = l0p, list<Ts39...>* = l0p, list<Ts40...>* = l0p,
        list<Ts41...>* = l0p, list<Ts42...>* = l0p, list<Ts43...>* = l0p, list<Ts44...>* = l0p,
        list<Ts45...>* = l0p, list<Ts46...>* = l0p, list<Ts47...>* = l0p, list<Ts48...>* = l0p,
        list<Ts49...>* = l0p, list<Ts50...>* = l0p, list<Ts51...>* = l0p, list<Ts52...>* = l0p,
        list<Ts53...>* = l0p, list<Ts54...>* = l0p, list<Ts55...>* = l0p, list<Ts56...>* = l0p,
        list<Ts57...>* = l0p, list<Ts58...>* = l0p, list<Ts59...>* = l0p, list<Ts60...>* = l0p,
        list<Ts61...>* = l0p, list<Ts62...>* = l0p, list<Ts63...>* = l0p, list<Ts64...>* = l0p,
        list<Ts65...>* = l0p, list<Ts66...>* = l0p, list<Ts67...>* = l0p, list<Ts68...>* = l0p,
        list<Ts69...>* = l0p, list<Ts70...>* = l0p, list<Ts71...>* = l0p, list<Ts72...>* = l0p,
        list<Ts73...>* = l0p, list<Ts74...>* = l0p, list<Ts75...>* = l0p, list<Ts76...>* = l0p,
        list<Ts77...>* = l0p, list<Ts78...>* = l0p, list<Ts79...>* = l0p, list<Ts80...>* = l0p,
        list<Ts81...>* = l0p, list<Ts82...>* = l0p, list<Ts83...>* = l0p, list<Ts84...>* = l0p,
        list<Ts85...>* = l0p, list<Ts86...>* = l0p, list<Ts87...>* = l0p, list<Ts88...>* = l0p,
        list<Ts89...>* = l0p, list<Ts90...>* = l0p, list<Ts91...>* = l0p, list<Ts92...>* = l0p,
        list<Ts93...>* = l0p, list<Ts94...>* = l0p, list<Ts95...>* = l0p, list<Ts96...>* = l0p,
        list<Ts97...>* = l0p, list<Ts98...>* = l0p, list<Ts99...>* = l0p, list<Ts100...>* = l0p,
        list<Ts101...>* = l0p, list<Ts102...>* = l0p, list<Ts103...>* = l0p, list<Ts104...>* = l0p,
        list<Ts105...>* = l0p, list<Ts106...>* = l0p, list<Ts107...>* = l0p, list<Ts108...>* = l0p,
        list<Ts109...>* = l0p, list<Ts110...>* = l0p, list<Ts111...>* = l0p, list<Ts112...>* = l0p,
        list<Ts113...>* = l0p, list<Ts114...>* = l0p, list<Ts115...>* = l0p, list<Ts116...>* = l0p,
        list<Ts117...>* = l0p, list<Ts118...>* = l0p, list<Ts119...>* = l0p, list<Ts120...>* = l0p,
        list<Ts121...>* = l0p, list<Ts122...>* = l0p, list<Ts123...>* = l0p, list<Ts124...>* = l0p,
        list<Ts125...>* = l0p, list<Ts126...>* = l0p, list<Ts127...>* = l0p, list<Ts128...>* = l0p,
        R* ... Rp) ->
    decltype(concat_helper<concat_select(sizeof...(R)+1)>::func(
        static_cast<list<
            Ts1..., Ts2..., Ts3..., Ts4..., Ts5..., Ts6..., Ts7..., Ts8..., Ts9..., Ts10...,
            Ts11..., Ts12..., Ts13..., Ts14..., Ts15..., Ts16..., Ts17..., Ts18..., Ts19...,
            Ts20..., Ts21..., Ts22..., Ts23..., Ts24..., Ts25..., Ts26..., Ts27..., Ts28...,
            Ts29..., Ts30..., Ts31..., Ts32..., Ts33..., Ts34..., Ts35..., Ts36..., Ts37...,
            Ts38..., Ts39..., Ts40..., Ts41..., Ts42..., Ts43..., Ts44..., Ts45..., Ts46...,
            Ts47..., Ts48..., Ts49..., Ts50..., Ts51..., Ts52..., Ts53..., Ts54..., Ts55...,
            Ts56..., Ts57..., Ts58..., Ts59..., Ts60..., Ts61..., Ts62..., Ts63..., Ts64...,
            Ts65..., Ts66..., Ts67..., Ts68..., Ts69..., Ts70..., Ts71..., Ts72..., Ts73...,
            Ts74..., Ts75..., Ts76..., Ts77..., Ts78..., Ts79..., Ts80..., Ts81..., Ts82...,
            Ts83..., Ts84..., Ts85..., Ts86..., Ts87..., Ts88..., Ts89..., Ts90..., Ts91...,
            Ts92..., Ts93..., Ts94..., Ts95..., Ts96..., Ts97..., Ts98..., Ts99..., Ts100...,
            Ts101..., Ts102..., Ts103..., Ts104..., Ts105..., Ts106..., Ts107..., Ts108...,
            Ts109..., Ts110..., Ts111..., Ts112..., Ts113..., Ts114..., Ts115..., Ts116...,
            Ts117..., Ts118..., Ts119..., Ts120..., Ts121..., Ts122..., Ts123..., Ts124...,
            Ts125..., Ts126..., Ts127..., Ts128...
        >*>(nullptr),
        Rp...
    ));
};

template<typename... Ts>
struct concat {
    static constexpr size_type N = sizeof...(Ts);
    using type = decltype(
        concat_helper<concat_select(N)>::func(static_cast<Ts*>(nullptr)...)
    );
};

template<typename... Ts>
using concat_t = typename concat<Ts...>::type;
// -------------------------------------------------------------------------------------------------
template<bool V>
struct ternary {
    template<typename TrueType, typename FalseType>
    using type = FalseType;

    template<typename TrueTrait, typename FalseTrait>
    using proxy_type = typename FalseTrait::type;

    template<typename T, T TrueValue, T FalseValue>
    static constexpr T value = FalseValue;

    template<typename T, typename TrueValueTrait, typename FalseValueTrait>
    static constexpr T proxy_value = FalseValueTrait::value;

};

template<>
struct ternary<true> {
    template<typename TrueType, typename FalseType>
    using type = TrueType;

    template<typename TrueTrait, typename FalseTrait>
    using proxy_type = typename TrueTrait::type;

    template<typename T, T TrueValue, T FalseValue>
    static constexpr T value = TrueValue;

    template<typename T, typename TrueValueTrait, typename FalseValueTrait>
    static constexpr T proxy_value = TrueValueTrait::value;
};
// -------------------------------------------------------------------------------------------------
template<typename T, typename Is>
struct repeat;

template<typename T, bool>
using repeat_helper_t = T;

template<typename T, size_type... Is>
struct repeat<T, index_sequence<Is...>> {
    using type = list<repeat_helper_t<T, static_cast<bool>(Is+1)>...>;
};

template<typename T, size_type N>
using repeat_t = typename repeat<T, make_index_sequence<N>>::type;
// -------------------------------------------------------------------------------------------------
template<typename Is, typename... Ts>
struct types_index_mapping;

template<typename T, size_type O>
struct type_ordinal_pair {
    using type = T;
    static constexpr size_type ordinal = O;
};

template<size_type... Is, typename... Ts>
struct types_index_mapping<index_sequence<Is...>, Ts...> : type_ordinal_pair<Ts, Is>... {
    template<typename T, size_type I>
    static type_ordinal_pair<T, I> select_ti(type_ordinal_pair<T, I>*);

    template<size_type I, typename T>
    static type_ordinal_pair<T, I> select_it(type_ordinal_pair<T, I>*);

    template<size_type I>
    using nth_type = typename decltype(
        select_it<I>(static_cast<types_index_mapping*>(nullptr))
    )::type;

    template<typename T>
    static constexpr size_type index_of = decltype(
        select_ti<T>(static_cast<types_index_mapping*>(nullptr))
    )::ordinal;
};

#ifdef __clang__
template<size_type I, typename... Ts>
using nth_type_t = __type_pack_element<I, Ts...>;
#else
template<size_type I, typename... Ts>
using nth_type_t = typename types_index_mapping<
    make_index_sequence<sizeof...(Ts)>,
    Ts...
>::template nth_type<I>;
#endif

// Ts... must to be distinct
template<typename T, typename... Ts>
inline constexpr size_type index_of_v = types_index_mapping<
    make_index_sequence<sizeof...(Ts)>,
    Ts...
>::template index_of<T>;
// -------------------------------------------------------------------------------------------------
template<typename... Ts>
struct typeset_contains_helper : type_identity<Ts>... {};

template<typename T, typename... Ts>
inline constexpr bool typeset_contains_v =
    is_base_of_v<type_identity<T>, typeset_contains_helper<Ts...>>;

} // namespace cavi

#endif
