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