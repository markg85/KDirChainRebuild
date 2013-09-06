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
    kdirectoryprivate_p.h \
    alt-lru/AltLRU.hpp \
    alt-lru/AltLRU_Types.hpp \
    alt-lru/AltLRU_CacheEntry.hpp

# should be the path to your KF5 libs folder
LIBS += -L/home/mark/kde_programming/kf5/lib64

# these are always needed regardless of the above path
LIBS += -lkdecore -lkio -lkiocore -lkdeui -lgomp -lKCoreAddons -lki18n

# include paths to KDE includes, You should not need to set those once KF5 is released.
INCLUDEPATH += /home/mark/kde_programming/kf5/include
INCLUDEPATH += /home/mark/kde_programming/kf5/include/KDE

# Some compile flags
QMAKE_CXXFLAGS += -fopenmp -D_GLIBCXX_PARALLEL -std=c++11
