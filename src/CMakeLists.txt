cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)

project(AsterixParser VERSION 1.0 LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Xml)

add_library(asterixparser SHARED
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

target_compile_options(asterixparser PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8 /pdb>")

target_compile_definitions(asterixparser
    PRIVATE ASTERIXPARSER_LIBRARY
    PRIVATE $<$<BOOL:${WIN32}>:YY_NO_UNISTD_H>
)

target_link_libraries(asterixparser PRIVATE
    Qt::Core
    Qt::Xml
)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

set(EXPORT_HEADER
    asterixparser.h
    asterixparser_global.h
)

set_target_properties(asterixparser PROPERTIES
    DEBUG_POSTFIX "d"
    PUBLIC_HEADER "${EXPORT_HEADER}"
    PDB_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
)

install(TARGETS asterixparser
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/include
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/buildlib/${CMAKE_BUILD_TYPE}
)
