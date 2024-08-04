#include <iostream>
#include <EASTL/string.h>

constexpr eastl::string_view LOCALE_FR_HELLO = "Bonjour %.*s! Comment allez-vous?\n";
constexpr eastl::string_view LOCALE_EN_HELLO = "Hello %.*s! How are you?\n";

constexpr const char* NAME_ELEANOR = "Eleanor Rigby";
constexpr eastl::string_view NAME_TOM = "Tom";

void SayHello(eastl::string_view localised, eastl::string_view fullName)
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