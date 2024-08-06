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