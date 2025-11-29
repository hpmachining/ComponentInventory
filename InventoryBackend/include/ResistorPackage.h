#pragma once
#include <string>

struct ResistorPackage {
    int id;
    std::string name;

    ResistorPackage() : id(0) {}
    ResistorPackage(const std::string& n) : id(0), name(n) {}
};
