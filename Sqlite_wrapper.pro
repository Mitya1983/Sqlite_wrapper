TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += \
    -lstdc++fs\
    -lsqlite3\
    -lpthread

SOURCES += \
        main.cpp \
        query.cpp \
        result.cpp \
        sqlite_wrapper.cpp

HEADERS += \
    query.h \
    result.h \
    sqlite_wrapper.h
