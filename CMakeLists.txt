cmake_minimum_required(VERSION 3.24)
project(qtchess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg WebEngineWidgets)
qt_standard_project_setup()

set(CMAKE_AUTOUIC_SEARCH_PATHS "src/gui/forms")

file(GLOB_RECURSE SRC_CPP src/*.cpp)
file(GLOB_RECURSE SRC_HPP src/*.hpp)
file(GLOB_RECURSE UI_FORMS src/gui/*.ui)
file(COPY src/assets DESTINATION .)
include_directories(src)

add_executable(qtchess src/main.cpp ${SRC_CPP} ${FORMS_HDR})
target_link_libraries(qtchess Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg Qt6::WebEngineWidgets)
add_compile_options(qtchess "-Wall")
add_compile_options(qtchess "-Wextra")
