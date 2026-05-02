cmake_minimum_required(VERSION 3.16)
project(Calculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt" "/usr/local/opt/qt")
endif()

find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Парсер + вычислитель — один модуль (рекурсивный спуск).
set(PARSER_SOURCES
    src/parser.cpp
    src/parser.h
)

set(UI_SOURCES
    src/ui/main_window.cpp
    src/ui/main_window.h
    src/ui/theme.cpp
    src/ui/theme.h
    src/ui/button_layout.cpp
    src/ui/button_layout.h
    src/ui/format.cpp
    src/ui/format.h
)

add_executable(calculator
    src/main.cpp
    ${PARSER_SOURCES}
    ${UI_SOURCES}
)

target_include_directories(calculator PRIVATE src)
target_link_libraries(calculator PRIVATE Qt6::Widgets)

if(APPLE)
    set_target_properties(calculator PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Calculator"
    )
endif()

enable_testing()
add_executable(parser_tests
    tests/parser_tests.cpp
    ${PARSER_SOURCES}
)
target_include_directories(parser_tests PRIVATE src)
add_test(NAME parser_tests COMMAND parser_tests)
