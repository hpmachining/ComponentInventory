cmake_minimum_required(VERSION 3.16)

# Solution-level project
project(ComponentInventory LANGUAGES CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Require modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add subprojects
add_subdirectory(InventoryBackend)
add_subdirectory(ComponentInventory)
add_subdirectory(tests)
add_subdirectory(qtui)

