#pragma once
#include <string>
#include <cctype>
#include <ctime>
#include <sqlite3.h>

// Utility to generate ISO timestamp
inline std::string currentTimestamp() {
    time_t now = time(nullptr);
    struct tm tmStruct;

#ifdef _WIN32
    // Windows safe version
    localtime_s(&tmStruct, &now);
#else
    // POSIX safe version
    localtime_r(&now, &tmStruct);
#endif

    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmStruct);
    return buf;
}

// Utility to safely extract text from a SQLite column.
// Returns empty string if the column is NULL.
inline std::string safeColumnText(sqlite3_stmt* stmt, int colIndex) {
    const unsigned char* text = sqlite3_column_text(stmt, colIndex);
    return text ? reinterpret_cast<const char*>(text) : "";
}

inline std::string normalizeWhitespace(const std::string& s)
{
    std::string result;
    result.reserve(s.size());

    bool inSpace = true; // Treat leading space as start
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!inSpace) {  // only insert one space for a sequence
                result.push_back(' ');
                inSpace = true;
            }
        }
        else {
            result.push_back(c);
            inSpace = false;
        }
    }

    // Remove trailing space if any
    if (!result.empty() && result.back() == ' ')
        result.pop_back();

    return result;
}
