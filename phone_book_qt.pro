QT += widgets sql

TARGET = PhoneBook
TEMPLATE = app

CONFIG += c++17

SOURCES += \
    main.cpp \
    contact.cpp \
    contactbook.cpp \
    mainwindow.cpp \
    contactdialog.cpp \
    checker.cpp \
    databaseconnection.cpp \
    contactdao.cpp \
    databasesettingsdialog.cpp

HEADERS += \
    contact.h \
    contactbook.h \
    mainwindow.h \
    contactdialog.h \
    checker.h \
    databaseconnection.h \
    contactdao.h \
    databasesettingsdialog.h
