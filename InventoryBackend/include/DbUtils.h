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

inline std::string trim(const std::string& s)
{
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start])))
        ++start;

    if (start == s.size())
        return {};

    size_t end = s.size() - 1;
    while (end > start && std::isspace(static_cast<unsigned char>(s[end])))
        --end;

    return s.substr(start, end - start + 1);
}
