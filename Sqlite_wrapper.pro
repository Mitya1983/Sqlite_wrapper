TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += \
    -lstdc++fs\
    -lsqlite3\
    -lpthread

SOURCES += \
        exceptions.cpp \
        main.cpp \
        sqlite_wrapper.cpp

HEADERS += \
    exceptions.h \
    sqlite_wrapper.h
