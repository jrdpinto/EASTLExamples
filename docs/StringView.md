# A guide to using EASTL string_view
EASTL provides a string class called ``eastl::string_view`` that is similar to ``std::string_view`` in that it is a 'view' into a string object. This 'view' takes the form of a pointer to the original string along with a cached string length. When combined, the string pointer and the cached length can be used to refer to either a substring or the entirety of the original string. As such, ``eastl::string_view`` is the ideal string type for creating substrings and parameter passing of immutable strings. It can also be used to store string literals. These advantages are especially evident when compared to alternatives such as ``eastl::string`` or a simple ``const char*``. The following code examples will illustrate these advantages while also highlighting a few caveats.

## C String string literals
Consider the [following code](https://github.com/jrdpinto/EASTLExamples/blob/master/StringLiteral/CString/CString.cpp) that formats a full name to be used within a pre-defined prank call message. The ``PrankMoe()`` function takes two parameters, one for the prank message, and a second that represents a full name. Using a space as a delimiter, the function extracts the first name from the string.

With the exception of ``MOE_DIALOGUE_2``, all of these strings are defined using ``constexpr const char*`` to ensure that they are evaluated statically.

```c++
#include <cstring>
#include <iostream>

constexpr const char* MOE_DIALOGUE_1 = "Hey, is there a %.*s here? Hey, everybody, I wanna %.*s!\n";
const char* MOE_DIALOGUE_2 = "Uh, %.*s? Hey, I'm lookin for %.*s!\n";

constexpr const char* PRANK_NAME_1 = "Seymour Butz";
constexpr const char* PRANK_NAME_2 = "Amanda Hugginkiss";

void PrankMoe(const char* localised, const char* fullName)
{
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

Let's compile this code and [view the disassembly](https://godbolt.org/z/67sMMs6KM) to analyse the impact of using ``constexpr const char*`` for string literals. The target platform and compiler for this example is x86_64 gcc, with all optimisations enabled (O3).

<details>
<summary>Disassembly</summary>

``` assembly
PrankMoe(char const*, char const*):
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
.LC0:
        .string "Seymour Butz"
.LC1:
        .string "Hey, is there a %.*s here? Hey, everybody, I wanna %.*s!\n"
.LC2:
        .string "Amanda Hugginkiss"
main:
        sub     rsp, 8
        mov     esi, OFFSET FLAT:.LC0
        mov     edi, OFFSET FLAT:.LC1
        call    PrankMoe(char const*, char const*)
        mov     rdi, QWORD PTR MOE_DIALOGUE_2[rip]
        mov     esi, OFFSET FLAT:.LC2
        call    PrankMoe(char const*, char const*)
        xor     eax, eax
        add     rsp, 8
        ret
.LC3:
        .string "Uh, %.*s? Hey, I'm lookin for %.*s!\n"
MOE_DIALOGUE_2:
        .quad   .LC3
```
</details>

This is a very simple program so the disassembly is fairly straightforward. All of the strings here are stored in the binary and referenced to avoid copying. It is worth nothing that the ‘constexpr’ strings are stored in the read-only memory section at LC0/1/2 and referenced directly. The non constexpr string ``MOE_DIALOGUE_2`` however, is referenced via a pointer. This is the advantage of using constexpr for string literals - they are evaluated statically and referenced directly. 

## EASTL String literals
Let's now [replace ``constexpr const char*``](https://github.com/jrdpinto/EASTLExamples/blob/master/StringLiteral/EASTLString/EASTLString.cpp) with ``eastl::string`` to see the impact it has on this code.

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

Note that the code now requires a custom allocator in order to use ``eastl::string`` as the class usually (but not always) allocates strings on the heap. To keep things simple, the custom allocator forwards to global new[]. Let's now examine the [resulting disassembly.](https://godbolt.org/z/bdo95z33G)

<details>
<summary>Disassembly</summary>

```assembly
operator new[](unsigned long, char const*, int, unsigned int, char const*, int):
        jmp     operator new[](unsigned long)
PrankMoe(eastl::basic_string<char, eastl::allocator> const&, eastl::basic_string<char, eastl::allocator> const&):
        push    r14
        push    r13
        push    r12
        mov     r12, rsi
        push    rbp
        mov     rbp, rdi
        push    rbx
        sub     rsp, 32
        movsx   rsi, BYTE PTR [rsi+23]
        test    sil, sil
        js      .L64
        movsx   rax, sil
        mov     edx, 23
        mov     r13, r12
        sub     rdx, rax
        lea     rcx, [r12+rdx]
        je      .L65
.L15:
        mov     rax, r13
        jmp     .L18
.L20:
        add     rax, 1
        cmp     rax, rcx
        je      .L19
.L18:
        cmp     BYTE PTR [rax], 32
        jne     .L20
        cmp     rax, rcx
        je      .L19
        sub     rax, r13
        cmp     rax, -1
        je      .L19
        cmp     rdx, rax
        pxor    xmm0, xmm0
        mov     BYTE PTR [rsp+23], 23
        cmovbe  rax, rdx
        movaps  XMMWORD PTR [rsp], xmm0
        mov     QWORD PTR [rsp+15], 0
        mov     rbx, rax
        cmp     rax, 23
        ja      .L66
        movaps  XMMWORD PTR [rsp], xmm0
        mov     QWORD PTR [rsp+15], 0
.L23:
        mov     rdi, rsp
.L22:
        mov     rdx, rbx
        mov     rsi, r13
        call    memmove
        cmp     BYTE PTR [rsp+23], 0
        js      .L67
        mov     eax, 23
        sub     eax, ebx
        mov     BYTE PTR [rsp+23], al
        test    al, al
        js      .L68
        movzx   eax, al
        mov     edx, 23
        sub     rdx, rax
        add     rdx, rsp
.L27:
        mov     BYTE PTR [rdx], 0
.L28:
        movsx   rax, BYTE PTR [r12+23]
        test    al, al
        js      .L69
        mov     ecx, 23
        mov     r8, r12
        sub     rcx, rax
.L40:
        movsx   rax, BYTE PTR [rsp+23]
        test    al, al
        js      .L41
        mov     esi, 23
        mov     rdx, rsp
        sub     rsi, rax
.L42:
        cmp     BYTE PTR [rbp+23], 0
        mov     rdi, rbp
        jns     .L44
        mov     rdi, QWORD PTR [rbp+0]
.L44:
        xor     eax, eax
        call    printf
        cmp     BYTE PTR [rsp+23], 0
        js      .L70
.L9:
        add     rsp, 32
        pop     rbx
        pop     rbp
        pop     r12
        pop     r13
        pop     r14
        ret
.L66:
        lea     rdi, [rax+1]
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
.L41:
        mov     rdx, QWORD PTR [rsp]
        mov     rsi, QWORD PTR [rsp+8]
        jmp     .L42
.L69:
        mov     r8, QWORD PTR [r12]
        mov     rcx, QWORD PTR [r12+8]
        jmp     .L40
.L64:
        mov     rax, QWORD PTR [r12+8]
        mov     rdx, rax
        test    rax, rax
        je      .L62
        mov     r13, QWORD PTR [r12]
        lea     rcx, [r13+0+rax]
        cmp     r13, rcx
        jne     .L15
.L62:
        pxor    xmm0, xmm0
        mov     BYTE PTR [rsp+23], 23
        movaps  XMMWORD PTR [rsp], xmm0
        mov     QWORD PTR [rsp+15], 0
.L48:
        mov     r14, QWORD PTR [r12]
        lea     rbx, [r14+rax]
        sub     rbx, r14
        mov     r13, rbx
        cmp     rbx, 23
        jbe     .L32
.L74:
        lea     rdi, [rbx+1]
        call    operator new[](unsigned long)
        mov     QWORD PTR [rsp+8], 0
        mov     QWORD PTR [rsp], rax
        mov     rdi, rax
        mov     rax, rbx
        bts     rax, 63
        mov     QWORD PTR [rsp+16], rax
.L33:
        cmp     BYTE PTR [rsp+23], 0
        js      .L34
        mov     rdi, rsp
.L34:
        mov     rdx, rbx
        mov     rsi, r14
        call    memmove
        cmp     BYTE PTR [rsp+23], 0
        js      .L71
        mov     eax, 23
        sub     eax, ebx
        mov     BYTE PTR [rsp+23], al
        test    al, al
        js      .L72
        movzx   eax, al
        mov     edx, 23
        sub     rdx, rax
        lea     rax, [rsp+rdx]
.L38:
        mov     BYTE PTR [rax], 0
        jmp     .L28
.L70:
        mov     rdi, QWORD PTR [rsp]
        test    rdi, rdi
        je      .L9
        call    operator delete[](void*)
        add     rsp, 32
        pop     rbx
        pop     rbp
        pop     r12
        pop     r13
        pop     r14
        ret
.L67:
        mov     QWORD PTR [rsp+8], rbx
.L25:
        add     rbx, QWORD PTR [rsp]
        mov     rdx, rbx
        jmp     .L27
.L19:
        pxor    xmm0, xmm0
        mov     edx, 23
        mov     BYTE PTR [rsp+23], 23
        movaps  XMMWORD PTR [rsp], xmm0
        sub     rdx, rsi
        mov     QWORD PTR [rsp+15], 0
        test    sil, sil
        js      .L73
.L29:
        mov     r14, r12
        lea     rbx, [r12+rdx]
        sub     rbx, r14
        mov     r13, rbx
        cmp     rbx, 23
        ja      .L74
.L32:
        pxor    xmm0, xmm0
        xor     edi, edi
        movaps  XMMWORD PTR [rsp], xmm0
        mov     QWORD PTR [rsp+15], 0
        jmp     .L33
.L71:
        mov     QWORD PTR [rsp+8], rbx
.L36:
        mov     rax, QWORD PTR [rsp]
        add     rax, r13
        jmp     .L38
.L73:
        mov     rax, QWORD PTR [r12+8]
        jmp     .L48
.L65:
        pxor    xmm0, xmm0
        mov     BYTE PTR [rsp+23], 23
        movaps  XMMWORD PTR [rsp], xmm0
        mov     QWORD PTR [rsp+15], 0
        jmp     .L29
.L72:
        mov     r13, QWORD PTR [rsp+8]
        jmp     .L36
.L68:
        mov     rbx, QWORD PTR [rsp+8]
        jmp     .L25
        mov     rbx, rax
        jmp     .L46
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
        je      .L85
        mov     rax, rsi
.L77:
        add     rax, 1
        cmp     BYTE PTR [rax], 0
        jne     .L77
        sub     rax, r13
        mov     rbx, rax
        mov     r12, rax
        cmp     rax, 23
        jbe     .L76
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
        js      .L79
        mov     eax, 23
        sub     eax, ebx
        mov     BYTE PTR [rbp+23], al
        test    al, al
        jns     .L83
        mov     r12, QWORD PTR [rbp+8]
        jmp     .L82
.L85:
        xor     r12d, r12d
        xor     ebx, ebx
.L76:
        pxor    xmm0, xmm0
        mov     rdx, r12
        mov     rsi, r13
        mov     rdi, rbp
        movups  XMMWORD PTR [rbp+0], xmm0
        mov     QWORD PTR [rbp+15], 0
        call    memmove
        cmp     BYTE PTR [rbp+23], 0
        js      .L79
        mov     eax, 23
        sub     eax, ebx
        mov     BYTE PTR [rbp+23], al
.L83:
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
.L79:
        mov     QWORD PTR [rbp+8], r12
.L82:
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
        js      .L103
.L91:
        mov     esi, OFFSET FLAT:PRANK_NAME_2
        mov     edi, OFFSET FLAT:MOE_DIALOGUE_2
        call    PrankMoe(eastl::basic_string<char, eastl::allocator> const&, eastl::basic_string<char, eastl::allocator> const&)
        add     rsp, 48
        xor     eax, eax
        pop     rbx
        ret
.L103:
        mov     rdi, QWORD PTR [rsp+16]
        test    rdi, rdi
        je      .L91
        call    operator delete[](void*)
        jmp     .L91
        mov     rbx, rax
        jmp     .L92
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
```
</details>

Yikes! This code is considerably more complex. Let's unpack this mess.

- Unlike the previous example, string literals in this code are no longer ``constexpr``. As ``eastl::string`` can sometimes involve dynamically allocated memory, it cannot be evaluated statically.
- The disassembly now includes a function called ``_GLOBAL__sub_I_operator new[]`` that is responsible for constructing and destroying (on ``_cta_atexit``) any global ``eastl::string`` objects when the program executes and concludes. Again, this is contrary to the previous example where string literals involved no runtime overhead.
- There are two versions of the ``PrankMoe()`` function - ``PrankMoe()`` and ``PrankMoe() [clone .cold]``. This is an optimisation that the compiler performs on a function to split it into two functions - a function that accounts for edge cases and error handling (eg: memory allocation failures, invalid or special string lengths), and a performant version that focuses on the more common execution path.
- In the main function where the ``PRANK_NAME_1`` string literal is passed to the ``PrankMoe()`` function, a new ``eastl::string`` object is constructed. This is another disadvantage to using ``eastl::string`` for parameter passing of immutable strings. If the input string is of a different type, a new object containing a copy of the original string must be constructed.
- Additionally, the call to ``substr()`` within ``PrankMoe()`` returns a brand new copy of the original string. For a large enough string this step will also result in another heap allocation.
- Overall, this code is bad, and it should feel bad.
- It should be noted however that the constructor does include an optimisation for small strings. [Small String Optimisation (SSO)](https://youtu.be/CIB_khrNPSU?si=peN-tP4md_6YjRAr) involves storing strings that are small enough within the local storage of the string class, thereby avoiding a heap allocation. In this case, the threshold for a small string is 23 characters as evidenced by the comparisons between string length and the value '23'.

  ``` Assembly
  mov     rax, rsi
  // Iterates over the string until a null terminator is found in order to determine its length
  .L77:
  add     rax, 1
  cmp     BYTE PTR [rax], 0
  jne     .L77
  sub     rax, r13
  mov     rbx, rax
  mov     r12, rax
  // If the string is 23 characters or less, jump to .L76
  cmp     rax, 23
  jbe     .L76
  ......
  // Uses 'memmove' to copy the string to the SSO buffer
  .L76:
  pxor    xmm0, xmm0
  mov     rdx, r12
  mov     rsi, r13
  mov     rdi, rbp
  movups  XMMWORD PTR [rbp+0], xmm0
  mov     QWORD PTR [rbp+15], 0
  call    memmove
  ```

## EASTL::string_view string literals
The [following example](https://github.com/jrdpinto/EASTLExamples/blob/master/StringLiteral/StringView/StringView.cpp) replaces ``const char*`` with ``eastl::string_view``. 

```C++
#include <iostream>
#include <EASTL/string.h>

constexpr eastl::string_view MOE_DIALOGUE_1 = "Hey, is there a %.*s here? Hey, everybody, I wanna %.*s!\n";
constexpr eastl::string_view MOE_DIALOGUE_2 = "Uh, %.*s? Hey, I'm lookin for %.*s!\n";

constexpr const char* PRANK_NAME_1 = "Seymour Butz";
constexpr eastl::string_view PRANK_NAME_2 = "Amanda Hugginkiss";

void PrankMoe(eastl::string_view localised, eastl::string_view fullName)
{
    size_t delimiterPosition = fullName.find(' ');
    eastl::string_view outputName = delimiterPosition != eastl::string_view::npos ?
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

[Compiling it](https://godbolt.org/z/xPqdzWafM) with the same parameters as before gives us the following output.

<details>
<summary>Disassembly</summary>

```Assembly
PrankMoe(eastl::basic_string_view<char>, eastl::basic_string_view<char>):
        lea     rsi, [rdx+rcx]
        test    rcx, rcx
        je      .L5
        mov     rax, rdx
        cmp     rdx, rsi
        jne     .L3
        jmp     .L7
.L4:
        add     rax, 1
        cmp     rsi, rax
        je      .L7
.L3:
        cmp     BYTE PTR [rax], 32
        jne     .L4
        mov     r9, rcx
        cmp     rax, rsi
        je      .L2
        sub     rax, rdx
        cmp     rcx, rax
        mov     r9, rax
        cmovbe  r9, rcx
        cmp     rax, -1
        cmove   r9, rcx
.L2:
        mov     r8, rdx
        mov     rsi, r9
        xor     eax, eax
        jmp     printf
.L7:
        mov     r9, rcx
        mov     r8, rdx
        xor     eax, eax
        mov     rsi, r9
        jmp     printf
.L5:
        xor     r9d, r9d
        mov     r8, rdx
        xor     eax, eax
        mov     rsi, r9
        jmp     printf
.LC0:
        .string "Seymour Butz"
main:
        mov     esi, OFFSET FLAT:.LC0
        sub     rsp, 8
        mov     rax, rsi
.L13:
        add     rax, 1
        cmp     BYTE PTR [rax], 0
        jne     .L13
        sub     rax, OFFSET FLAT:.LC0
        mov     rdx, rsi
        mov     rdi, QWORD PTR MOE_DIALOGUE_1[rip]
        mov     rsi, QWORD PTR MOE_DIALOGUE_1[rip+8]
        mov     rcx, rax
        call    PrankMoe(eastl::basic_string_view<char>, eastl::basic_string_view<char>)
        mov     rdx, QWORD PTR PRANK_NAME_2[rip]
        mov     rcx, QWORD PTR PRANK_NAME_2[rip+8]
        mov     rdi, QWORD PTR MOE_DIALOGUE_2[rip]
        mov     rsi, QWORD PTR MOE_DIALOGUE_2[rip+8]
        call    PrankMoe(eastl::basic_string_view<char>, eastl::basic_string_view<char>)
        xor     eax, eax
        add     rsp, 8
        ret
.LC1:
        .string "Amanda Hugginkiss"
PRANK_NAME_2:
        .quad   .LC1
        .quad   17
.LC2:
        .string "Uh, %.*s? Hey, I'm lookin for %.*s!\n"
MOE_DIALOGUE_2:
        .quad   .LC2
        .quad   36
.LC3:
        .string "Hey, is there a %.*s here? Hey, everybody, I wanna %.*s!\n"
MOE_DIALOGUE_1:
        .quad   .LC3
        .quad   57
```

</details>

Observations
- The string literals that were defined with ``constexpr string_view`` have been stored in the binary along with a new property that represents their length. This is because ``eastl::string_view`` caches the length of the underlying string on construction.
    ```Assembly
    .LC3:
        .string "Hey, is there a %.*s here? Hey, everybody, I wanna %.*s!\n"
    MOE_DIALOGUE_1:
        .quad   .LC3
        .quad   57
    ```
    Constructor defined in [string_view.h](https://github.com/electronicarts/EASTL/blob/05f4b4aef33f2f3ded08f19fa97f5a27ff35ff9f/include/EASTL/string_view.h).
    ```C++
    EA_CONSTEXPR basic_string_view(const T* s) : mpBegin(s), mnCount(s != nullptr ? CharStrlen(s) : 0) {}
    ```
- Operations such as ``length()`` or ``empty()`` are more performant as the length is cached. The cached string size has the added advantage that the class does not need to rely on the presence of a null terminator ``\0`` to find the end of the string.
- Note the use of ``eastl::string_view`` as parameters on the ``PrankMoe()`` function. This class is ideal for passing immutable strings as it stores a const pointer (``mpBegin`` as shown in the constructor above) to the original string without allocating any new memory for a copy. Not requiring a memory allocation also allows the string to be ``constexpr`` so that it can be evaluated statically.
- The ``Seymour Butz`` string literal is implicitly converted to ``eastl::string_view`` when passed to the ``PrankMoe()`` function. Unlike the previous example however, there is no copying involved, and no potential for an allocation on the heap.
- Also worth noting that it is sufficient to pass ``eastl::string_view`` by value rather than by reference. As the class simply points to the original string, the extra level of indirection with a reference is unnecessary.
- Overall, the binary is only slightly bigger than the version that used C strings and is slightly more performant due to the cached string size. It is considerably better than the example that used ``eastl::string`` as it does not involve any heap allocations.

## Caveats
For all of its benefits, there are a few caveats to be aware of when utilising ``eastl::string_view``.

- As stated before, the class stores a ``const`` pointer to the original string. Should the original string fall out of scope or be deallocated, the pointer will become invalid. For this use case, be sure to use ``eastl::string`` to save a copy of the original string.
- The ``const`` nature of the underlying string also means that ``eastl::string_view`` is not intended for dynamic construction of strings.
- While ``eastl::string_view`` is ideal for parameter passing of immutable strings, if the string needs to be modified in any way ``eastl::string&`` is a better choice.
- When using the ``substr()`` function, bear in mind that the class still stores a pointer to the original string in its entirety. This means that the ``data()`` function which returns a C string, will return the full string (from the pointer onwards) and not the substring.
  ``` C++
  eastl::string_view fullName = "Amanda Hugginkiss";
  eastl::string_view firstName = fullName.substr(0,6);

  printf("%.*s\n", firstName.length(), firstName.data()); // Prints 'Amanda'
  std::cout << firstName.data() << std::endl;             // Prints 'Amanda Hugginkiss'
  ```
