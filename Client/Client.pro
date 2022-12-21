TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Mailbox_Manager.cpp \
        Process_Manager.cpp \
        main.cpp

HEADERS += \
    Mailbox_Manager.h \
    Process_Manager.h
