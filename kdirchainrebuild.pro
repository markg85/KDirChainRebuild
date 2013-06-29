#-------------------------------------------------
#
# Project created by QtCreator 2012-12-27T19:47:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kdirchainrebuild
TEMPLATE = app


SOURCES += main.cpp \
    dirmodel.cpp \
    kdirlisterv2.cpp \
    kdirlisterv2_p.cpp \
    kdirectory.cpp \
    kdirectoryentry.cpp \
    kdirectoryprivate_p.cpp

HEADERS  += \
    dirmodel.h \
    kdirlisterv2.h \
    kdirlisterv2_p.h \
    kdirectory.h \
    kdirectoryentry.h \
    kdirectoryprivate_p.h

LIBS += -lkdecore -lkio -lkdeui -lgomp -L/home/mark/kde_programming/kdelibs_git_build/lib
INCLUDEPATH += /usr/include/KDE
QMAKE_CXXFLAGS += -fopenmp -D_GLIBCXX_PARALLEL -std=c++11
