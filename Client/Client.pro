TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Event.cpp \
        Mailbox_Manager.cpp \
        Process_Manager.cpp \
        Shared_Memory.cpp \
        Utility.cpp \
        main.cpp

HEADERS += \
    Event.h \
    Mailbox_Manager.h \
    Process_Manager.h \
    Shared_Memory.h \
    Utility.h
