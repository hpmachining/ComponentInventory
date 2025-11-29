cmake_minimum_required(VERSION 3.16)

# Solution-level project
project(ComponentInventory LANGUAGES CXX)

enable_testing()

# Require modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add subprojects
add_subdirectory(InventoryBackend)
add_subdirectory(ComponentInventory)
add_subdirectory(tests)

