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