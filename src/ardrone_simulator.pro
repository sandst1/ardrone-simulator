#-------------------------------------------------
#
# Project created by QtCreator 2011-01-23T22:45:46
#
#-------------------------------------------------

QT       += core gui network

TARGET = ardrone_simulator
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    dronesim.cpp \
    atcommandparser.cpp \
    dronemodel.cpp \
    navdatagenerator.cpp

HEADERS  += mainwindow.h \
    dronesim.h \
    atcommandparser.h \
    dronemodel.h \
    navdatagenerator.h \
    ardrone_common.h

FORMS    += mainwindow.ui
