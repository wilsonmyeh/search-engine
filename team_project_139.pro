######################################################################
# Automatically generated by qmake (2.01a) Mon May 5 18:38:46 2014
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QMAKE_CXX = g++
QMAKE_LINK = g++
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS -= -stdlib=libc++
QMAKE_LFLAGS -= -stdlib=libc++
CONFIG += debug
DESTDIR = bin
OBJECTS_DIR = bin

# Input
HEADERS += src/mainwin.h \
           src/msort.h \
           src/pagerank.h \
           src/pagewin.h \
           src/Set.h \
           src/WebPage.h
SOURCES += src/Main.cpp src/mainwin.cpp src/pagewin.cpp src/WebPage.cpp
