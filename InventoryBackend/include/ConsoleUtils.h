#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

inline void configureConsoleUtf8() {
#ifdef _WIN32
    // On Windows, force console output to UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif
    // On Linux/macOS, terminals are already UTF-8 by default
}
