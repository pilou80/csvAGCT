#-------------------------------------------------
#
# Project created by QtCreator 2016-07-26T10:44:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = csvAGCT
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    fileinfocsv.cpp

HEADERS  += mainwidget.h \
    fileinfocsv.h

FORMS    += mainwidget.ui \
    fileinfocsv.ui
