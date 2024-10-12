cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)

project(AsterixParser VERSION 1.0 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Xml)

set(CMAKE_DEBUG_POSTFIX "d")

#linux .so output
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/buildlib/${CMAKE_BUILD_TYPE}")
#win .dll output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/buildlib/${CMAKE_BUILD_TYPE}")
#win .lib output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/buildlib/${CMAKE_BUILD_TYPE}")

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if (WIN32)
    add_compile_definitions(YY_NO_UNISTD_H)
endif()

add_library(asterixparser SHARED
    asterix.h
    asterix.cpp
    asterix.cpp
    asterix.h
    asterixparser.cpp
    asterixparser.h
    asterixparser_global.h
    bit.h
    scale_expression.bison.cpp
    scale_expression.bison.h
    scale_expression.flex.cpp
    scale_expression.flex.h
    scale_expression.ll
    scale_expression.y
    scaleexpressionparser.cpp
    scaleexpressionparser.h
    uap.cpp
    uap.h
)

target_compile_definitions(asterixparser
    PRIVATE ASTERIXPARSER_LIBRARY
)

target_link_libraries(asterixparser
    PRIVATE Qt::Core
    PRIVATE Qt::Xml
)
