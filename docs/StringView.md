# A guide to using EASTL string_view
EASTL provides a string class called ``eastl::string_view`` that is similar to ``std::string_view`` in that it is ideal for storing string literals and for parameter passing of immutable strings. This is especially true when compared to alternatives such as ``eastl::string`` or simple ``const char*``. The following code examples will illustrate these advantages while also highlighting a few caveats.

## C String string literals
Consider the [following code](StringLiteral/CString/CString.cpp) that displays localised strings in English and French. The ``SayHello()`` function takes two parameters, one for the localised string, and a second that represents a full name to be formatted into the first string. The function will also use a string delimiter to extract a first name from the name parameter. 

With the exception of ``LOCALE_EN_HELLO``, all of these strings are defined using ``constexpr const char*`` to ensure that they are evaluated statically.

```c++
#include <cstring>
#include <iostream>

constexpr const char* LOCALE_FR_HELLO = "Bonjour %.*s! Comment allez-vous?\n";
const char* LOCALE_EN_HELLO = "Hello %.*s! How are you?\n";

constexpr const char* NAME_ELEANOR = "Eleanor Rigby";
constexpr const char* NAME_TOM = "Tom";

void SayHello(const char* localised, const char* fullName)
{
    size_t length = std::strlen(fullName);
    if (length <= 0)
    {
        return;
    }

    const char* delimiter = std::strchr(fullName, ' ');
    int firstNameLength = delimiter != nullptr ? delimiter - fullName : length;

    printf(localised, firstNameLength, fullName);
}

int main()
{
    SayHello(LOCALE_EN_HELLO, NAME_ELEANOR);
    SayHello(LOCALE_FR_HELLO, NAME_TOM);

    return 0;
}
```

Let's compile this code and [view the disassembly](https://godbolt.org/z/EGjef6zEr) to analyse the impact of using ``constexpr const char*`` for string literals. The target platform and compiler for this example is x86_64 gcc, with all optimisations enabled (O3).

``` Assembly
SayHello(char const*, char const*):
        push    r12
        mov     r12, rdi
        mov     rdi, rsi
        push    rbp
        push    rbx
        mov     rbx, rsi
        call    strlen
        test    rax, rax
        je      .L1
        mov     rdi, rbx
        mov     esi, 32
        mov     rbp, rax
        call    strchr
        mov     rdx, rbx
        mov     rdi, r12
        mov     rsi, rax
        sub     rsi, rbx
        test    rax, rax
        pop     rbx
        cmove   esi, ebp
        pop     rbp
        xor     eax, eax
        pop     r12
        jmp     printf
.L1:
        pop     rbx
        pop     rbp
        pop     r12
        ret
.LC0:
        .string "Eleanor Rigby"
.LC1:
        .string "Tom"
.LC2:
        .string "Bonjour %.*s! Comment allez-vous?\n"
main:
        sub     rsp, 8
        mov     rdi, QWORD PTR LOCALE_EN_HELLO[rip]
        mov     esi, OFFSET FLAT:.LC0
        call    SayHello(char const*, char const*)
        mov     esi, OFFSET FLAT:.LC1
        mov     edi, OFFSET FLAT:.LC2
        call    SayHello(char const*, char const*)
        xor     eax, eax
        add     rsp, 8
        ret
.LC3:
        .string "Hello %.*s! How are you?\n"
LOCALE_EN_HELLO:
        .quad   .LC3
```

A few observations

- The ‘constexpr’ strings are stored in the read-only memory section at LC0-2 and referenced directly. The non constexpr string is referenced via a pointer instead. This is the advantage of using constexpr for string literals - they are evaluated statically and referenced directly. 
- The ``SayHello()`` function relies on ``std::strlen()`` to determine if the string is empty. This operation is potentially dangerous for strings that are not properly null-terminated. 
       
## EASTL::string_view string literals
The [following example](StringLiteral/StringView/StringView.cpp) replaces ``const char*`` with ``eastl::string_view``. 

```C++
#include <iostream>
#include <EASTL/string.h>

constexpr eastl::string_view LOCALE_FR_HELLO = "Bonjour %.*s! Comment allez-vous?\n";
constexpr eastl::string_view LOCALE_EN_HELLO = "Hello %.*s! How are you?\n";

constexpr const char* NAME_ELEANOR = "Eleanor Rigby";
constexpr eastl::string_view NAME_TOM = "Tom";

void SayHello(const eastl::string_view localised, const eastl::string_view fullName)
{
    size_t length = fullName.length();
    if (length <= 0)
    {
        return;
    }

    size_t delimiterPosition = fullName.find(' ');
    int firstNameLength = delimiterPosition != eastl::string_view::npos ? delimiterPosition : length;

    printf(localised.data(), firstNameLength, fullName);
}

int main()
{
    SayHello(LOCALE_EN_HELLO, NAME_ELEANOR);
    SayHello(LOCALE_FR_HELLO, NAME_TOM);

    return 0;
}
```

[Compiling it](https://godbolt.org/z/TWhKhh6G1) with the same parameters as before gives us the following output.

```Assembly
SayHello(eastl::basic_string_view<char>, eastl::basic_string_view<char>):
        test    rcx, rcx
        je      .L1
        lea     r8, [rdx+rcx]
        mov     rax, rdx
        cmp     rdx, r8
        jne     .L3
        jmp     .L7
.L5:
        add     rax, 1
        cmp     r8, rax
        je      .L7
.L3:
        cmp     BYTE PTR [rax], 32
        jne     .L5
        mov     rsi, rcx
        cmp     r8, rax
        je      .L4
        sub     rax, rdx
        mov     rsi, rax
        cmp     rax, -1
        cmove   rsi, rcx
.L4:
        xor     eax, eax
        jmp     printf
.L1:
        ret
.L7:
        mov     rsi, rcx
        jmp     .L4
.LC0:
        .string "Eleanor Rigby"
main:
        mov     esi, OFFSET FLAT:.LC0
        sub     rsp, 8
        mov     rax, rsi
.L12:
        add     rax, 1
        cmp     BYTE PTR [rax], 0
        jne     .L12
        sub     rax, OFFSET FLAT:.LC0
        mov     rdx, rsi
        mov     rdi, QWORD PTR LOCALE_EN_HELLO[rip]
        mov     rsi, QWORD PTR LOCALE_EN_HELLO[rip+8]
        mov     rcx, rax
        call    SayHello(eastl::basic_string_view<char>, eastl::basic_string_view<char>)
        mov     rdx, QWORD PTR NAME_TOM[rip]
        mov     rcx, QWORD PTR NAME_TOM[rip+8]
        mov     rdi, QWORD PTR LOCALE_FR_HELLO[rip]
        mov     rsi, QWORD PTR LOCALE_FR_HELLO[rip+8]
        call    SayHello(eastl::basic_string_view<char>, eastl::basic_string_view<char>)
        xor     eax, eax
        add     rsp, 8
        ret
.LC1:
        .string "Tom"
NAME_TOM:
        .quad   .LC1
        .quad   3
.LC2:
        .string "Hello %.*s! How are you?\n"
LOCALE_EN_HELLO:
        .quad   .LC2
        .quad   25
.LC3:
        .string "Bonjour %.*s! Comment allez-vous?\n"
LOCALE_FR_HELLO:
        .quad   .LC3
        .quad   34
```

Observations
- The string literals that were defined with ``constexpr string_view`` have been stored in the binary along with a new property that represents their length. This is because ``eastl::string_view`` caches the length of the underlying string on construction.
    ```Assembly
    LOCALE_FR_HELLO:
        .quad   .LC3
        .quad   34
    ```
    Constructor defined in [string_view.h](external/EASTL/include/EASTL/string_view.h).
    ```C++
    EA_CONSTEXPR basic_string_view(const T* s) : mpBegin(s), mnCount(s != nullptr ? CharStrlen(s) : 0) {}
    ```
- Operations such as ``length()`` or ``empty()`` are more performant as the length is cached.
- Note the use of ``eastl::string_view`` as parameters on the ``SayHello()`` function. This class is ideal for passing immutable strings as it stores a const pointer (``mpBegin`` as shown in the constructor above) to the original string without allocating any new memory for a copy. Not requiring a memory allocation also allows the string to be ``constexpr`` so that it can be evaluated statically.
- The ``NAME_ELEANOR`` string literal did not need to be explicitly converted when passed to the ``SayHello()`` function.
- Also worth noting that it is sufficient to pass ``eastl::string_view`` by value rather than by reference. As the class simply points to the original string, the extra level of indirection with a reference is unnecessary.

## eastl::string string literals
Let's now [replace ``eastl::string_view``](StringLiteral/EASTLString/EASTLString.cpp) with ``eastl::string`` to see the impact it has on this code.

```C++
#include <iostream>
#include <EASTL/string.h>

void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return new uint8_t[size];
}

const eastl::string LOCALE_FR_HELLO = "Bonjour %.*s! Comment allez-vous?\n";
const eastl::string LOCALE_EN_HELLO = "Hello %.*s! How are you?\n";

const char* NAME_ELEANOR = "Eleanor Rigby";
const eastl::string  NAME_TOM = "Tom";

void SayHello(const eastl::string& localised, const eastl::string& fullName)
{
    size_t length = fullName.length();
    if (length <= 0)
    {
        return;
    }

    size_t delimiterPosition = fullName.find(' ');
    int firstNameLength = delimiterPosition != eastl::string_view::npos ? delimiterPosition : length;

    printf(localised.data(), firstNameLength, fullName);
}

int main()
{
    SayHello(LOCALE_EN_HELLO, NAME_ELEANOR);
    SayHello(LOCALE_FR_HELLO, NAME_TOM);

    return 0;
}
```