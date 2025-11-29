# Project-level CMakeLists.txt for the main application
project(ComponentInventoryApp LANGUAGES CXX)

# Define the executable
add_executable(${PROJECT_NAME}
    src/main.cpp
)

# Link against the backend library
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        InventoryBackend
)

# Optional: warnings
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()