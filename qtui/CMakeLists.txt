find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(ComponentInventoryUI
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.ui
    include/mainwindow.h
)

target_include_directories(ComponentInventoryUI PRIVATE include)
target_link_libraries(ComponentInventoryUI PRIVATE Qt6::Widgets Qt6::Sql)

install(TARGETS ComponentInventoryUI DESTINATION bin)
