#-------------------------------------------------
#
# Project created by QtCreator 2011-01-23T22:45:46
#
#-------------------------------------------------

QT       += core gui network

TARGET = ardrone_simulator
TEMPLATE = app

AR_DRONE_SDK= $$(HOME)/ARDrone_SDK_1_5_Version_20101004

INCLUDEPATH += $${AR_DRONE_SDK}/ARDroneLib/Soft/Common \
               $${AR_DRONE_SDK}/ARDroneLib/VP_SDK \
               $${AR_DRONE_SDK}/ARDroneLib/VP_SDK/VP_Os/linux \
               $${AR_DRONE_SDK}/ARDroneLib/VP_SDK/VP_Os \
               $${AR_DRONE_SDK}/ARDroneLib/Soft/Lib \
               $${AR_DRONE_SDK}/ARDroneLib/VLIB \
               $${AR_DRONE_SDK}/ARDroneLib

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
    navdatagenerator.h

FORMS    += mainwindow.ui
