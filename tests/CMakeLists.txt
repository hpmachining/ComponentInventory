# Project-level CMakeLists.txt for unit tests
project(InventoryBackendTests LANGUAGES CXX)

# Define the test executable
add_executable(${PROJECT_NAME}
    src/DatabaseTests.cpp
    src/SchemaManagerTests.cpp
    src/ComponentManagerTest.cpp
    src/CategoryManagerTest.cpp
    src/ManufacturerManagerTests.cpp
    src/ResistorPackageManagerTests.cpp
    src/ResistorCompositionManagerTests.cpp
    src/ResistorManagerTests.cpp
    src/CapacitorPackageManagerTests.cpp
    src/CapacitorDielectricManagerTests.cpp
    src/CapacitorManagerTests.cpp
    src/ElectrolyticManagerTests.cpp
    src/BJTManagerTests.cpp
    src/TransistorManagerTests.cpp)

# Link against backend and gtest
find_package(GTest CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        InventoryBackend
        GTest::gtest
        GTest::gtest_main
)

# Optional: warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# Register with CTest
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

