#pragma once

#include <string>

enum class LookupError
{
    None = 0,
    EmptyName = 1,
    AlreadyExists = 2
};

struct LookupItem
{
    int id;
    std::string name;
};
