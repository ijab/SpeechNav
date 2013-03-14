
QTITANDIR = $$quote($$(QTITANDIR))

include($$QTITANDIR/src/shared/platform.pri)

QTITAN_LIB_PATH = $$QTITANDIR/lib$$QTITAN_LIB_PREFIX

INCLUDEPATH += $$QTITANDIR/include

win32-msvc|win32-msvc.net|win32-msvc2002|win32-msvc2003|win32-msvc2005|win32-msvc2008|win32-msvc2010 {

    CONFIG(debug, debug|release):LIBS += $$QTITAN_LIB_PATH/qtnribbond2.lib
        else:LIBS += $$QTITAN_LIB_PATH/qtnribbon2.lib
}

win32-g++ {

    LIBS += -L$$QTITAN_LIB_PATH

    CONFIG(debug, debug|release):LIBS += -lqtnribbond2
        else:LIBS += -lqtnribbon2
}

unix {

    LIBS += -L$$QTITAN_LIB_PATH

    CONFIG(debug, debug|release):LIBS += -lqtnribbond
        else:LIBS += -lqtnribbon
}

