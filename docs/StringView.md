# A guide to using EASTL string_view
EASTL provides a string class called ``eastl::string_view`` that is similar to ``std::string_view`` in that it is a 'view' into a string object. This 'view' takes the form of a pointer to the original string along with a cached string length. When combined, the string pointer and the cached length can be used to refer to either a substring or the entirety of the original string. As such, ``eastl::string_view`` is ideal for creating substrings and parameter passing of immutable strings. It can also be used to store string literals. This is especially true when compared to alternatives such as ``eastl::string`` or simple ``const char*``. The following code examples will illustrate these advantages while also highlighting a few caveats.

## C String string literals
Consider the [following code](StringLiteral/CString/CString.cpp) that formats a full name to be used within a pre-defined prank call message. The ``PrankMoe()`` function takes two parameters, one for the prank message, and a second that represents a full name. Using a space as a delimiter, the function extracts the first name from the string. The function will also exit early if either input string is null, or empty.

With the exception of ``MOE_DIALOGUE_2``, all of these strings are defined using ``constexpr const char*`` to ensure that they are evaluated statically.

```c++
#include <cstring>
#include <iostream>

constexpr const char* MOE_DIALOGUE_1 = "Hey, is there a %.*s here? Hey, everybody, I wanna %.*s!\n";
const char* MOE_DIALOGUE_2 = "Uh, %.*s? Hey, I'm lookin for %.*s!\n";

constexpr const char* PRANK_NAME_1 = "Seymour Butz";
constexpr const char* PRANK_NAME_2 = "Amanda Hugginkiss";

inline bool IsEmpty(const char* string)
{
    return !string || string[0] == '\0';
}

void PrankMoe(const char* localised, const char* fullName)
{
    if (IsEmpty(localised) || IsEmpty(fullName))
    {
        return;
    }

    const char* delimiter = std::strchr(fullName, ' ');
    int firstNameLength = delimiter != nullptr ? delimiter - fullName : strlen(fullName);

    printf(localised, firstNameLength, fullName, strlen(fullName), fullName);
}

int main()
{
    PrankMoe(MOE_DIALOGUE_1, PRANK_NAME_1);
    PrankMoe(MOE_DIALOGUE_2, PRANK_NAME_2);

    return 0;
}
```

Let's compile this code and [view the disassembly](https://godbolt.org/z/6jsxrdhz6) to analyse the impact of using ``constexpr const char*`` for string literals. The target platform and compiler for this example is x86_64 gcc, with all optimisations enabled (O3).

<details>
<summary>Disassembly</summary>

``` assembly
PrankMoe(char const*, char const*) [clone .part.0]:
        push    r12
        mov     r12, rdi
        push    rbp
        push    rbx
        mov     rbx, rsi
        mov     esi, 32
        mov     rdi, rbx
        call    strchr
        mov     rdi, rbx
        mov     rbp, rax
        call    strlen
        mov     rsi, rbp
        mov     r8, rbx
        mov     rdx, rbx
        sub     rsi, rbx
        test    rbp, rbp
        pop     rbx
        mov     rcx, rax
        cmove   esi, eax
        mov     rdi, r12
        pop     rbp
        xor     eax, eax
        pop     r12
        jmp     printf
PrankMoe(char const*, char const*):
        test    rdi, rdi
        je      .L14
        push    r12
        push    rbp
        push    rbx
        cmp     BYTE PTR [rdi], 0
        mov     rbx, rdi
        je      .L6
        mov     rbp, rsi
        test    rsi, rsi
        je      .L6
        cmp     BYTE PTR [rsi], 0
        jne     .L17
.L6:
        pop     rbx
        pop     rbp
        pop     r12
        ret
.L14:
        ret
.L17:
        mov     esi, 32
        mov     rdi, rbp
        call    strchr
        mov     rdi, rbp
        mov     r12, rax
        call    strlen
        mov     rsi, r12
        mov     r8, rbp
        mov     rdx, rbp
        sub     rsi, rbp
        test    r12, r12
        mov     rcx, rax
        mov     rdi, rbx
        cmove   esi, eax
        pop     rbx
        xor     eax, eax
        pop     rbp
        pop     r12
        jmp     printf
.LC0:
        .string "Seymour Butz"
.LC1:
        .string "Hey, is there a %.*s here? Hey, everybody, I wanna %.*s!\n"
.LC2:
        .string "Amanda Hugginkiss"
main:
        mov     edi, OFFSET FLAT:.LC1
        sub     rsp, 8
        mov     esi, OFFSET FLAT:.LC0
        call    PrankMoe(char const*, char const*) [clone .part.0]
        mov     rdi, QWORD PTR MOE_DIALOGUE_2[rip]
        test    rdi, rdi
        je      .L19
        cmp     BYTE PTR [rdi], 0
        jne     .L24
.L19:
        xor     eax, eax
        add     rsp, 8
        ret
.L24:
        mov     esi, OFFSET FLAT:.LC2
        call    PrankMoe(char const*, char const*) [clone .part.0]
        jmp     .L19
.LC3:
        .string "Uh, %.*s? Hey, I'm lookin for %.*s!\n"
MOE_DIALOGUE_2:
        .quad   .LC3
```

</details>

A few observations:

- The ‘constexpr’ strings are stored in the read-only memory section at LC0-2 and referenced directly. The non constexpr string is referenced via a pointer instead. This is the advantage of using constexpr for string literals - they are evaluated statically and referenced directly. 
- There are two versions of the 'PrankMoe()' function in the binary - the original which checks the input strings to ensure that they are not null/empty, and an optimised version `PrankMoe(char const*, char const*) [clone .part.0]:` that excludes the null/empty check. The optimised version appears to be used when it is certain that the input strings are not null or empty.

## EASTL String literals
Let's now [replace ``constexpr const char*``](StringLiteral/EASTLString/EASTLString.cpp) with ``eastl::string`` to see the impact it has on this code.

```C++
#include <iostream>
#include <EASTL/string.h>

void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return new uint8_t[size];
}

const eastl::string MOE_DIALOGUE_1 = "Hey, is there a %.*s here? Hey, everybody, I wanna %.*s!\n";
const eastl::string MOE_DIALOGUE_2 = "Uh, %.*s? Hey, I'm lookin for %.*s!\n";

const char* PRANK_NAME_1 = "Seymour Butz";
const eastl::string  PRANK_NAME_2 = "Amanda Hugginkiss";

void PrankMoe(const eastl::string& localised, const eastl::string& fullName)
{
    if (localised.empty() || fullName.empty())
    {
        return;
    }

    size_t delimiterPosition = fullName.find(' ');
    eastl::string outputName = delimiterPosition != eastl::string::npos ?
        fullName.substr(0, delimiterPosition) : fullName;

    printf(localised.data(), outputName.length(), outputName.data(), fullName.length(), fullName.data());
}

int main()
{
    PrankMoe(MOE_DIALOGUE_1, PRANK_NAME_1);
    PrankMoe(MOE_DIALOGUE_2, PRANK_NAME_2);

    return 0;
}
```

Note that the code now requires a custom allocator in order to use ``eastl::string`` as the class usually (but not always) allocates strings on the heap. To keep things simple, the custom allocator forwards to global new[]. Let's now examine the [resulting disassembly.](https://godbolt.org/z/Ezjzbocce)

<details>
<summary>Disassembly</summary>
operator new[](unsigned long, char const*, int, unsigned int, char const*, int):
        jmp     operator new[](unsigned long)
PrankMoe(eastl::basic_string<char, eastl::allocator> const&, eastl::basic_string<char, eastl::allocator> const&):
        push    r14
        mov     edx, 23
        push    r13
        push    r12
        mov     r12, rsi
        push    rbp
        mov     rbp, rdi
        push    rbx
        sub     rsp, 32
        movsx   rax, BYTE PTR [rdi+23]
        sub     rdx, rax
        test    al, al
        jns     .L11
        mov     rdx, QWORD PTR [rdi+8]
.L11:
        test    rdx, rdx
        je      .L9
        movsx   rax, BYTE PTR [r12+23]
        test    al, al
        js      .L70
        mov     ecx, 23
        mov     r13, r12
        sub     rcx, rax
        lea     rdx, [r12+rcx]
        je      .L9
.L16:
        mov     rax, r13
        cmp     r13, rdx
        jne     .L17
        jmp     .L71
.L20:
        add     rax, 1
        cmp     rax, rdx
        je      .L19
.L17:
        cmp     BYTE PTR [rax], 32
        jne     .L20
        cmp     rax, rdx
        je      .L19
        sub     rax, r13
        cmp     rax, -1
        je      .L19
        cmp     rax, rcx
        mov     rbx, rcx
        pxor    xmm0, xmm0
        mov     BYTE PTR [rsp+23], 23
        cmovbe  rbx, rax
        movaps  XMMWORD PTR [rsp], xmm0
        mov     QWORD PTR [rsp+15], 0
        cmp     rbx, 23
        ja      .L72
        movaps  XMMWORD PTR [rsp], xmm0
        mov     QWORD PTR [rsp+15], 0
.L23:
        mov     rdi, rsp
.L22:
        mov     rdx, rbx
        mov     rsi, r13
        call    memmove
        cmp     BYTE PTR [rsp+23], 0
        js      .L73
        mov     eax, 23
        sub     eax, ebx
        mov     BYTE PTR [rsp+23], al
        test    al, al
        js      .L74
        movzx   eax, al
        mov     edx, 23
        sub     rdx, rax
        lea     rbx, [rsp+rdx]
.L27:
        mov     BYTE PTR [rbx], 0
.L28:
        movsx   rax, BYTE PTR [r12+23]
        test    al, al
        js      .L75
        mov     ecx, 23
        mov     r8, r12
        sub     rcx, rax
.L37:
        movsx   rax, BYTE PTR [rsp+23]
        test    al, al
        js      .L38
        mov     esi, 23
        mov     rdx, rsp
        sub     rsi, rax
.L39:
        cmp     BYTE PTR [rbp+23], 0
        mov     rdi, rbp
        jns     .L41
        mov     rdi, QWORD PTR [rbp+0]
.L41:
        xor     eax, eax
        call    printf
        cmp     BYTE PTR [rsp+23], 0
        jns     .L9
        mov     rdi, QWORD PTR [rsp]
        test    rdi, rdi
        je      .L9
        call    operator delete[](void*)
.L9:
        add     rsp, 32
        pop     rbx
        pop     rbp
        pop     r12
        pop     r13
        pop     r14
        ret
.L70:
        mov     rcx, QWORD PTR [r12+8]
        test    rcx, rcx
        je      .L9
        mov     r13, QWORD PTR [r12]
        lea     rdx, [r13+0+rcx]
        jmp     .L16
.L72:
        lea     rdi, [rbx+1]
        call    operator new[](unsigned long)
        mov     QWORD PTR [rsp+8], 0
        mov     QWORD PTR [rsp], rax
        mov     rdi, rax
        mov     rax, rbx
        bts     rax, 63
        mov     QWORD PTR [rsp+16], rax
        sar     rax, 56
        test    al, al
        js      .L22
        jmp     .L23
.L38:
        mov     rdx, QWORD PTR [rsp]
        mov     rsi, QWORD PTR [rsp+8]
        jmp     .L39
.L75:
        mov     r8, QWORD PTR [r12]
        mov     rcx, QWORD PTR [r12+8]
        jmp     .L37
.L19:
        mov     rbx, rdx
        pxor    xmm0, xmm0
        mov     BYTE PTR [rsp+23], 23
        sub     rbx, r13
        movaps  XMMWORD PTR [rsp], xmm0
        mov     QWORD PTR [rsp+15], 0
        mov     r14, rbx
        cmp     rbx, 23
        jbe     .L29
        lea     rdi, [rbx+1]
        call    operator new[](unsigned long)
        mov     QWORD PTR [rsp+8], 0
        mov     QWORD PTR [rsp], rax
        mov     rdi, rax
        mov     rax, rbx
        bts     rax, 63
        mov     QWORD PTR [rsp+16], rax
        sar     rax, 56
        test    al, al
        jns     .L31
.L30:
        mov     rdx, r14
        mov     rsi, r13
        call    memmove
        cmp     BYTE PTR [rsp+23], 0
        js      .L76
        mov     eax, 23
        sub     eax, ebx
        mov     BYTE PTR [rsp+23], al
        test    al, al
        js      .L77
        movzx   eax, al
        mov     edx, 23
        sub     rdx, rax
        lea     r14, [rsp+rdx]
.L35:
        mov     BYTE PTR [r14], 0
        jmp     .L28
.L73:
        mov     QWORD PTR [rsp+8], rbx
.L25:
        add     rbx, QWORD PTR [rsp]
        jmp     .L27
.L71:
        mov     BYTE PTR [rsp+23], 23
        xor     r14d, r14d
        xor     ebx, ebx
.L29:
        pxor    xmm0, xmm0
        movaps  XMMWORD PTR [rsp], xmm0
        mov     QWORD PTR [rsp+15], 0
.L31:
        mov     rdi, rsp
        jmp     .L30
.L76:
        mov     QWORD PTR [rsp+8], r14
.L33:
        add     r14, QWORD PTR [rsp]
        jmp     .L35
.L77:
        mov     r14, QWORD PTR [rsp+8]
        jmp     .L33
.L74:
        mov     rbx, QWORD PTR [rsp+8]
        jmp     .L25
        mov     rbx, rax
        jmp     .L43
PrankMoe(eastl::basic_string<char, eastl::allocator> const&, eastl::basic_string<char, eastl::allocator> const&) [clone .cold]:
eastl::basic_string<char, eastl::allocator>::basic_string(char const*, eastl::allocator const&) [base object constructor]:
        push    r13
        pxor    xmm0, xmm0
        mov     r13, rsi
        push    r12
        push    rbp
        mov     rbp, rdi
        push    rbx
        sub     rsp, 8
        movups  XMMWORD PTR [rdi], xmm0
        mov     BYTE PTR [rdi+23], 23
        mov     QWORD PTR [rdi+15], 0
        cmp     BYTE PTR [rsi], 0
        je      .L88
        mov     rax, rsi
.L80:
        add     rax, 1
        cmp     BYTE PTR [rax], 0
        jne     .L80
        sub     rax, r13
        mov     rbx, rax
        mov     r12, rax
        cmp     rax, 23
        jbe     .L79
        lea     rdi, [rax+1]
        call    operator new[](unsigned long)
        mov     QWORD PTR [rbp+8], 0
        mov     rdx, rbx
        mov     rsi, r13
        mov     QWORD PTR [rbp+0], rax
        mov     rdi, rax
        mov     rax, rbx
        bts     rax, 63
        mov     QWORD PTR [rbp+16], rax
        sar     rax, 56
        test    al, al
        cmovns  rdi, rbp
        call    memmove
        cmp     BYTE PTR [rbp+23], 0
        js      .L82
        mov     eax, 23
        sub     eax, ebx
        mov     BYTE PTR [rbp+23], al
        test    al, al
        jns     .L86
        mov     r12, QWORD PTR [rbp+8]
        jmp     .L85
.L88:
        xor     r12d, r12d
        xor     ebx, ebx
.L79:
        pxor    xmm0, xmm0
        mov     rdx, r12
        mov     rsi, r13
        mov     rdi, rbp
        movups  XMMWORD PTR [rbp+0], xmm0
        mov     QWORD PTR [rbp+15], 0
        call    memmove
        cmp     BYTE PTR [rbp+23], 0
        js      .L82
        mov     eax, 23
        sub     eax, ebx
        mov     BYTE PTR [rbp+23], al
.L86:
        movsx   rax, al
        sub     rbp, rax
        mov     BYTE PTR [rbp+23], 0
        add     rsp, 8
        lea     r12, [rbp+23]
        pop     rbx
        pop     rbp
        pop     r12
        pop     r13
        ret
.L82:
        mov     QWORD PTR [rbp+8], r12
.L85:
        add     r12, QWORD PTR [rbp+0]
        mov     BYTE PTR [r12], 0
        add     rsp, 8
        pop     rbx
        pop     rbp
        pop     r12
        pop     r13
        ret
main:
        push    rbx
        sub     rsp, 48
        mov     rsi, QWORD PTR PRANK_NAME_1[rip]
        lea     rbx, [rsp+16]
        lea     rdx, [rsp+15]
        mov     rdi, rbx
        call    eastl::basic_string<char, eastl::allocator>::basic_string(char const*, eastl::allocator const&) [complete object constructor]
        mov     rsi, rbx
        mov     edi, OFFSET FLAT:MOE_DIALOGUE_1
        call    PrankMoe(eastl::basic_string<char, eastl::allocator> const&, eastl::basic_string<char, eastl::allocator> const&)
        cmp     BYTE PTR [rsp+39], 0
        js      .L106
.L94:
        mov     esi, OFFSET FLAT:PRANK_NAME_2
        mov     edi, OFFSET FLAT:MOE_DIALOGUE_2
        call    PrankMoe(eastl::basic_string<char, eastl::allocator> const&, eastl::basic_string<char, eastl::allocator> const&)
        add     rsp, 48
        xor     eax, eax
        pop     rbx
        ret
.L106:
        mov     rdi, QWORD PTR [rsp+16]
        test    rdi, rdi
        je      .L94
        call    operator delete[](void*)
        jmp     .L94
        mov     rbx, rax
        jmp     .L95
main.cold:
.LC2:
        .string "Hey, is there a %.*s here? Hey, everybody, I wanna %.*s!\n"
.LC3:
        .string "Uh, %.*s? Hey, I'm lookin for %.*s!\n"
.LC4:
        .string "Amanda Hugginkiss"
_GLOBAL__sub_I_operator new[](unsigned long, char const*, int, unsigned int, char const*, int):
        push    rbx
        mov     esi, OFFSET FLAT:.LC2
        mov     edi, OFFSET FLAT:MOE_DIALOGUE_1
        sub     rsp, 16
        lea     rbx, [rsp+15]
        mov     rdx, rbx
        call    eastl::basic_string<char, eastl::allocator>::basic_string(char const*, eastl::allocator const&) [complete object constructor]
        mov     edx, OFFSET FLAT:__dso_handle
        mov     esi, OFFSET FLAT:MOE_DIALOGUE_1
        mov     edi, OFFSET FLAT:eastl::basic_string<char, eastl::allocator>::~basic_string() [complete object destructor]
        call    __cxa_atexit
        mov     rdx, rbx
        mov     esi, OFFSET FLAT:.LC3
        mov     edi, OFFSET FLAT:MOE_DIALOGUE_2
        call    eastl::basic_string<char, eastl::allocator>::basic_string(char const*, eastl::allocator const&) [complete object constructor]
        mov     edx, OFFSET FLAT:__dso_handle
        mov     esi, OFFSET FLAT:MOE_DIALOGUE_2
        mov     edi, OFFSET FLAT:eastl::basic_string<char, eastl::allocator>::~basic_string() [complete object destructor]
        call    __cxa_atexit
        mov     rdx, rbx
        mov     esi, OFFSET FLAT:.LC4
        mov     edi, OFFSET FLAT:PRANK_NAME_2
        call    eastl::basic_string<char, eastl::allocator>::basic_string(char const*, eastl::allocator const&) [complete object constructor]
        add     rsp, 16
        mov     edx, OFFSET FLAT:__dso_handle
        mov     esi, OFFSET FLAT:PRANK_NAME_2
        mov     edi, OFFSET FLAT:eastl::basic_string<char, eastl::allocator>::~basic_string() [complete object destructor]
        pop     rbx
        jmp     __cxa_atexit
.LC5:
        .string "Seymour Butz"
PRANK_NAME_1:
        .quad   .LC5
</details>

Yikes! This code is considerably more complex. Let's unpack this mess.

- Unlike the previous example, string literals in this code are no longer ``constexpr``. As ``eastl::string`` can sometimes involve dynamically allocated memory, it cannot be evaluated statically.
- The disassembly now includes a function called ``_GLOBAL__sub_I_operator new[]`` that is responsible for constructing and destroying (on ``_cta_atexit``) any global strings when the program executes and concludes. Again, this is contrary to the previous example where string literals involved no runtime overhead.
- 

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