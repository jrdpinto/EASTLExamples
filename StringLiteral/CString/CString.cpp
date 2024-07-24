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