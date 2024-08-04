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