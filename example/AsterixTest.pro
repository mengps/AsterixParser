QT = core xml

CONFIG += c++17 cmdline debug_and_release

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += PWD_PATH=$$PWD

HEADERS +=

SOURCES += \
        main.cpp

INCLUDEPATH += $$PWD/../src

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../buildlib/debug -lasterixparserd
} else {
    LIBS += -L$$PWD/../buildlib/release -lasterixparser
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
