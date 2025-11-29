#pragma once
#include <string>

struct Category {
    int id;                  // Primary key
    std::string name;        // e.g. "Resistor"
    std::string description; // Optional longer description

    Category() : id(0) {}
    Category(const std::string& n, const std::string& d = "")
        : id(0), name(n), description(d) {
    }
};
