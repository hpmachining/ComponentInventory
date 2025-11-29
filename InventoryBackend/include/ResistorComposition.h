#pragma once
#include <string>

struct ResistorComposition {
    int id;
    std::string name;

    ResistorComposition() : id(0) {}
    ResistorComposition(const std::string& n) : id(0), name(n) {}
};
