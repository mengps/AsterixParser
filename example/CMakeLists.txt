cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)

project(Example VERSION 1.0 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Xml)

add_executable(Example
    main.cpp
)

#传递目录
target_compile_definitions(Example PRIVATE PWD_PATH=${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Example
    PRIVATE Qt::Core
    PRIVATE asterixparser
)

target_include_directories(Example
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src
)
