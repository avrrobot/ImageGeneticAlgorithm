#-------------------------------------------------
#
# Project created by QtCreator 2014-05-16T19:13:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtTest
TEMPLATE = app

QMAKE_CXXFLAGS += -O3 -Wall

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    allpainter.cpp \
    geneticalgorithm.cpp \
    threadworker.cpp \
    graphpainter.cpp \
    genetic_threader.cpp

HEADERS  += mainwindow.h \
    allpainter.h \
    geneticalgorithm.h \
    threadworker.h \
    graphpainter.h \
    genetic_threader.h

FORMS    += mainwindow.ui
