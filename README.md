# cavi
cavi is an open-source library that aims to provide performant utilities for closed hierarchies (i.e. all class types of the hierarchy are known at compile time). 

Requires C++17 or above.


1. [Utilities](https://github.com/bnbajwa/cavi#utilities)
2. [Visitation](https://github.com/bnbajwa/cavi#visitation)
3. [Dynami Casting](https://github.com/bnbajwa/cavi#dynamic-casting)
4. [Supported Hierarchies](https://github.com/bnbajwa/cavi#supported-hierarchies)
5. [Hierarchy Setup](https://github.com/bnbajwa/cavi#hierarchy-setup)
6. [Code Reuse / Libraries](https://github.com/bnbajwa/cavi#code-reuse--libraries)
7. [Additional Notes](https://github.com/bnbajwa/cavi#additional-notes)
8. [To Do List](https://github.com/bnbajwa/cavi#to-do-list)

## Utilities
Currently the library comes with the following utilities:
- Hierarchy-aware visitation
- Fast dynamic casting

## Visitation
### cavi::visit

```c++
template<typename Visitor, typename... Os>
decltype(auto) visit(Visitor&& visitor, Os&& ... os)
```
- The return type is deduced by decltype
- All invocations of the visitor must have the same return type and value category

<br/>

```c++
template<typename R, typename Visitor, typename... Os>
R visit(Visitor&& visitor, Os&& ... os)
```
- The return type is R
- The result of all invocations of the visitor must be implicitly convertible to R 
- If R is (possible cv-qualified) void, then the value returned by the visitor invocation is discarded

<br/>

__For both of the above:__
- Applies the visitor to the objects os...
- None of the objects os... may be pointer values (ill-formed otherwise)
- Objects os... must be of complete type
- No exceptions are thrown if and only if the visitor does not throw any exceptions
- The runtime time complexity is constant
- Let S(x) be the set containing all types of the hierarchy that the object x belongs to. Let N := ```|S(os)| * ... * 1```, where |A| is the size of the set A.
Then, for N <= 256, the implementation uses a switch statement; otherwise a dispatch table is used.  The (N=256) threshold can easily be increased or decreased by modfying the library source but increasing it means longer compile times.
- Let Z(x) be the set consisting of x and all classes that inherit — directly or indirectly — object x as a base class. Let set W be defined as follows:
  - If ```sizeof...(os) == 0```, then let W := ```{}```,
  - If ```sizeof...(os) == 1```, then let W := ```Z(os)```.
  - Otherwise, let W := ```Z(os) × ...```, where ```A × B``` is the cartesian product of sets A and B.
 
  If the set W is empty, then the __only__ invocation of the visitor is one with no arguments. Otherwise, the visitor is invoked __only__ for arguments of type(s) and form in set W. See [Example 2](https://github.com/bnbajwa/cavi#example-2) for further reading.
- The behaviour of _cavi::visit_ invoked with an object undergoing construction/destruction (or invoked with any of its base class subobjects) is as follows:
  - If _cavi::visit_ is used during destruction, the behaviour is undefined
  - _cavi::visit_ can be used in a constructor (including the mem-initializer or default member initializer when either is used for a non-static data member — if used in a mem-initializer that initializes a base class subobject, the behaviour is undefined). When _cavi::visit_ is used in a constructor, the most derived object is considered to be the constructors class type and is pointed to by the this pointer. If the operand of _cavi::visit_ is not the said object considered to be the most derived or any of its base class subobjects, the behaviour is undefined.

### Example 1

Suppose you had the following hierarchy for a console variables implementation (often found in video games):
```c++
struct var {
protected:
    var() = default;
    var(const var&) = default;
    var(var&&) = default;
};

struct var_string : var {
    std::string value;
    var_string(std::string val) : value(std::move(val)) {}
};

struct var_vector2 : var { 
    float x, y;
    var_vector2(float x_, float y_) : x(x_), y(y_) {}
};

struct var_arith : var {
protected:
    var_arith() = default;
    var_arith(const var_arith&) = default;
    var_arith(var_arith&&) = default;
};

struct var_u32 : var_arith {
    uint32_t value;
    var_u32(uint32_t val) : value(val) {}
};

struct var_flt : var_arith {
    float value;
    var_flt(float val) : value(val) {}
};
```
After the setup for the hierarchy, visitation can be done in the same fashion as std::visit :
```c++
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

int main() {
    var_u32 v1{150};
    var_flt v2{3.14f};
    var_string v3{"Hello World"};
    var_vector2 v4{1.5f, 3.0f};

    print_var(v1);
    print_var(v2);
    print_var(v3);
    print_var(v4);
    return 0;
}
```
Alternatively, the overload pattern popular with std::visit can also be used:
```c++
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void print_var(const var& vr) {
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
```
Output:
```
150
3.14
Hello World
(1.5, 3)
```
### Example 2
I mentioned above that the visitation is "hierarchy-aware" — this means that the visitation utility uses the information it has about the hierarchy aswell the type of the input object to reduce the set of types for which the visitor has to provide an implementation for.

For example, suppose you have a reference/pointer value of type ```var_arith```. Then the most derived object type possibilites are ```var_arith``` itself or it's derived classes, and so from the hierarchy above this gives us the set: ```{var_arith, var_u32, var_flt}```. Therefore, a visitor only has to provide implementation for these three possibilites. Also note that the most derived type can't be ```var_arith``` either as it can't be instantiated, however, this is not possible for the visitation utility to deduce from the information it has access to.

To illustrate, the following function compiles:
```c++
std::string arith_to_string_representation(const var_arith& r) {
    return cavi::visit(overloaded {
        [](const var_arith&) { assert(false); return std::string(""); },
        [](const var_u32& v) { return std::to_string(v.value); },
        [](const var_flt& v) { return std::to_string(v.value); },
        [](auto&& v) { static_assert(cavi::dependent_false<decltype(v)>, "unhandled type"); }
    }, r);
}
```
Hierarchy-aware visitation also extends into multiple dispatch. The set of possibilities that the visitor has to cover is the cartesian product of all of the sets of possibilities. E.g. suppose that in addition to a reference/pointer of type ```var_arith``` as above, you also have a reference/pointer of type ```var_string```. The set of most derived object type possibilities for latter is just ```{var_string}``` as ```var_string``` has no derived classes. Applying a visitor to both of the aforementioned objects would therefore require the visitor to cover the cartesian product of the two sets:<br/>```{var_arith, var_u32, var_flt}``` __×__ ```{var_string}``` = ```{(var_arith, var_string), (var_u32, var_string), (var_flt, var_string)}``` 

Again, to illustrate, the following function compiles as it covers all combinations from the set:
```c++
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
```

## Dynamic Casting
### cavi::cast

```c++
template<typename T, typename F>
T cast(F&& obj)
```

```c++
template<typename T, typename F>
T cast(F* obj) noexcept
```

The specification is the same as of __dynamic_cast__ in the C++17 standard except for some differences:
- If you are casting to a pointer type, then __dynamic_cast__ requires that the input is also a pointer, whereas _cavi::cast_ permits non-pointer inputs so the nullptr check can be avoided. Example:

    ```c++
    void do_downcast(var_arith& from) {
        var_u32* p1 = cavi::cast<var_u32*>(from); // OK
        var_u32* p2 = dynamic_cast<var_u32*>(from); // ill-formed
    }
    ```
- For downcasts and crosscasts __dynamic_cast__ requires that the input object is of polymorphic type (i.e. for which std::is_polymorphic is true), whereas _cavi::cast_ does not have this requirement
- A failed cast to a reference type via __dynamic_cast__ throws an exception of a type that matches a handler of type ```std::bad_cast```, whereas _cavi::cast_ calls a user defined function in namespace __cavi__ declared as:
  ```c++
  [[noreturn]] void throw_bad_cast();
  ```
  You must define this function once in a translation unit if you want to cast to reference types via _cavi::cast_, otherwise you will get a linker error. Example:
  ```c++
  //some_file.cpp
  #include <typeinfo>
  namespace cavi {
      [[noreturn]] void throw_bad_cast() {
          throw std::bad_cast{};
      }
  }
  ```
- The behaviour of _cavi::cast_ invoked with an object undergoing construction/destruction (or invoked with any of its base class subobjects) is as follows:
  - If _cavi::cast_ is used during destruction, the behaviour is undefined
  - _cavi::cast_ can be used in a constructor (including the mem-initializer or default member initializer when either is used for a non-static data member — if used in a mem-initializer that initializes a base class subobject, the behaviour is undefined). When _cavi::cast_ is used in a constructor, the most derived object is considered to be the constructors class type and is pointed to by the _this_ pointer. If the operand of _cavi::cast_ is not the said object considered to be the most derived or any of its base class subobjects, the behaviour is undefined.

### cavi::isa
```c++
template<typename... TestTs, typename F>
bool isa(F&& obj) noexcept
```

```c++
template<typename... TestTs, typename F>
bool isa(F* obj) noexcept
```
Returns true if and only if _cavi::cast_ will succeed in casting the input object to _one or more_ of the TestTs... types. It is the equivalent of invoking _cavi::cast\<TestTs\>_ for all TestTs... and testing if one of them succeeds (although, it is implemented more efficiently). Therefore, the specification _cavi::cast_ applies here as it is just a series of of calls to _cavi::cast_, except for one key difference: if any of the TestTs... types are reference types, ```[[noreturn]] void throw_bad_cast()``` is not called.

### Generated Assembly
Consider the following function that tests if we can cast a ```var*``` to a ```var_u32*```:
```c++
var* get_var();
int main() {
    var* v = get_var();
    if(cavi::isa<var_u32*>(v)) {
        printf("isa: true\n");
    }
    return 0;
}
```
Generated assembly on clang 10.0 with ```-O3``` and ```-flto``` (same result with ```-flto=thin``` instead of ```-flto```). MSVC 2019 with ```/O2``` and ```/GL``` also generates the same. GCC 9.3 also generates the same for the important part i've commented on with ```-O3``` and ```-flto```.
```asm
50                     push    rax
E8 1A 00 00 00         call    _Z7get_varv
48 85 C0               test    rax, rax            ; test v == nullptr
74 0F                  jz      short loc_4011AA    ; conditional jump on result
80 38 04               cmp     byte ptr [rax], 4   ; test v->type_id == 4
75 0A                  jnz     short loc_4011AA    ; conditional jmp on result
BF 04 20 40 00         mov     edi, offset s
E8 B6 FE FF FF         call    _puts
                  loc_4011AA:
31 C0                  xor     eax, eax
59                     pop     rcx
C3                     retn
```
The nullptr check can avoided if you know the ptr is not null by dereferencing: ```if(cavi::isa<var_u32*>(*v))```, then the important part from above reduces to:
```asm
80 38 04               cmp     byte ptr [rax], 4
75 0C                  jnz     short loc_14000124A
```

Code is changed to test if we can cast to ```var_arith*``` which is more difficult as you can no longer just do one comparison of the type_id. And thats because if the most derived type was any of ```{var_arith, var_u32, var_flt}```, they could all be casted to ```var_arith*```. It now becomes a bit test, generated assembly for Clang and GCC:
```asm
0F B6 00               movzx   eax, byte ptr [rax]
B9 34 00 00 00         mov     ecx, 38h
48 0F A3 C1            bt      rcx, rax
73 05                  jnb     short loc_14000101C
```

MSVC:
```asm
0F B6 08               movzx   ecx, byte ptr [rax]
B8 01 00 00 00         mov     eax, 1
48 D3 E0               shl     rax, cl
A8 34                  test    al, 38h
74 0C                  jz      short loc_140001254
```

Code is changed to test multiple types at once: ```if(cavi::isa<var_string*, var_vector2*, var_u32*>(*v))```, then the generated assembly is again a bit test on all three compilers. Clang:
```asm
0F B6 00               movzx   eax, byte ptr [rax]
B9 2A 00 00 00         mov     ecx, 2Ah       
48 0F A3 C1            bt      rcx, rax
73 05                  jnb     short loc_14000101C
```

Example of typical assembly generated from __cavi::cast__, involving a type check (compare or bit test) + pointer adjustment.
```asm
48 89 C6               mov     rsi, rax
48 8D 56 E8            lea     rdx, [rsi-18h]
31 C0                  xor     eax, eax
80 7E 08 06            cmp     byte ptr [rsi], 6
48 0F 45 D0            cmovnz  rdx, rax
```

The majority of _cavi::isa_ and _cavi::cast_ calls will transform to assembly similar to above, infact for a single inheritance hierarchy with no virtual bases, _every_ single one will be of the above form. However, not every cast can be so simply done on multiple inheritance hierarchies — crosscasts and a subset of downcasts require more assembly to do them correctly.

## Supported Hierarchies
All closed hierarchies are supported except for those with:
1. \>256 classes
2. Virtual bases
3. Ambiguous direct bases

In regards to _(1)_ and _(2)_, the limit of 256 classes is due to a macro (CAVI_INSTANTIATE), however, it can easily be increased or reduced to any arbitrary number. It's just that increasing it leads to slighly higher compile times and thus 256 was chosen as the sweet spot. And support for hierarchies with virtual base classes is on the to-do list. 

Moreover, in regards to _(3)_, here is an example of such:
```c++
struct A {};
struct B : A {};
struct C : A, B {};
```
The direct base class A of C cannot be accessed from C due to ambiguity. However, it can be made non-ambiguous via a proxy class:
```c++
struct A {};
struct B : A {};
struct proxyA : A {};
struct C : proxyA, B {};
```
Now, it is accessible via the route: C → proxyA → A. Also, note that inaccessible in this context means purely in terms of ambiguity and not whether the base class is inherited publicly or not.

## Hierarchy Setup
The hierarchy from [Example 1](https://github.com/bnbajwa/hello-world#example-1) will be used to demonstrate the setup. 

### Step 1
Create a new header file for your hierarchy and include the header: ```cavi/core.h```. Declare the classes of your hierarchy and their base classes using the template type ```cavi::hierarchy_decl```. Lastly, use either the ```CAVI_DEF_BAREBONES_BASE``` or ```CAVI_DEF_STANDARD_BASE``` macro to define a base class for your hierarchy.

```var_HY.h```
```c++
#include "cavi/core.h"

using var_HY = cavi::hierarchy_decl<
    cavi::class_<struct var>,
    cavi::class_<struct var_string, var>,
    cavi::class_<struct var_vector2, var>,
    cavi::class_<struct var_arith, var>,
    cavi::class_<struct var_u32, var_arith>,
    cavi::class_<struct var_flt, var_arith>
>;

CAVI_DEF_BAREBONES_BASE(var_HY, var_HYB)
```
<br/>

To clarify, the ```CAVI_DEF_BAREBONES_BASE``` and ```CAVI_DEF_STANDARD_BASE``` macros define a templated base class that the classes in the hierarchy will inherit from in step 2. The first parameter of the macros is the hierarchy declaration and second is the name of the base that will be defined.
<br/>

The type of hierarchies they support:
 - The barebones base works only on single inheritance hierarchies (with no virtual bases)
 - The standard base works on single and multiple inheritance hierarchies (with no virtual bases)

The templated base class defined by the macro can be empty or have data members — it only has data members when it is inherited by a class with __zero__ base classes. For example, in the _var_ hierarchy above, only ```struct var``` will inherit a base that has data members as it has zero base classes. All others classes will inherit an empty base which will not add any extra size due to the empty base class optimization. 

The difference in data members of the bases:
   - The barebones base has only one data member and that is an integral type_id. The type of this integral id could be any of ```{unsigned char, unsigned short, unsigned long, unsigned long long}``` depending the range it needs to cover. E.g. if you have up to 256 classes in the hierarchy then the id will fit in ```unsigned char``` and require only 1 byte in size
   - The standard base has integral type_id + void* pointer. The type of the integral type_id is determined in the same way as in the barebones case

### Step 2
Now for every class in the hierarchy, include the relevant header file created in step 1 and inherit from the base class (publicly and non-virtually) at the end of the base list. The template parameter for the base is the class inherting the base itself. Like so:

```var.h```
```c++
#include "var_HY.h"
struct var : var_HYB<var> {
protected:
    var() = default;
    var(const var&) = default;
    var(var&&) = default;
};

struct var_string : var, var_HYB<var_string> {
    std::string value;
    var_string(std::string val) : value(std::move(val)) {}
};

struct var_vector2 : var, var_HYB<var_vector2> {
    float x, y;
    var_vector2(float x_, float y_) : x(x_), y(y_) {}
};

struct var_arith : var, var_HYB<var_arith> {
protected:
    var_arith() = default;
    var_arith(const var_arith&) = default;
    var_arith(var_arith&&) = default;
};

struct var_u32 : var_arith, var_HYB<var_u32> {
    uint32_t value;
    var_u32(uint32_t val) : value(val) {}
};

struct var_flt : var_arith, var_HYB<var_flt> {
    float value;
    var_flt(float val) : value(val) {}
};
```
<br/>

Make sure that the base class is the last one in the base list as that is important. If you would really like you can also inherit the base as protected/private, but then you need to make some relevant machinery of _cavi_ a friend via the ```CAVI_MAKE_FRIEND``` macro like so:
```c++
struct var_string : var, private var_HYB<var_string> {
    CAVI_MAKE_FRIEND;
    std::string value;
    var_string(std::string val) : value(std::move(val)) {}
};
```

### Step 3
Finally, make a new cpp file for your hierarchy, include the header ```cavi/instantiate.h``` and include the headers of all the classes in the hierarchy so this translation unit has the definition of all classes in the hierarchy. And, invoke the ```CAVI_INSTANTIATE``` macro with the hierarchy declaration and the base class. Like so (var.h has the definitions of the var_* structs):

```var_HY.cpp```
```c++
#include "cavi/instantiate.h"
#include "var.h"

CAVI_INSTANTIATE(var_HY, var_HYB)
```
And that's it. 

Include ```cavi/visit.h``` when you need to use _cavi::visit_ and ```cavi/cast.h``` for _cavi::cast_ and _cavi::isa_.

### What to look out for
There are compile time checks in place to check for incorrect setup but not everything can be checked for correctness. 

#### Step 1
Make sure the hierarchy declaration is correct as its almost impossible to verify

#### Step 2
Mistakes that are caught at compile time: 
- If you put in the wrong type as the template parameter to the base
- If any type listed in the hierarchy declaration does not inherit the hierarchy base
- If you inherit the base non-publicy and do not make use of the ```CAVI_MAKE_FRIEND``` macro

Mistakes that aren't caught:
- Hierarchy base class is not placed at the end of the base list

#### Step 3
This step completely checked, if it not done or done wrongly, it results in a compiler or linker error.

## Code Reuse / Libraries
What if you wanted to turn the above hierarchy and functionality with it into a library, so that you or others could reuse it in other projects?  We would need a way to allow new types to be added to the hierarchy and to allow new operations/functionality. Here's how it could be done with _cavi_.

### Making The Library

Let's start with library itself first and lets call it 'convar'. Since we don't know the full hierarchy from a library point of view, we can't yet define the base for the hierarchy with the macros. That also means we can't inherit from a cavi generated base because we dont have a base defined yet. We have to make use of templates:

```convar_types.h```
```c++
namespace convar {
template<template <typename> class HYB>
struct var : HYB<var<HYB>> {
protected:
    var() = default;
    var(const var&) = default;
    var(var&&) = default;
};

template<template <typename> class HYB>
struct var_string : var<HYB>, HYB<var_string<HYB>> {
    std::string value;
    var_string(std::string val) : value(std::move(val)) {}
};

template<template <typename> class HYB>
struct var_vector2 : var<HYB>, HYB<var_vector2<HYB>> {
    float x, y;
    var_vector2(float x_, float y_) : x(x_), y(y_) {}
};

template<template <typename> class HYB>
struct var_arith : var<HYB>, HYB<var_arith<HYB>> {
protected:
    var_arith() = default;
    var_arith(const var_arith&) = default;
    var_arith(var_arith&&) = default;
};

template<template <typename> class HYB>
struct var_u32 : var_arith<HYB>, HYB<var_u32<HYB>> {
    uint32_t value;
    var_u32(uint32_t val) : value(val) {}
};

template<template <typename> class HYB>
struct var_flt : var_arith<HYB>, HYB<var_flt<HYB>> {
    float value;
    var_flt(float val) : value(val) {}
};
}
```
Now, you want to update your hierarchy decl to take into account the templates. The user will use this to extend the hierarchy if he needs to.

```convar_hierarchy.h```
```c++
#include "cavi/core.h"

namespace convar {
template<template <typename> class HYB> struct var;
template<template <typename> class HYB> struct var_string;
template<template <typename> class HYB> struct var_vector2;
template<template <typename> class HYB> struct var_vector2;
template<template <typename> class HYB> struct var_arith;
template<template <typename> class HYB> struct var_u32;
template<template <typename> class HYB> struct var_flt;

using convar_hierarchy = cavi::hierarchy_decl<
    cavi::class_tpl<var>,
    cavi::class_tpl<var_string, var>,
    cavi::class_tpl<var_vector2, var>,
    cavi::class_tpl<var_arith, var>,
    cavi::class_tpl<var_u32, var_arith>,
    cavi::class_tpl<var_flt, var_arith>
>;
}
```

Lastly, the functionality that is to be provided with the library needs to be written in such a way so that it can work with new types it does not know about. That is a rich topic in itself — lookup customization points on a search engine to read more. For example, _std::hash_ supports user-defined types through class template specialization. I will give an example of using argument-dependent lookup (ADL) for customization.

```convar_lib.h```
```c++
#include "cavi/visit.h"
#include "convar_types.h"

namespace convar {
template<typename TV>
void print_var_impl(const TV& v) {
    using T = cavi::remove_volatile_t<TV>;
    if constexpr(is_specialization_of_any_of_v<T, var, var_arith>) {
        assert(false);
    }
    else if constexpr(is_specialization_of_v<T, var_vector2>) {
        std::cout << '(' << v.x << ", " << v.y << ")\n";
    }
    else if constexpr(is_specialization_of_any_of_v<T, var_string, var_u32, var_flt>) {
        std::cout << v.value << '\n';
    }
    else {
        static_assert(cavi::dependent_false<T>, "unhandled type");
    }
}

template<template <typename> class HB>
void print_var(const var<HB>& vr) {
    cavi::visit([](const auto& v) {
        print_var_impl(v);
    }, vr);
}

}
```
And finally, that is our library complete (Note, the user is expected to provide an overload for ```print_var_impl()``` for any types they add to the hierarchy, otherwise, the compile time assert in ```print_var_impl()``` will be triggered).

### Using The Library
Now, from the perspective of a user of the library, it is more or less the same as if you were to creating your own hierarchy and the following the setup process (step 1-3). ```cavi::extend_hierarchy_t``` can be used to extend the hierarchy from the library to add new types — we will be adding ```var_bool```.

```var_HY.h```
```c++
#include "convar/convar_hierarchy.h"
namespace myns {
// forward declaration of base
template<typename>
class var_HYBext;

// the library could provide macro to make aliases for a given base so user wouldnt have to do it
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

```
As in step 2 of the setup process, we have to make ```var_bool``` inherit from ```var_HYBext```:

```var.h```
```c++
#include "var_HY.h"
#include "convar/convar_types.h"

namespace myns {
struct var_bool : var_arith, var_HYBext<var_bool> {
	bool value;
	var_bool(bool v) : value(v) {}
};
}
```

And, as with step 3 from setup, we have to instantiate via ```CAVI_INSTANTIATE```:

```var_HY.cpp```
```c++
#include "cavi/instantiate.h"
#include "var.h"

CAVI_INSTANTIATE(myns::var_HYext, myns::var_HYBext)
```
Now, to illustrate the usage of the library function ```convar::print_var``` with our own added implementation for ```var_bool```:

```main.cpp```
```c++
#include "var.h"
#include "convar/convar_lib.h"

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
```

Output:
```
150
3.14
Hello World
(10, 15)
true
```
Note, that new operations/functionality can be added via ```cavi::visit``` (and ofc ```cavi::cast``` and ```cavi::isa``` can also be used).

## Additional Notes
- Classes cannot be constructed via aggregate initialization — this is a side effect of a preventative measure in place to stop a source of mistakes
- A class may only be part of one hierarchy
- Library can be used in a multi-threaded environment safely as hierarchy base data is only written in constructors and after that it is only read

## To-do list
The following are currently on the to-do list:
- Cloning utility (deep copy from base pointers)
- Support for hierarchies with virtual bases
- Factory utility (constructing class types from a runtime identifier e.g. class name)
