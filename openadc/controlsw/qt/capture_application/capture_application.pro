#-------------------------------------------------
#
# Project created by QtCreator 2013-06-12T14:52:01
#
#-------------------------------------------------

QT       += core gui

TARGET = capture_application
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../openadc_dialog/openadc_qt.cpp

HEADERS  += mainwindow.h \
    ../openadc_dialog/openadc_qt.h

FORMS    += mainwindow.ui \
    ../openadc_dialog/openadc_qt.ui
