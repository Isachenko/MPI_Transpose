TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += qt

SOURCES += main.cpp \
    isach_mpi_logger.cpp

INCLUDEPATH += /usr/include/mpich2/
LIBS += -lmpich -lopa -lpthread -lrt
QMAKE_CXXFLAGS += -Bsymbolic-functions

CONFIG += -std = c++11

HEADERS += \
    isach_mpi_readMatrixFromFIle.h \
    isach_mpi_transposition.h \
    isach_mpi_logger.h
