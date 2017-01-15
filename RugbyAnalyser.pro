#-------------------------------------------------
#
# Project created by QtCreator 2016-10-26T22:43:10
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RugbyAnalyser
TEMPLATE = app


SOURCES += main.cpp\
        menuwindow.cpp \
    keyeventoptions.cpp \
    querybuilder.cpp \
    querydb.cpp \
    resultswindow.cpp \
    viewdb.cpp \
    dataanalyser.cpp

HEADERS  += menuwindow.h \
    keyeventoptions.h \
    querybuilder.h \
    querydb.h \
    resultswindow.h \
    viewdb.h \
    dataanalyser.h

FORMS    += menuwindow.ui \
    keyeventoptions.ui \
    resultswindow.ui \
    viewdb.ui
