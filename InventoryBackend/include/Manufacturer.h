#pragma once
#include <string>

struct Manufacturer {
    int id;                  // Primary key
    std::string name;        // e.g. "Vishay"
    std::string country;     // Optional
    std::string website;     // Optional
    std::string notes;       // Optional

    Manufacturer() : id(0) {}
    Manufacturer(const std::string& n,
        const std::string& c = "",
        const std::string& w = "",
        const std::string& note = "")
        : id(0), name(n), country(c), website(w), notes(note) {
    }
};
