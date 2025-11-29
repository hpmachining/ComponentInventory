# Project-level CMakeLists.txt for InventoryBackend
project(InventoryBackend LANGUAGES CXX)

# Define as a static library (explicit)
add_library(${PROJECT_NAME} STATIC)

# Attach sources explicitly
target_sources(${PROJECT_NAME}
    PRIVATE
        src/CategoryManager.cpp
        src/ComponentManager.cpp
        src/Database.cpp
        src/ManufacturerManager.cpp
        src/ResistorCompositionManager.cpp
        src/ResistorManager.cpp
        src/ResistorPackageManager.cpp
        src/SchemaManager.cpp
 "include/DbUtils.h")

# Public headers live directly in include/
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Link against SQLite3
find_package(SQLite3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SQLite::SQLite3)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${SQLite3_INCLUDE_DIRS}   # <-- add this
)
# Optional: warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()