QT -= gui
QT += xml

TEMPLATE = lib
DEFINES += ASTERIXPARSER_LIBRARY

CONFIG += c++17

win32 {
    DEFINES += YY_NO_UNISTD_H
}

TARGET = $$qtLibraryTarget(asterixparser)

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/buildlib/debug
} else {
    DESTDIR = $$PWD/buildlib/release
}

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/asterix.cpp \
    src/asterixparser.cpp \
    src/scale_expression.bison.cpp \
    src/scale_expression.flex.cpp \
    src/scaleexpressionparser.cpp \
    src/uap.cpp

HEADERS += \
    src/asterixparser_global.h \
    src/asterix.h \
    src/asterixparser.h \
    src/bit.h \
    src/scale_expression.bison.h \
    src/scale_expression.flex.h \
    src/scaleexpressionparser.h \
    src/uap.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
