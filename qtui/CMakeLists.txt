find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(ComponentInventoryUI
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.ui
    include/mainwindow.h
    include/ComponentTableModel.h
    src/ComponentTableModel.cpp
    src/DevDataSeeder.cpp
    src/ComponentEditDialog.ui "include/ComponentEditDialog.h" "src/ComponentEditDialog.cpp" "include/AddLookupDialog.h" "src/AddLookupDialog.cpp")

target_include_directories(ComponentInventoryUI PRIVATE
    include
    ../InventoryBackend/include
)

target_link_libraries(ComponentInventoryUI PRIVATE
    InventoryBackend
    Qt6::Widgets
)

install(TARGETS ComponentInventoryUI DESTINATION bin)

# --- Windows Qt Runtime Deployment (for F5 Run) ---
if (WIN32)
    get_target_property(_qt6_qmake_location Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt6_bin_dir "${_qt6_qmake_location}" DIRECTORY)

    add_custom_command(TARGET ComponentInventoryUI POST_BUILD
        COMMAND "${_qt6_bin_dir}/windeployqt.exe"
        "$<TARGET_FILE:ComponentInventoryUI>"
        --no-translations
        --no-system-d3d-compiler
        --no-opengl-sw
    )
endif()
